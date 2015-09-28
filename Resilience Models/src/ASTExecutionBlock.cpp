#include "ASTExecutionBlock.h"
#include "ASTMachModel.h"

ASTExecutionBlock::ASTExecutionBlock(string name,
                                     ASTExpression *parallelism,
                                     ParseVector<ASTExecutionStatement*> statements)
    : name(name),
      parallelism(parallelism),
      statements(statements.begin(), statements.end())
{
}

void
ASTExecutionBlock::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Execution Block";
    if (name != "")
        out << " (name='"<<name<<"')";
    out <<" (control node '"<<nodeid<<"')"<<endl;
    if (parallelism)
    {
        out << Indent(indent+1) << "Parallelism:"<<endl;
        parallelism->Print(out, indent+2);
    }
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Print(out, indent+1);
    }
}

void
ASTExecutionBlock::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "execute ";
    if (name != "")
        out << name << " ";
    if (parallelism)
        out << "["<<parallelism->GetText(ASTExpression::ASPEN)<<"] ";
    out << "{" << endl;
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Export(out, indent+1);
    }
    out << Indent(indent) << "}" <<endl;
}

ASTExpression*
ASTExecutionBlock::GetResourceRequirementExpression(const ASTAppModel *app,
                                                    string resource) const
{
    ASTExpression *expr = NULL;

    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (req) // requires statement
        {
            // see if the resource matches; note that 'bytes' means loads or stores
            if (req->resource == resource || 
                (resource=="bytes" && (req->resource=="loads"||req->resource=="stores")))
            {
                if (!expr)
                    expr = req->quantity->Cloned();
                else
                    expr = new ASTBinaryExpression("+",expr,req->quantity->Cloned());
            }
        }
    }

    if (!expr)
        return new ASTReal(0);

    ASTExpression *par = GetParallelExpression();
    if (par)
        return new ASTBinaryExpression("*", par, expr);
    return expr;
}

set<string>
ASTExecutionBlock::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return GetDataArrays();
}

set<string>
ASTExecutionBlock::GetDataArrays() const
{
    set<string> arrays;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;
        if (req->tofrom != "")
            arrays.insert(req->tofrom);
    }
    return arrays;
}

ASTExpression*
ASTExecutionBlock::GetParallelExpression() const
{
    if (parallelism)
        return parallelism->Cloned();
    else
        return new ASTReal(1);
}

ASTExpression*
ASTExecutionBlock::GetDynamicEnergyExpressionForResource(string resource,
                                                         string sockettype,
                                                         ASTMachModel *mach) const
{
    // We do not use depth here; we just multiply the per-core power
    // by the total number of parallel work items; idle cores will 
    // be accounted for by static power.
    ASTExpression *par_nchunks = GetParallelExpression();

    ///\todo: to match scala-ASPEN1, we don't use the entire serial
    /// runtime, only the per-resource serial time.  I don't (yet)
    /// understand why, exactly.
    //ASTExpression *ser_runtime = GetSerialTimeExpression(mach, sockettype);
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;
        if (req->resource == resource)
        {
            // get the resource and traits
            vector<string> traits;
            for (unsigned t = 0; t < req->asitems.size(); ++t)
                traits.push_back(req->asitems[t]->name);

            // get the "serial" time expression for this resource
            ASTExpression *time =
                mach->GetMachine()->GetTimeExpression(resource, traits,
                                                      true, sockettype,
                                                      req->quantity);

            ASTExpression *rdp =
                mach->GetMachine()->GetSerialResourceDynamicPowerExpression(req->resource,
                                                                            sockettype);
            return new ASTBinaryExpression("*", par_nchunks,
                                           new ASTBinaryExpression("*",
                                                                   time,
                                                                   rdp));
        }
    }

    return new ASTReal(0);
}

struct resgrouper
{
    string resource;
    string tofrom;
    vector<string> traits;
    bool operator<(const resgrouper &b) const
    {
        if (resource < b.resource)
            return true;
        if (resource > b.resource)
            return false;
        if (tofrom < b.tofrom)
            return true;
        if (tofrom > b.tofrom)
            return false;
        if (traits.size() < b.traits.size())
            return true;
        if (traits.size() > b.traits.size())
            return false;
        int n = traits.size();
        for (int i=0; i<n; ++i)
        {
            if (traits[i] < b.traits[i])
                return true;
            if (traits[i] > b.traits[i])
                return false;
        }
        return false;
    }
};

