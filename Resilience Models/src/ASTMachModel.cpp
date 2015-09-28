// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "ASTMachModel.h"

ASTMachModel::ASTMachModel(ParseVector<ASTStatement*> globals,
                           ParseVector<ASTMachComponent*> components)
    : globals(globals.begin(),globals.end()), components(components.begin(),components.end())
{
    machine = NULL;
}

ASTMachModel::~ASTMachModel()
{
    for (unsigned int i=0; i<globals.size(); i++)
        delete globals[i];
    for (unsigned int i=0; i<components.size(); i++)
        delete components[i];
}

void
ASTMachModel::CompleteAndCheck()
{
    // ensure we don't do this call more than once
    if (!paramMap.Empty())
        return;

    // build the maps
    for (unsigned int i=0; i<globals.size(); ++i)
    {
        ASTAssignStatement *assign = dynamic_cast<ASTAssignStatement*>(globals[i]);
        if (assign)
            paramMap[assign->name] = assign->value;
    }
    for (unsigned int i=0; i<components.size(); ++i)
    {
        compMap[components[i]->name] = components[i];
        // find a machine
        ASTMachine *mach = dynamic_cast<ASTMachine*>(components[i]);
        if (mach)
            machine = mach;
    }

    // check all the components
    for (unsigned int i=0; i<components.size(); ++i)
    {
        components[i]->CompleteAndCheck(compMap);
    }
}

void
ASTMachModel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Machine model:" << endl;
    out << Indent(indent+1) << "Globals:" << endl;
    for (unsigned int i=0; i<globals.size(); i++)
    {
        globals[i]->Print(out, indent+2);
    }
    out << Indent(indent+1) << "Components:" << endl;
    for (unsigned int i=0; i<components.size(); i++)
    {
        components[i]->Print(out, indent+2);
    }
}

const ASTMachine*
ASTMachModel::GetMachine() const
{
    return machine;
}

ASTExpression*
ASTMachModel::GetTimeExpression(const string &resource,
                                const vector<string> &traits,
                                string sockettype,
                                ASTExpression *value) const
{
    ///\todo: do we really want value to be passed all the way down to the resource?
    /// in some sense, it does make sense, because for other
    /// uses, we might have different values at the time we need the
    /// resource...  I think the *right* thing to do might be at some
    /// point down the machine component hierarchy, value should be an
    /// expression instead of a number.  For the purposes here, a
    /// number is okay, but we'll need something a little more
    /// abstract (like ability to substitute for a new global var) for
    /// other purposes.

    // find a machine component
    return machine->GetTimeExpression(resource, traits, false, sockettype, value);
}

double
ASTMachModel::CountTime(const string &resource,
                        const vector<string> &traits,
                        string sockettype,
                        ASTExpression *value) const
{
    ExprPtr expr(GetTimeExpression(resource, traits, sockettype, value));
    return expr->Evaluate(paramMap);
}

string
ASTMachModel::GetTimeExprText(const string &resource,
                              const vector<string> &traits,
                              string sockettype,
                              bool expand,
                              ASTExpression *value,
                              const NameMap<const ASTExpression*> &subst) const
{
    ExprPtr expr(GetTimeExpression(resource, traits, sockettype, value));

    // do the substitutions first
    expr = ExprPtr(expr->Expanded(subst));

    // now expand if needed
    if (expand)
        expr = ExprPtr(expr->Expanded(paramMap));

    //expr->Print(cerr);
        
    return expr->GetText();
}
