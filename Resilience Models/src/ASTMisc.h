// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MISC_H
#define AST_MISC_H

#include "AST.h"
#include "ASTExpression.h"

/// example: "as trait1, trait2(value)"
class ASTAsItem
{
  public:
    string name;
    const ASTExpression *expr;
  public:
    ASTAsItem(string name, ASTExpression *expr) : name(name), expr(expr)
    {
    }
    virtual ~ASTAsItem()
    {
        delete expr;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "As "<<name<< (expr ? " with value:" : "") << endl;
        if (expr)
            expr->Print(out, indent+1);
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << name;
        if (expr)
            out << "(" << expr->GetText(ASTExpression::ASPEN) << ")";
    }
};

/// example: "param x" and "data y" as occurs in kernel arg declarations
class ASTVarDecl : public ASTNode
{
  public:
    string type;
    string name;
  public:
    ASTVarDecl(string type, string name) : type(type), name(name) { }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Type="<<type<< " name=" << name << endl;
    }
    virtual void Export(ostream &out, int indent = 0) const
    {
        out << type << " " << name;
    }
};

/// example: "with simd [base/simdwidth]" or "with stride(n) [base*n]"
class ASTTrait : public ASTNode
{
  public:
    const string name;
    const string arg;
    const ASTExpression *quantity;
  public:
    ASTTrait(string name, ASTExpression *quantity) : name(name), arg(""), quantity(quantity) { }
    ASTTrait(string name, string arg, ASTExpression *quantity) : name(name), arg(arg), quantity(quantity) { }
    virtual ~ASTTrait()
    {
        delete quantity;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Trait '" << name << "'";
        if (arg != "")
            out << " (arg="<<arg<<")";
        out << ": quantity=" << endl;
        quantity->Print(out, indent+2);
    }
};

/// example: "dynamic power [tdp-idle]" or "static power [30]"
class ASTMachPower : public ASTNode
{
  public:
    const ASTExpression *staticPower;
    const ASTExpression *dynamicPower;
  public:
    ASTMachPower(ASTExpression *staticPower = NULL, ASTExpression *dynamicPower = NULL)
        : staticPower(staticPower), dynamicPower(dynamicPower)
    {
    }
    virtual ~ASTMachPower()
    {
        delete staticPower;
        delete dynamicPower;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Static power: " << (!staticPower ? "undefined":"") << endl;
        if (staticPower)
            staticPower->Print(out, indent+1);
        out << Indent(indent) << "Dynamic power: " << (!dynamicPower ? "undefined":"") << endl;
        if (dynamicPower)
            dynamicPower->Print(out, indent+1);
    }
};

/// example: "resource flops(n) [n/clock] with simd [base/simdwidth]"
class ASTMachResource : public ASTNode
{
  public:
    const string name;
    const string arg;
    const ASTExpression *quantity;
    const vector<const ASTTrait*> traits;
    const ASTMachPower *power;
  public:
    ASTMachResource(string name, 
                    string arg,
                    ASTExpression *quantity,
                    ParseVector<ASTTrait*> traits,
                    ASTMachPower *power)
        : name(name), arg(arg), quantity(quantity), traits(traits.begin(), traits.end()), power(power)
    {
        
    }
    virtual ~ASTMachResource()
    {
        delete quantity;
        delete power;
        for (unsigned int i=0; i<traits.size(); ++i)
            delete traits[i];
    }
    const ASTTrait *GetTrait(const string &tn) const
    {
        for (unsigned int i=0; i<traits.size(); ++i)
        {
            if (traits[i]->name == tn)
                return traits[i];
        }
        return NULL;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Resource '"<<name<<"' (arg="<<arg<<") quantity=" << endl;
        quantity->Print(out, indent+1);
        if (traits.size() == 0)
            out << Indent(indent+1) << "Traits: none" << endl;
        else
        {
            for (unsigned int i=0; i<traits.size(); ++i)
                traits[i]->Print(out, indent+1);
        }
        power->Print(out, indent+1);
    }
};

/// example: "capacity [18*giga]"
class ASTMachProperty : public ASTNode
{
  public:
    const string name;
    const ASTExpression *value;
  public:
    ASTMachProperty(string name, ASTExpression *value) : name(name), value(value)
    {
    }
    virtual ~ASTMachProperty()
    {
        delete value;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Property '"<<name<<"'="<< endl;
        value->Print(out, indent+1);
    }
};



#endif