ASTExpression*
ASTExecutionBlock::GetSerialTimeExpression(ASTAppModel *app,
                                           ASTMachModel *mach,
                                           string sockettype) const
{
    /// ---------
    /// Experimental (new): Group resource usage (and limit it appropriately) (plus some "smart" things like assuming stride-0 expressions are constant, etc.)
    /// ---------
#if 0


    map<resgrouper, ASTExpression*> resource_usage;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;
        resgrouper rg;
        rg.resource = req->resource;
        rg.tofrom   = req->tofrom;

        //// hack, skip intracomm!
        //if (rg.resource == "intracomm")
        //{
        //    resource_usage[rg] = new ASTReal(0);
        //    continue;
        //}

        bool skip = false;
        for (unsigned t = 0; t < req->asitems.size(); ++t)
        {
            if (req->asitems[t]->name == "stride")
            {
                try
                {
                    double stride = req->asitems[t]->expr->Evaluate();
                    if (stride == 0)
                        skip = true; // skip this resource requirement entirely; a stride of 0 means assume register usage
                    else if (stride <= 1)
                        continue; // skip this trait; we ignore the value for traits, but removing the stride trait means it's assumed to be continuous
                }
                catch (...)
                {
                    // ignore errors from evaluating the trait quantity
                }
            }
            rg.traits.push_back(req->asitems[t]->name);
        }
        if (skip)
            continue;
        if (resource_usage.count(rg))
            resource_usage[rg] = new ASTBinaryExpression("+", resource_usage[rg], req->quantity->Cloned());
        else
            resource_usage[rg] = req->quantity->Cloned();
    }

#if 1
    // clamp every let of loads/stores to max total size of a given array
    ///\todo: even this modification should probably be limited to stride-0 accesses
    for (map<resgrouper, ASTExpression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        if (rg.tofrom != "")
        {
            ASTExpression *size = app->GetSingleArraySize(rg.tofrom);
            if (size)
            {
                // simple option ignored parallelism
                //it->second = new ASTFunctionCall("min", it->second, size);

                // complex option; only read a total of 'size' bytes across ALL parallel instances
                ASTExpression *par = GetParallelExpression();
                it->second = new ASTFunctionCall("min", it->second, new ASTBinaryExpression("/",size,par));
            }
        }
    }
#endif

    // DEBUG
#if 1
    for (map<resgrouper, ASTExpression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        cerr << "rg=" << rg.resource << " tofrom=" << rg.tofrom << " traits=";
        for (unsigned int i=0; i<rg.traits.size(); ++i)
            cerr << rg.traits[i] << ",";
        cerr << " ::: " << it->second->GetText() << endl;
    }
    cerr << endl;
#endif

    // calculate times for each requirement and add them up
    map<string,ASTExpression*> requirements;
    for (map<resgrouper, ASTExpression*>::iterator it = resource_usage.begin();
         it != resource_usage.end(); ++it)
    {
        const resgrouper &rg = it->first;
        const ASTExpression *quantity = it->second;

        // get the "serial" time expression for this resource
        ASTExpression *time = mach->GetMachine()->GetTimeExpression(rg.resource, rg.traits, true, sockettype, quantity);
        //cerr << "   ---- time for resource '"<<resource<<"': "<<time->GetText() << endl;
        if (requirements[rg.resource])
            requirements[rg.resource] = new ASTBinaryExpression("+",requirements[rg.resource],time);
        else
            requirements[rg.resource] = time;
    }


#else

    /// ---------
    /// Standard (old): Calculate runtime for each resource statement independently
    /// ---------


    // Get the raw requirements for each type of resource
    map<string,ASTExpression*> requirements;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTExecutionStatement *s = statements[i];
        const ASTRequiresStatement *req = dynamic_cast<const ASTRequiresStatement*>(s);
        if (!req)
            continue;

        // get the resource and traits
        string resource = req->resource;
        vector<string> traits;

        ///\todo: hack: ignore memory traits, just use flops ones
        if (resource == "flops")
        {
            for (unsigned t = 0; t < req->asitems.size(); ++t)
                traits.push_back(req->asitems[t]->name);
        }

        // get the "serial" time expression for this resource
        ASTExpression *time = mach->GetMachine()->GetTimeExpression(resource, traits, true, sockettype, req->quantity);
        //cerr << "   ---- time for resource '"<<resource<<"': "<<time->GetText() << endl;
        if (requirements[resource])
            requirements[resource] = new ASTBinaryExpression("+",requirements[resource],time);
        else
            requirements[resource] = time;
    }
