// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_EXECUTION_STATEMENT_H
#define AST_EXECUTION_STATEMENT_H

#include "AST.h"

// ****************************************************************************
// Class:  ASTExecutionStatement
//
// Purpose:
///   Base class for any statements that might exist within an execution
///   block.  At the moment, that's only a resource requirement statement.
//
// Programmer:  Jeremy Meredith
// Creation:    September 19, 2013
//
// Modifications:
// ****************************************************************************

class ASTExecutionStatement : public ASTNode
{
};

class ASTRequiresStatement : public ASTExecutionStatement
{
  public:
    const string resource;
    const ASTExpression *quantity;
    const ASTExpression *count;
    const ASTExpression *size;
    const string tofrom;
    const vector<const ASTAsItem*> asitems;
  public:
    ASTRequiresStatement(string r, ASTExpression *count, ASTExpression *size, string tf, ParseVector<ASTAsItem*> a)
        : resource(r), count(count), size(size), tofrom(tf), asitems(a.begin(), a.end())
    {
        if (size)
            quantity = new ASTBinaryExpression("*", count, size);
        else
            quantity = count;
    }
    virtual ~ASTRequiresStatement()
    {
        delete quantity;
        for (unsigned int i=0; i<asitems.size(); ++i)
        {
            delete asitems[i];
        }
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Requires "<<resource << " of quantity: " << endl;
        quantity->Print(out, indent+1);
        // if we don't have a size, then quantity==count, so don't
        // try to separate the two when printing
        if (size)
        {
            out << Indent(indent+1) << "Where count is: " << endl;
            count->Print(out, indent+2);
            out << Indent(indent+1) << "And size is: " << endl;
            size->Print(out, indent+2);
        }
        if (tofrom != "")
            out << Indent(indent+1) << "To/From: "<<tofrom << endl;
        for (unsigned int i=0; i<asitems.size(); i++)
        {
            asitems[i]->Print(out, indent+1);
        }
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        ///\todo: do '[count] of size [size]' if size is not NULL
        out << Indent(indent) <<resource << " [" << quantity->GetText(ASTExpression::ASPEN) << "]";
        if (tofrom != "")
        {
            ///\todo: guessing to/from here.
            if (resource=="loads")
                out << " from " << tofrom;
            else
                out << " to " << tofrom;
        }
        if (asitems.size() > 0)
            out << " as ";
        for (unsigned int i=0; i<asitems.size(); i++)
        {
            asitems[i]->Export(out, indent);
            if (i+1<asitems.size())
                out << ", ";
        }
        out << endl;
    }
};

#endif
