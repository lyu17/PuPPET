// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ASTKernel.h"

#include "ASTMachModel.h"
#include "ASTControlStatement.h"
#include "ASTExecutionBlock.h"

ASTKernel::ASTKernel(const string &n,
                     ParseVector<ASTControlStatement*> statements,
                     ParseVector<ASTVarDecl*> args)
    : name(n),
      args(args.begin(), args.end()),
      statements(statements.begin(), statements.end())
{
}

ASTKernel::~ASTKernel()
{
    for (unsigned int i=0; i<statements.size(); i++)
        delete statements[i];
}

void
ASTKernel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Kernel "<<name<< ":" << endl;
    if (args.size() > 0)
    {
        out << Indent(indent+1) << "Args:" << endl;
        for (unsigned int i=0; i<args.size(); i++)
        {
            args[i]->Print(out, indent+2);
        }
    }
    out << Indent(indent+1) << "Statements:" << endl;
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Print(out, indent+2);
    }
}

void
ASTKernel::Export(ostream &out, int indent) const
{
    out << Indent(indent) << "kernel "<<name;
    if (args.size() > 0)
    {
        out << "(";
        for (unsigned int i=0; i<args.size(); i++)
        {
            args[i]->Export(out, indent);
            if (i+1<args.size())
                out << ", ";
        }
        out << ")";
    }
    out << " {" << endl;
    for (unsigned int i=0; i<statements.size(); i++)
    {
        statements[i]->Export(out, indent+1);
    }
    out << Indent(indent) << "}" << endl;
}

ASTExpression*
ASTKernel::GetResourceRequirementExpression(const ASTAppModel *app,
                                            string resource) const
{
    ASTExpression *expr = NULL;

    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        const ASTControlStatement *ctrl = dynamic_cast<const ASTControlStatement*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            ///\todo: now it's the same code as ctrl
            if (!expr)
                expr = exec->GetResourceRequirementExpression(app,resource);
            else
                expr = new ASTBinaryExpression("+", expr,
                                               exec->GetResourceRequirementExpression(app,resource));
        }
        else if (ctrl)
        {
            if (!expr)
                expr = ctrl->GetResourceRequirementExpression(app,resource);
            else
                expr = new ASTBinaryExpression("+", expr,
                                               ctrl->GetResourceRequirementExpression(app,resource));
        }
        else
        {
            //cerr << "UNKNOWN STMT in kernel '"<<name<<"': ";s->Print(cerr);
        }
    }

    if (!expr)
        return new ASTReal(0);

    return expr;
}

set<string>
ASTKernel::GetExclusiveDataArrays(string blockname) const
{
    set<string> arrays;

    // collect the list of data arrays
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            // if they specified an execution block name, only count that block's arrays
            if (blockname == "" || blockname == exec->name)
            {
                set<string> newarrays = exec->GetDataArrays();
                arrays.insert(newarrays.begin(), newarrays.end());
            }
        }
    }

    return arrays;
}

set<string>
ASTKernel::GetInclusiveDataArrays(const ASTAppModel *app) const
{
    set<string> arrays = GetExclusiveDataArrays();

    // collect the list of data arrays
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        set<string> newarrays = s->GetInclusiveDataArrays(app);
        arrays.insert(newarrays.begin(), newarrays.end());
    }

    return arrays;
}

ASTExpression*
ASTKernel::GetExclusiveDataSizeExpression(const ASTAppModel *app, 
                                          string blockname) const
{
    set<string> arrays = GetExclusiveDataArrays(blockname);
    return GetDataSizeExpression(app, arrays);
}

ASTExpression*
ASTKernel::GetInclusiveDataSizeExpression(const ASTAppModel *app) const
{
    set<string> arrays = GetInclusiveDataArrays(app);
    return GetDataSizeExpression(app, arrays);
}

///\todo: this whole function is a bit of a hack....:
ASTExpression*
ASTKernel::GetSingleMapInclusiveDataSizeExpression(const ASTAppModel *app,
                                                   string mapname) const
{
    set<string> arrays;

    // collect the list of data arrays
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        if (s->label == mapname)
        {
            set<string> newarrays = s->GetInclusiveDataArrays(app);
            arrays.insert(newarrays.begin(), newarrays.end());
        }
    }

    return GetDataSizeExpression(app, arrays);
}

