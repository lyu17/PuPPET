// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ASTControlStatement.h"
#include "ASTKernel.h"
#include "ASTAppModel.h"

// ****************************************************************************

ASTControlStatement::ASTControlStatement()
{
    static int nindex = 0;
    char tmp[100];
    sprintf(tmp, "%03d", nindex+100);
    nodeid = tmp;
    ++nindex;
}


// ****************************************************************************

ASTControlKernelCallStatement::ASTControlKernelCallStatement(string name, ParseVector<ASTExpression*> args)
    : ASTControlStatement(), module(""), name(name), arguments(args.begin(), args.end())
{
}

ASTControlKernelCallStatement::ASTControlKernelCallStatement(string module, string name, ParseVector<ASTExpression*> args)
    : ASTControlStatement(), module(module), name(name), arguments(args.begin(), args.end())
{
}

ASTControlKernelCallStatement::~ASTControlKernelCallStatement()
{
    for (unsigned int i=0; i<arguments.size(); i++)
        delete arguments[i];
}

void
ASTControlKernelCallStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': kernel call "<<name;
    if (module != "")
        out << " in module "<<module;
    if (arguments.size() > 0)
    {
        out << " with "<<arguments.size()<<" args:" << endl;
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            arguments[i]->Print(out, indent+1);
        }
    }
    else
        out << endl;
}

void
ASTControlKernelCallStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent);
    if (module!="")
        out << module << ".";
    out << name;
    if (arguments.size() > 0)
    {
        out << "(";
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            out << arguments[i]->GetText(ASTExpression::ASPEN);
            if (i+1<arguments.size())
                out << ", ";
        }
        out << ")";
    }
    out << endl;
}

ASTExpression*
ASTControlKernelCallStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const
{
    string fqname = (module=="") ? name : (module + "." + name);

    // If we can't find what we're looking for, just return the name.
    // (This is intentional; a NULL app means do no expansion.)
    if (!app)
        return new ASTIdentifier(fqname);

    // Find the module
    const ASTAppModel *appForCall = app;
    if (module != "")
    {
        if (! app->importedModels.Contains(module))
            return new ASTIdentifier(fqname);
        appForCall = app->importedModels[module];
    }

    if (appForCall->kernelMap.Contains(name))
    {
        ExprPtr ret(appForCall->kernelMap[name]->GetResourceRequirementExpression(appForCall,resource));
        ///\todo: hack: we can't yet do expansion within a module.
        /// if we're in one, force it now.  The probably only works one
        /// level deep.
        if (appForCall != app)
            ret = ExprPtr(ret->Expanded(appForCall->paramMap));
        return ret->Cloned();
    }

    return new ASTIdentifier(fqname);

}

ASTExpression*
ASTControlKernelCallStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    string fqname = (module=="") ? name : (module + "." + name);

    // If we can't find what we're looking for, just return the name.
    // (This is intentional; a NULL app means do no expansion.)
    if (!app)
        return new ASTIdentifier(fqname);

    // Find the module
    const ASTAppModel *appForCall = app;
    if (module != "")
    {
        if (! app->importedModels.Contains(module))
            THROW(ModelError, "Can't find model for kernel");
        appForCall = app->importedModels[module];
    }

    if (appForCall->kernelMap.Contains(name))
    {
        ///\todo: ugh, const cast.  need const app model in arg list.
        ASTAppModel *appForCall_nonconst = const_cast<ASTAppModel*>(appForCall);
        ExprPtr ret(appForCall->kernelMap[name]->GetTimeExpression(appForCall_nonconst,mach,sockettype));
        ///\todo: hack: we can't yet do expansion within a module.
        /// if we're in one, force it now.  The probably only works one
        /// level deep.
        if (appForCall != app)
            ret = ExprPtr(ret->Expanded(appForCall->paramMap));
        return ret->Cloned();
    }

    THROW(ModelError, "Can't find kernel");
}