#endif

    // Fold any that conflict using "+"
    map<set<string>, ASTExpression*> conflicting;
    bool foundnewconflict = true;
    while (foundnewconflict)
    {
        foundnewconflict = false;

        // first, look for conflicts with an already-folded resource
        //cerr << "looking for conflicts with an already-folded resource\n";
        for (map<string,ASTExpression*>::iterator req = requirements.begin(); req != requirements.end(); req++)
        {
            string reqres = req->first;
            for (map<set<string>,ASTExpression*>::iterator conf = conflicting.begin(); conf != conflicting.end(); conf++)
            {
                for (set<string>::iterator confitem = conf->first.begin(); confitem != conf->first.end(); confitem++)
                {
                    if (mach->GetMachine()->CheckConflict(reqres, *confitem, sockettype))
                    {
                        //cerr << "   -- Found conflict between so-far-not-conflicting-item "<<reqres<<" and already-conflicting-item "<<*confitem<<endl;
                        foundnewconflict = true;
                        break;
                    }
                }
                if (foundnewconflict)
                {
                    set<string> newset(conf->first);
                    newset.insert(reqres);
                    conflicting[newset] = new ASTBinaryExpression("+", conf->second, req->second);
                    conflicting.erase(conf);
                    requirements.erase(req);
                }
                if (foundnewconflict)
                    break;
            }
            if (foundnewconflict)
                break;
        }

        // next, look for conflicts between iterms that have not yet been folded
        //cerr << "no conflicts with existing conflicting items; looking at unfolded pairs\n";
        for (map<string,ASTExpression*>::iterator req1 = requirements.begin(); req1 != requirements.end(); req1++)
        {
            string reqres1 = req1->first;
            //cerr << " (1) reqres1 = " << reqres1 << endl;
            for (map<string,ASTExpression*>::iterator req2 = requirements.begin(); req2 != requirements.end(); req2++)
            {
                string reqres2 = req2->first;
                //cerr << "    (2) reqres2 = " << reqres2 << endl;
                if (reqres1 != reqres2)
                {
                    if (mach->GetMachine()->CheckConflict(reqres1, reqres2, sockettype))
                    {
                        //cerr << "   -- Found conflict between so-far-not-conflicting-items "<<reqres1<<" and "<<reqres2<<endl;
                        set<string> newset;
                        newset.insert(reqres1);
                        newset.insert(reqres2);
                        conflicting[newset] = new ASTBinaryExpression("+", req1->second, req2->second);
                        foundnewconflict = true;
                        requirements.erase(req1);
                        requirements.erase(req2);
                    }
                }
                if (foundnewconflict)
                    break;
            }
            if (foundnewconflict)
                break;
        }

        //cerr << "foundnewconflict="<<foundnewconflict<<endl;
    }

    // We should have found all conflicting requirements
    // Whatever's left (from the individual or previously-folded
    // requirements) can be folded using Max()
    ASTExpression *runtime = NULL;
    for (map<string,ASTExpression*>::iterator req = requirements.begin(); req != requirements.end(); req++)
    {
        //cerr << "for "<<req->first<<", serial runtime expr = "<<req->second->GetText()<<endl;
        if (!runtime)
            runtime = req->second;
        else
            runtime = new ASTFunctionCall("max", runtime, req->second);
    }
    for (map<set<string>,ASTExpression*>::iterator conf = conflicting.begin(); conf != conflicting.end(); conf++)
    {
        //cerr << "for (conflicting set) serial runtime expr = "<<conf->second->GetText()<<endl;
        if (!runtime)
            runtime = conf->second;
        else
            runtime = new ASTFunctionCall("max", runtime, conf->second);
    }
    if (!runtime)
        THROW(ModelError, "Couldn't get serial time");
    return runtime;
}

ASTExpression*
ASTExecutionBlock::GetDepthExpression(ASTMachModel *mach,
                                      string sockettype) const
{
    ASTExpression *totalcores = mach->GetMachine()->GetTotalCoresExpression(sockettype);
    ASTExpression *par = GetParallelExpression();
    return new ASTFunctionCall("ceil", new ASTBinaryExpression("/", par, totalcores));
}

ASTExpression *
ASTExecutionBlock::GetTimeExpression(ASTAppModel *app,
                                     ASTMachModel *mach,
                                     string sockettype) const
{
    ASTExpression *runtime = GetSerialTimeExpression(app, mach, sockettype);
    ///\todo: since old control flows are now kernels, this now has
    /// the possibility of failing (and returning NULL).  Check for
    /// that case now and throw an error.
    if (!runtime)
        THROW(ModelError, "Couldn't get time expression of this execution block");
    ASTExpression *depth = GetDepthExpression(mach, sockettype);
    if (!depth)
        THROW(ModelError, "Couldn't get depth of this execution block");
    return new ASTBinaryExpression("*", depth, runtime);
}