ASTExpression*
ASTKernel::GetDataSizeExpression(const ASTAppModel *app, set<string> arrays) const
{
    ///\todo: we now have GetSingleArraySise in AppModel we could use.

    // for each data array, find its definition in the app global list
    ASTExpression *expr = NULL;
    for (set<string>::iterator it = arrays.begin(); it != arrays.end(); it++)
    {
        string arrayname = *it;
        bool found = false;
        for (unsigned int i=0; i<app->globals.size(); ++i)
        {
            const ASTStatement *s = app->globals[i];
            const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
            if (!ds || ds->name != arrayname)
                continue;
            found = true;
            // we now have the data array declaration of interest....
            ASTExpression *newexpr = NULL;

            // if it's a quantity, just return it, otherwise, we need to 
            // do some calculation to figure out the size
            if (ds->quantity)
                newexpr = ds->quantity->Cloned();
            else // assert ds->as ??
            {
                const ASTFunctionCall *as = ds->as;
                ///\todo: This is simplistic; we just assume that
                /// an array or matrix or whatever is just a list
                /// of expressions that get multiplied together.
                /// e.g. "data mat as Matrix(n,n,wordsize)"
                /// has a data set size of n*n*wordsize, and
                /// "Array(n,wordsize)" as a size of n*wordsize.
                for (unsigned int i=0; i<as->arguments.size(); ++i)
                {
                    if (newexpr)
                        newexpr = new ASTBinaryExpression("*", newexpr,
                                                          as->arguments[i]->Cloned());
                    else
                        newexpr = as->arguments[i]->Cloned();
                }
            }
            if (expr)
                expr = new ASTBinaryExpression("+", expr, newexpr);
            else
                expr = newexpr;
            break;
        }
        if (!found)
        {
            ///\todo: may need to do parameter substitution on DATA sets;
            /// we can't actually figure out the necessary array size
            /// until some kernels are called.  Otherwise, I'd like to 
            /// throw an error here.
            /// For now, how about returning a sentinel?
            cerr << "never found declaration for array "<<arrayname<<endl;
            return new ASTReal(-9999);
            //throw "Missing array declaration";
        }
    }

    if (!expr)
        return new ASTReal(0);
    else
        return expr;
}

ASTExpression*
ASTKernel::GetDynamicEnergyExpressionForResource(string resource,
                                                 string sockettype,
                                                 ASTMachModel *mach) const
{
    ///\todo:  is adding them together the right thing now that 
    // we can have multiple execution blocks within a kernel?
    ASTExpression *expr = NULL;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            if (!expr)
                expr = exec->GetDynamicEnergyExpressionForResource(resource,sockettype,mach);
            else
                expr = new ASTBinaryExpression("+", expr,
                                               exec->GetDynamicEnergyExpressionForResource(resource,sockettype,mach));
        }
    }

    if (!expr)
        return new ASTReal(0);

    return expr;
}

ASTExpression*
ASTKernel::GetSerialTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const
{
    ///\todo:  is adding them together the right thing now that 
    // we can have multiple execution blocks within a kernel?
    ASTExpression *expr = NULL;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        if (exec) // it's a kernel-like requires statement
        {
            if (!expr)
                expr = exec->GetSerialTimeExpression(app,mach,sockettype);
            else
                expr = new ASTBinaryExpression("+", expr,
                                               exec->GetSerialTimeExpression(app,mach,sockettype));
        }
    }

    if (!expr)
        return new ASTReal(0);

    return expr;
}

///\todo: code duplication (at least GetSerialTimeExpression with
/// GetTimeExpression)
ASTExpression*
ASTKernel::GetTimeExpression(ASTAppModel *app,
                             ASTMachModel *mach,
                             string socketbase,
                             NameMap<string> socketoverride) const
{
    ///\todo:  is adding them together the right thing now that 
    // we can have multiple execution blocks within a kernel?
    ASTExpression *expr = NULL;
    for (unsigned int i=0; i<statements.size(); ++i)
    {
        const ASTControlStatement *s = statements[i];
        const ASTExecutionBlock *exec = dynamic_cast<const ASTExecutionBlock*>(s);
        string sockettype = socketbase;
        if (exec) // it's a kernel-like requires statement
        {
            if (socketoverride.Contains(exec->name))
                sockettype = socketoverride[exec->name];
        }

        if (!expr)
            expr = s->GetTimeExpression(app,mach,sockettype);
        else
            expr = new ASTBinaryExpression("+", expr,
                                           s->GetTimeExpression(app,mach,sockettype));
    }

    if (!expr)
        return new ASTReal(0);

    return expr;
}

