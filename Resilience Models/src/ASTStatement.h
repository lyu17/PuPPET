// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_STATEMENT_H
#define AST_STATEMENT_H

#include "AST.h"
#include "ASTExpression.h"
#include "ParseVector.h"

// ****************************************************************************
// Class:  ASTStatement
//
// Purpose:
///   Base class for all statements.  Major types of statement include
///   globals (param and data declarations and imports),
///   control (kernel calls, execution blocks, and other control flow) and
///   execution (resource requirements within execution blocks).
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTStatement : public ASTNode
{
};

class ASTAssignStatement : public ASTStatement
{
  public:
    string name;
    const ASTExpression *value;
  public:
    ASTAssignStatement(const string &n, ASTExpression *v)
        : name(n), value(v)
    {
    }
    virtual ~ASTAssignStatement()
    {
        delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Identifier "<<name<< " = " << endl;
        value->Print(out, indent+1);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "param "<<name<< " = " << value->GetText(ASTExpression::ASPEN) << endl;
    }
};

class ASTAssignRangeStatement : public ASTAssignStatement
{
  public:
    const ASTExpression *minval;
    const ASTExpression *maxval;
  public:
    ASTAssignRangeStatement(const string &n, ASTExpression *v,
                            ASTExpression *minval, ASTExpression *maxval)
        : ASTAssignStatement(n, v), minval(minval), maxval(maxval)
    {
    }
    virtual ~ASTAssignRangeStatement()
    {
        delete minval;
        delete maxval;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Identifier "<<name<< " = " << endl;
        out << Indent(indent+1) << "Range:" << endl;
        out << Indent(indent+2) << "Default value:" << endl;
        value->Print(out, indent+3);
        out << Indent(indent+2) << "Minimum value:" << endl;
        minval->Print(out, indent+3);
        out << Indent(indent+2) << "Maximum value:" << endl;
        maxval->Print(out, indent+3);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "param "<<name<< " = "
            << value->GetText(ASTExpression::ASPEN) << " in "
            << minval->GetText(ASTExpression::ASPEN) << " .. "
            << maxval->GetText(ASTExpression::ASPEN) << endl;
    }
};

class ASTDataStatement : public ASTStatement
{
  public:
    string name;
    const ASTExpression *quantity;
    const ASTFunctionCall *as;
  public:
    ASTDataStatement(const string &n, ASTExpression *q, ASTFunctionCall *a)
        : name(n), quantity(q), as(a)
    {
    }
    virtual ~ASTDataStatement()
    {
        delete quantity;
        delete as;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Data "<<name<<":"<<endl;
        if (quantity)
        {
            out << Indent(indent+1) << "Quantity:"<<endl;
            quantity->Print(out, indent+2);
        }
        if (as)
        {
            out << Indent(indent+1) << "As:"<<endl;
            as->Print(out, indent+2);
        }
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        if (quantity)
        {
            out << Indent(indent) << "data "<<name<<" ["<<quantity->GetText(ASTExpression::ASPEN)<<"]" << endl;
        }
        if (as)
        {
            out << Indent(indent) << "data "<<name<<" as "<<as->GetText(ASTExpression::ASPEN) << endl;
        }
    }
};

class ASTImportStatement : public ASTStatement
{
  public:
    string ident;
    string file;
    const vector<const ASTAssignStatement*> arguments;
  public:
    ASTImportStatement(string ident, string file, ParseVector<ASTAssignStatement*> args) : ident(ident), file(file), arguments(args.begin(), args.end()) { }
    virtual ~ASTImportStatement()
    {
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            delete arguments[i];
        }
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Import "<<ident<< " from ''"<<file<<"'' with "<<arguments.size()<<" args:" << endl;
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            arguments[i]->Print(out, indent+1);
        }
    }
};

#endif