set<string>
ASTControlKernelCallStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;

    string fqname = (module=="") ? name : (module + "." + name);

    // If we can't find what we're looking for, just return the name.
    // (This is intentional; a NULL app means do no expansion.)
    if (!app)
        return arrays;

    // Find the module
    const ASTAppModel *appForCall = app;
    if (module != "")
    {
        if (! app->importedModels.Contains(module))
            return arrays;
        appForCall = app->importedModels[module];
    }

    if (appForCall->kernelMap.Contains(name))
    {
        return appForCall->kernelMap[name]->GetInclusiveDataArrays(app);
    }

    return arrays;
}


// ****************************************************************************


ASTControlIterateStatement::ASTControlIterateStatement(ASTExpression *q, ASTControlStatement *item)
    : ASTControlStatement(), quantity(q), item(item)
{
}

ASTControlIterateStatement::~ASTControlIterateStatement()
{
    delete quantity;
    delete item;
}

void
ASTControlIterateStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': iterate:" << endl;
    out << Indent(indent+1) << "Quantity:" << endl;
    quantity->Print(out, indent+2);
    item->Print(out, indent+1);
}

void
ASTControlIterateStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "iterate ["<<quantity->GetText(ASTExpression::ASPEN)<<"] {" << endl;
    item->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

ASTExpression*
ASTControlIterateStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                     string resource) const
{
    return new ASTBinaryExpression("*", quantity->Cloned(),
                                   item->GetResourceRequirementExpression(app, resource));
}

ASTExpression*
ASTControlIterateStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    return new ASTBinaryExpression("*", quantity->Cloned(),
                                   item->GetTimeExpression(app, mach, sockettype));
}

set<string>
ASTControlIterateStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return item->GetInclusiveDataArrays(app);
}

// ****************************************************************************

ASTControlMapStatement::ASTControlMapStatement(ASTExpression *q, ASTControlStatement *item)
    : ASTControlStatement(), quantity(q), item(item)
{
}

ASTControlMapStatement::~ASTControlMapStatement()
{
    delete quantity;
    delete item;
}

void
ASTControlMapStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': map:" << endl;
    out << Indent(indent+1) << "Quantity:" << endl;
    quantity->Print(out, indent+2);
    item->Print(out, indent+1);
}

void
ASTControlMapStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "iterate ["<<quantity->GetText(ASTExpression::ASPEN)<<"] {" << endl;
    item->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

ASTExpression*
ASTControlMapStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                                     string resource) const
{
    return new ASTBinaryExpression("*", quantity->Cloned(),
                                   item->GetResourceRequirementExpression(app, resource));
}

ASTExpression*
ASTControlMapStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    return new ASTBinaryExpression("*", quantity->Cloned(),
                                   item->GetTimeExpression(app, mach, sockettype));
}

set<string>
ASTControlMapStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    return item->GetInclusiveDataArrays(app);
}

// ****************************************************************************

ASTControlParallelStatement::ASTControlParallelStatement(ParseVector<ASTControlStatement*> items)
    : ASTControlStatement(), items(items.begin(), items.end())
{
    ///\todo: assert items->size() > 0
}

ASTControlParallelStatement::~ASTControlParallelStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
}

void
ASTControlParallelStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': parallel:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
    {
        items[i]->Print(out, indent+1);
    }
}

void
ASTControlParallelStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "par {" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

ASTExpression*
ASTControlParallelStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                           string resource) const
{
    ASTExpression *expr = items[0]->GetResourceRequirementExpression(app, resource);
    for (unsigned int i=1; i<items.size(); ++i)
    {
        expr = new ASTBinaryExpression("+", expr,
                                       items[i]->GetResourceRequirementExpression(app, resource));
    }
    return expr;
}

ASTExpression*
ASTControlParallelStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    ASTExpression *expr = items[0]->GetTimeExpression(app, mach, sockettype);
    for (unsigned int i=1; i<items.size(); ++i)
    {
        expr = new ASTBinaryExpression("+", expr,
                                       items[i]->GetTimeExpression(app, mach, sockettype));
    }
    return expr;
}

set<string>
ASTControlParallelStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;
    for (unsigned int i=0; i<items.size(); ++i)
    {
        set<string> newarrays = items[i]->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}


// ****************************************************************************

ASTControlSequentialStatement::ASTControlSequentialStatement(ParseVector<ASTControlStatement*> items)
    : ASTControlStatement(), items(items.begin(), items.end())
{
    ///\todo: assert items->size() > 0
}

