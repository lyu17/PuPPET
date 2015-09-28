// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_RESILIENCE_STATEMENT_H
#define AST_RESILIENCE_STATEMENT_H

#include "AST.h"
#include "ASTExpression.h"
#include "ParseVector.h"

// ****************************************************************************
// Class:  ASTResilienceStatement
//
// Purpose:
///   Base class for all resilience statements. Major types of statement include
///   globals (param and data declarations and imports),
///   control (kernel calls, execution blocks, and other control flow) and
///   execution (resource requirements within execution blocks).
//
// Programmer:  Li Yu
// Creation:    May 01, 2014
//
// Modifications:
// ****************************************************************************
class ASTResilienceStatement : public ASTNode
{
};

class ASTResilienceDataStatement : public ASTResilienceStatement
{
  public:
    string name;
    const ASTExpression *value;
  public:
    ASTResilienceDataStatement(const string &n, ASTExpression *v) : name(n), value(v)
    {
    }
    virtual ~ASTResilienceDataStatement()
    {
        delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Data Info: "<<name<< " = " << endl;
        value->Print(out, indent+1);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) <<name<< " = " << value->GetText(ASTExpression::ASPEN) << endl;
    }
};


#endif
