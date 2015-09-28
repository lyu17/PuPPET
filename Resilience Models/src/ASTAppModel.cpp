// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ASTAppModel.h"

ASTAppModel::ASTAppModel(const string &n, ParseVector<ASTStatement*> globals, ParseVector<ASTKernel*> kernels)
    : name(n), globals(globals.begin(),globals.end()), kernels(kernels.begin(),kernels.end())
{
    mainKernel = NULL;
}

ASTAppModel::~ASTAppModel()
{
    for (unsigned int i=0; i<globals.size(); i++)
        delete globals[i];
    for (unsigned int i=0; i<kernels.size(); i++)
        delete kernels[i];
}

void
ASTAppModel::CompleteAndCheck()
{
    // ensure we don't do this call more than once
    if (!paramMap.Empty())
        return;

    for (unsigned int i=0; i<globals.size(); ++i)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(globals[i]);
        if (assign)
        {
            paramMap[assign->name] = assign->value;
        }
    }

    for (unsigned int i=0; i<kernels.size(); ++i)
        kernelMap[kernels[i]->name] = kernels[i];

    // Find the main kernel
    mainKernel = kernelMap["main"];
}

void
ASTAppModel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "App model "<<name<< ":" << endl;
    out << Indent(indent+1) << "Globals:" << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Print(out, indent+2);
    }
    out << Indent(indent+1) << "Kernels:" << endl;
    for (unsigned int i=0; i<kernels.size(); i++)
    {
        kernels[i]->Print(out, indent+2);
    }
}

void
ASTAppModel::Export(ostream &out, int indent) const
{
    out << "model " <<name<<" {" << endl;
    out << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Export(out, indent+1);
    }
    out << endl;
    for (unsigned int i=0; i<kernels.size(); i++)
    {
        kernels[i]->Export(out, indent+1);
    }
    out << endl;
    out << "}" << endl;
}

int
ASTAppModel::FindParametersWithRanges(vector<string> &names,
                                      vector<double> &defvals,
                                      vector<double> &minvals,
                                      vector<double> &maxvals) const
{
    ///\todo: we currently allow ranges to have expressions for their default and limit values.
    /// While convenient, it adds some pain (like having to create a paramMap to evaluate
    /// these values here) which may not be worth the effort.  We might want to restrict these to
    /// explicit values only, not expressions.
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTAssignRangeStatement *range = dynamic_cast<const ASTAssignRangeStatement*>(globals[i]);
        if (!range)
            continue;
        names.push_back(range->name);
        defvals.push_back(range->value->Evaluate(paramMap));
        minvals.push_back(range->minval->Evaluate(paramMap));
        maxvals.push_back(range->maxval->Evaluate(paramMap));
    }        
    return names.size();
}

ASTExpression*
ASTAppModel::GetResourceRequirementExpression(string resource) const
{
    if (!mainKernel)
        THROW(ModelError, "No main() kernel defined");

    return mainKernel->GetResourceRequirementExpression(this, resource);
}

ASTExpression*
ASTAppModel::GetControlFlowExpression() const
{
    if (!mainKernel)
        THROW(ModelError, "No main() kernel defined");

    ///\todo: A null AppModel gives it nothing to substitute.  This
    /// is a little ugly, though.
    return mainKernel->GetResourceRequirementExpression(NULL, "");
}

double
ASTAppModel::Count(string resource, string withparam, double withparamvalue) const
{
    ExprPtr expr(GetResourceRequirementExpression(resource));

    if (withparam != "")
    {
        ExprPtr real(new ASTReal(withparamvalue));
        NameMap<const ASTExpression*> subst(withparam, real.get());
        expr = ExprPtr(expr->Expanded(subst));
    }

    return expr->Evaluate(paramMap);
}

string
ASTAppModel::GetResourceRequirementExpressionText(string resource,
                                                  bool expand,
                                                  bool simplify,
                           const NameMap<const ASTExpression*> &subst) const
{
    ExprPtr expr(GetResourceRequirementExpression(resource));

    // do the substitutions first
    expr = ExprPtr(expr->Expanded(subst));

    // now expand if needed
    if (expand)
        expr = ExprPtr(expr->Expanded(paramMap));

    // and simplify if needed
    if (simplify)
    {
        int passes = 10;
        for (int i=0; i<passes; ++i)
            expr = ExprPtr(expr->Simplified());
    }
    return expr->GetText();
}

ASTExpression*
ASTAppModel::GetSingleArraySize(string array) const
{
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTStatement *s = globals[i];
        const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
        if (!ds)
            continue;

        ASTExpression *expr = NULL;

        // if it's a quantity, just return it, otherwise, we need to 
        // do some calculation to figure out the size
        if (ds->quantity)
            expr = ds->quantity->Cloned();
        else // assert ds->as ??
        {
            const ASTFunctionCall *as = ds->as;
            ///\todo: This is simplistic; we just assume that
            /// an array or matrix or whatever is just a list
            /// of expressions that get multiplied together.
            /// e.g. "data mat as Matrix(n,n,wordsize)"
            /// has a data set size of n*n*wordsize, and
            /// "Array(n,wordsize)" as a size of n*wordsize.

            ///\todo: assert as->arguments.size()>0
            for (unsigned int i=0; i<as->arguments.size(); ++i)
            {
                if (expr)
                    expr = new ASTBinaryExpression("*", expr,
                                                   as->arguments[i]->Cloned());
                else
                    expr = as->arguments[i]->Cloned();
            }
        }
        return expr;
    }

    return NULL;
}


ASTExpression*
ASTAppModel::GetGlobalArraySizeExpression() const
{
    ASTExpression *expr = NULL;
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        const ASTStatement *s = globals[i];
        const ASTDataStatement *ds = dynamic_cast<const ASTDataStatement*>(s);
        if (!ds)
            continue;

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
    }

    if (!expr)
        return new ASTReal(0);
    else
        return expr;
}