ASTControlSequentialStatement::~ASTControlSequentialStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
}

void
ASTControlSequentialStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': sequential:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
    {
        items[i]->Print(out, indent+1);
    }
}

void
ASTControlSequentialStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "seq {" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
}

ASTExpression*
ASTControlSequentialStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                           string resource) const
{
    ASTExpression *expr = items[0]->GetResourceRequirementExpression(app, resource);
    for (unsigned int i=1; i<items.size(); ++i)
    {
        expr = new ASTBinaryExpression("+", expr,
                                       items[i]->GetResourceRequirementExpression(app, resource));
    }
    return expr;
}

ASTExpression*
ASTControlSequentialStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    ASTExpression *expr = items[0]->GetTimeExpression(app, mach, sockettype);
    for (unsigned int i=1; i<items.size(); ++i)
    {
        expr = new ASTBinaryExpression("+", expr,
                                       items[i]->GetTimeExpression(app, mach, sockettype));
    }
    return expr;
}

set<string>
ASTControlSequentialStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;
    for (unsigned int i=0; i<items.size(); ++i)
    {
        set<string> newarrays = items[i]->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}

// ****************************************************************************

ASTControlIfThenStatement::ASTControlIfThenStatement(ASTExpression *comparison,
                                                     ASTControlStatement *thenitem,
                                                     ASTControlStatement *elseitem)
    : ASTControlStatement(), comparison(comparison), thenitem(thenitem), elseitem(elseitem)
{
}

ASTControlIfThenStatement::~ASTControlIfThenStatement()
{
    delete comparison;
    delete thenitem;
    if (elseitem)
        delete elseitem;
}

void
ASTControlIfThenStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': ifthen:" << endl;
    out << Indent(indent+1) << "Condition:" << endl;
    comparison->Print(out, indent+2);
    out << Indent(indent+1) << "Then:" << endl;
    thenitem->Print(out, indent+2);
    if (elseitem)
    {
        out << Indent(indent+1) << "Else:" << endl;
        elseitem->Print(out, indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else for IfThen" << endl;
    }

}

void
ASTControlIfThenStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "if (" << comparison->GetText(ASTExpression::ASPEN) << ") {" << endl;
    thenitem->Export(out, indent+1);
    out << Indent(indent) << "}" << endl;
    if (elseitem)
    {
        out << Indent(indent) << "else {" << endl;
        elseitem->Export(out, indent+1);
        out << Indent(indent) << "}" << endl;
    }
}

ASTExpression*
ASTControlIfThenStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                                     string resource) const
{
    return new ASTIfThenExpression(comparison->Cloned(),
                                   thenitem->GetResourceRequirementExpression(app, resource),
                                   elseitem ? elseitem->GetResourceRequirementExpression(app, resource) : new ASTReal(0));
}

ASTExpression*
ASTControlIfThenStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    return new ASTIfThenExpression(comparison->Cloned(),
                                   thenitem->GetTimeExpression(app, mach, sockettype),
                                   elseitem ? elseitem->GetTimeExpression(app, mach, sockettype) : new ASTReal(0));
}

set<string>
ASTControlIfThenStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays = thenitem->GetInclusiveDataArrays(app);
    if (elseitem)
    {
        set<string> newarrays = elseitem->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}

// ****************************************************************************

ASTProbabilityItem::ASTProbabilityItem(ASTExpression *quantity, ASTControlStatement *item)
    : quantity(quantity), item(item)
{
}

ASTProbabilityItem::~ASTProbabilityItem()
{
}

void
ASTProbabilityItem::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "With probability:" << endl;
    quantity->Print(out, indent+1);
    out << Indent(indent) << "do:" << endl;
    item->Print(out, indent+1);
}

void
ASTProbabilityItem::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "[" << quantity->GetText(ASTExpression::ASPEN) << "] {";
    item->Export(out, indent+1);
    out << Indent(indent) << "}";
}

// ****************************************************************************

ASTControlProbabilityStatement::ASTControlProbabilityStatement(ParseVector<ASTProbabilityItem*> items,
                                                               ASTControlStatement *elseitem)
    : ASTControlStatement(), items(items.begin(), items.end()), elseitem(elseitem)
{
    ///\todo: assert items->size() > 0
}

ASTControlProbabilityStatement::~ASTControlProbabilityStatement()
{
    for (unsigned int i=0; i<items.size(); i++)
        delete items[i];
    if (elseitem)
        delete elseitem;
}

void
ASTControlProbabilityStatement::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Control node '"<<nodeid<<"': probability:" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Print(out, indent+1);
    if (elseitem)
    {
        out << Indent(indent+1) << "Else do:" << endl;
        elseitem->Print(out, indent+2);
    }
    else
    {
        out << Indent(indent+1) << "No Else for Probability" << endl;
    }
}

void
ASTControlProbabilityStatement::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "probability {" << endl;
    for (unsigned int i=0; i<items.size(); i++)
        items[i]->Export(out, indent+1);
    if (elseitem)
    {
        out << Indent(indent+1) << "else {" << endl;
        elseitem->Export(out, indent+2);
        out << Indent(indent+1) << "}" << endl;
    }
}

ASTExpression*
ASTControlProbabilityStatement::GetResourceRequirementExpression(const ASTAppModel *app,
                                              string resource) const
{
    ASTExpression *sumprob = NULL;
    ASTExpression *expr = NULL;

    for (unsigned int i=0; i<items.size(); ++i)
    {
        const ASTExpression *qi = items[i]->quantity;
        ASTExpression *tmp = new ASTBinaryExpression("*", qi->Cloned(),
                                                     items[i]->item->GetResourceRequirementExpression(app, resource));
        if (expr)
            expr = new ASTBinaryExpression("+", expr, tmp);
        else
            expr = tmp;
        // if we have an "else", we need to add up all the probabilities we encountered so far
        if (elseitem)
        {
            if (sumprob)
                sumprob = new ASTBinaryExpression("+", sumprob, qi->Cloned());
            else
                sumprob = qi->Cloned();
        }
    }

    // if we have an else, add it here
    if (elseitem)
    {
        // the else probability is 1 - the sum of the other probabilities
        ASTExpression *elseprob  = new ASTBinaryExpression("-", new ASTReal(1.0), sumprob);
        // weight the else item by that probability
        ASTExpression *elseexpr = new ASTBinaryExpression("*", elseprob, 
                                                          elseitem->GetResourceRequirementExpression(app, resource));
        // and add it to th final expression
        expr = new ASTBinaryExpression("+", expr, elseexpr);
    }

    if (!expr)
        THROW(ModelError, "Couldn't create valid expression for probability statement");

    return expr;
}

ASTExpression*
ASTControlProbabilityStatement::GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    ///\todo: lots of code duplication
    ASTExpression *sumprob = NULL;
    ASTExpression *expr = NULL;

    for (unsigned int i=0; i<items.size(); ++i)
    {
        const ASTExpression *qi = items[i]->quantity;
        ASTExpression *tmp = new ASTBinaryExpression("*", qi->Cloned(),
                                                     items[i]->item->GetTimeExpression(app, mach, sockettype));
        if (expr)
            expr = new ASTBinaryExpression("+", expr, tmp);
        else
            expr = tmp;
        // if we have an "else", we need to add up all the probabilities we encountered so far
        if (elseitem)
        {
            if (sumprob)
                sumprob = new ASTBinaryExpression("+", sumprob, qi->Cloned());
            else
                sumprob = qi->Cloned();
        }
    }

    // if we have an else, add it here
    if (elseitem)
    {
        // the else probability is 1 - the sum of the other probabilities
        ASTExpression *elseprob  = new ASTBinaryExpression("-", new ASTReal(1.0), sumprob);
        // weight the else item by that probability
        ASTExpression *elseexpr = new ASTBinaryExpression("*", elseprob, 
                                                          elseitem->GetTimeExpression(app, mach, sockettype));
        // and add it to th final expression
        expr = new ASTBinaryExpression("+", expr, elseexpr);
    }

    if (!expr)
        THROW(ModelError, "Couldn't create valid expression for probability statement");

    return expr;
}

set<string>
ASTControlProbabilityStatement::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays;
    for (unsigned int i=0; i<items.size(); ++i)
    {
        set<string> newarrays = items[i]->item->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }
    return arrays;
}
