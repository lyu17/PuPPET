// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_EXPRESSION_H
#define AST_EXPRESSION_H

#include <sstream>
#include <cmath>

#include "Exception.h"
#include "AST.h"
#include "NameMap.h"
#include "ParseVector.h"

// ****************************************************************************
// Class:  ASTExpression
//
// Purpose:
///   Base class for all mathematical expressions.
//
// Programmer:  Jeremy Meredith
// Creation:    May 17, 2013
//
// Modifications:
// ****************************************************************************
class ASTExpression : public ASTNode
{
  public:
    enum TextStyle { ASPEN, C, GNUPLOT };
  public:
    virtual void Print(ostream &out, int indent = 0) const = 0;
    virtual double Evaluate() const = 0;
    virtual string GetText(TextStyle style = GNUPLOT) const = 0;
    virtual bool           Equivalent(const ASTExpression *e) const { return false; }
    virtual ASTExpression *Cloned() const = 0;
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const { return Cloned(); }
    virtual ASTExpression *OneStepSimplified() const { return Cloned(); }
    virtual ASTExpression *Simplified() const
    {
        int nsteps = 100;
        ASTExpression *expr = Cloned();
        for (int i=0; i<nsteps; ++i)
        {
            //cerr << "\n\n----  STEP ----\n";
            ASTExpression *newexpr = expr->OneStepSimplified();
            delete expr;
            expr = newexpr;
            //if (dynamic_cast<ASTValue*>(expr))
            //    break;
            //expr->Print(cerr);
        }
        return expr;
    }

    // helper function; do the expansion, evaluate, and free the expansion
    virtual double Evaluate(const NameMap<const ASTExpression*> &paramMap) const
    {
        ASTExpression *expanded = this->Expanded(paramMap);
        double value = expanded->Evaluate();
        delete expanded;
        return value;
    }
};

// Each operation on an expression creates a new one
// and leaves the old one unmodified.  Using auto_ptr
// greatly simplifies the memory management to
// prevent leaks.
typedef auto_ptr<ASTExpression> ExprPtr;


class ASTValue : public ASTExpression
{
  public:
    virtual double GetValue() const = 0;
};

class ASTInteger : public ASTValue
{
  public:
    long long value;
    ASTInteger(long long value) : value(value) { }
    virtual ASTExpression *Cloned() const
    {
        return new ASTInteger(value);
    }
    virtual void Print(ostream &out, int indent = 0) const { out << Indent(indent) << "Int: "<<value<<endl;}
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTValue *v = dynamic_cast<const ASTValue*>(e);
        if (v && v->GetValue() == value)
            return true;
        return false;
    }
    virtual double Evaluate() const
    {
        return value;
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        ostringstream out;
        out << value;
        return out.str();
    }
    virtual double GetValue() const
    {
        return value;
    }
};

class ASTReal : public ASTValue
{
  public:
    double value;
    ASTReal(double value) : value(value) { }
    virtual ASTExpression *Cloned() const
    {
        return new ASTReal(value);
    }
    virtual void Print(ostream &out, int indent = 0) const { out << Indent(indent) << "Real: "<<value<<endl;}
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTValue *v = dynamic_cast<const ASTValue*>(e);
        if (v && v->GetValue() == value)
            return true;
        return false;
    }
    virtual double Evaluate() const
    {
        return value;
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        ostringstream out;
        out << value;
        return out.str();
    }
    virtual double GetValue() const
    {
        return value;
    }
};

class ASTIdentifier : public ASTExpression
{
  public:
    string name;
    ASTIdentifier(const string& name) : name(name) { }
    virtual void Print(ostream &out, int indent = 0) const { out << Indent(indent) << "Ident: "<<name<<endl;}
    virtual ASTExpression *Cloned() const
    {
        return new ASTIdentifier(name);
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTIdentifier *i = dynamic_cast<const ASTIdentifier*>(e);
        if (i && i->name == name)
            return true;
        return false;
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        if (paramMap.Contains(name))
        {
            if (expandedset.count(name) != 0)
                THROW(GrammaticalError, "Recursive expansion of ", name, " detected");
            expandedset.insert(name);
            return paramMap[name]->Expanded(paramMap, expandedset);
        }
        else
            return Cloned();
    }
    virtual double Evaluate() const
    {
        THROW(GrammaticalError, "Can't evaluate; identifier '", name, "' wasn't expanded");
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        return name;
    }
};

class ASTSequenceExpression : public ASTExpression
{
  public:
    string op;
    vector<ASTExpression*> entries;
  public:
    ASTSequenceExpression(string op,
                          ASTExpression *a = NULL,
                          ASTExpression *b = NULL,
                          ASTExpression *c = NULL,
                          ASTExpression *d = NULL)
        : op(op)
    {
        if (a)
            entries.push_back(a);
        if (b)
            entries.push_back(b);
        if (c)
            entries.push_back(c);
        if (d)
            entries.push_back(d);
    }
    virtual ~ASTSequenceExpression()
    {
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            delete entries[i];
        }
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTSequenceExpression *s = dynamic_cast<const ASTSequenceExpression*>(e);
        if (!s)
            return false;
        if (s->entries.size() != entries.size())
            return false;
        if (s->op != op)
            return false;
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            if (! entries[i]->Equivalent(s->entries[i]))
                return false;
        }
        return true;
    }
    virtual ASTExpression *OneStepSimplified() const;
    virtual ASTExpression *Cloned() const
    {
        ASTSequenceExpression *expr = new ASTSequenceExpression(op);
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            expr->entries.push_back(entries[i]->Cloned());
        }
        return expr;
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        ASTSequenceExpression *expr = new ASTSequenceExpression(op);
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            expr->entries.push_back(entries[i]->Expanded(paramMap, expandedset));
        }
        return expr;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Sequence "<<op<<endl;
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            out << Indent(indent+1) << "Entry "<<i<<endl;
            entries[i]->Print(out,indent+2);
        }
    }
    virtual double Evaluate() const
    {
        if (entries.size() == 0)
            THROW(GrammaticalError, "No entries in sequence");

        double val = entries[0]->Evaluate();
        if (op == "+") 
        {
            for (unsigned int i=1; i<entries.size(); ++i)
                val += entries[i]->Evaluate();
        }
        else if (op == "*") 
        {
            for (unsigned int i=1; i<entries.size(); ++i)
                val *= entries[i]->Evaluate();
        }
        else
            THROW(GrammaticalError, "Unknown operator : ", op);

        return val;
    }    
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        if (entries.size() == 0)
            THROW(GrammaticalError, "No entries in sequence");

        string s = "(";
        for (unsigned int i=0; i<entries.size(); ++i)
        {
            if (i != 0)
                s += " " + op + " ";
            bool needparens = true;
            ASTExpression *e = entries[i];
            if (dynamic_cast<ASTValue*>(e) ||
                dynamic_cast<ASTIdentifier*>(e))
                needparens = false;

            if (needparens)
                s += "(" + e->GetText() + ")";
            else
                s += e->GetText();
        }
        s += ")";
        return s;
    }
};

class ASTBinaryExpression : public ASTExpression
{
  private:
    int GetPrecedence() const
    {
        if (op == "+") return 1;
        if (op == "-") return 1;
        if (op == "*") return 2;
        if (op == "/") return 2;
        if (op == "^") return 4;
        THROW(GrammaticalError, "Unknown operator : ", op);
    }
  public:
    string op;
    ASTExpression *lhs;
    ASTExpression *rhs;
    ASTBinaryExpression(string op, ASTExpression *lhs, ASTExpression *rhs)
        : op(op), lhs(lhs), rhs(rhs)
    {
    }
    virtual ~ASTBinaryExpression()
    {
        delete lhs;
        delete rhs;
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTBinaryExpression *b = dynamic_cast<const ASTBinaryExpression*>(e);
        if (!b)
            return false;
        if (b->op != op)
            return false;
        if (! b->lhs->Equivalent(lhs))
            return false;
        if (! b->rhs->Equivalent(rhs))
            return false;
        return true;
    }
    virtual ASTExpression *Cloned() const
    {
        return new ASTBinaryExpression(op, lhs->Cloned(), rhs->Cloned());
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        return new ASTBinaryExpression(op, lhs->Expanded(paramMap, expandedset), rhs->Expanded(paramMap, expandedset));
    }
    virtual ASTExpression *OneStepSimplified() const
    {
        ASTValue *lv = dynamic_cast<ASTValue*>(lhs);
        ASTValue *rv = dynamic_cast<ASTValue*>(rhs);
        ASTBinaryExpression *lb = dynamic_cast<ASTBinaryExpression*>(lhs);
        ASTBinaryExpression *rb = dynamic_cast<ASTBinaryExpression*>(rhs);
        ASTValue *lblv = lb ? dynamic_cast<ASTValue*>(lb->lhs) : 0;
        ASTValue *lbrv = lb ? dynamic_cast<ASTValue*>(lb->rhs) : 0;
        ASTValue *rblv = rb ? dynamic_cast<ASTValue*>(rb->lhs) : 0;
        ASTValue *rbrv = rb ? dynamic_cast<ASTValue*>(rb->rhs) : 0;
        ASTSequenceExpression *ls = dynamic_cast<ASTSequenceExpression*>(lhs);
        ASTSequenceExpression *rs = dynamic_cast<ASTSequenceExpression*>(rhs);

        // add/subtract identities
        if (op == "+" && lv && lv->GetValue()==0)
            return rhs->OneStepSimplified();
        if ((op == "+" || op == "-") && rv && rv->GetValue()==0)
            return lhs->OneStepSimplified();

        // mul/div identities
        if (op == "*" && lv && lv->GetValue()==1)
            return rhs->OneStepSimplified();
        if ((op == "*" || op == "/") && rv && rv->GetValue()==1)
            return lhs->OneStepSimplified();

        // 0*x or x*0 is 0
        if (op == "*" && ((lv && lv->GetValue()==0) || (rv && rv->GetValue()==0)))
            return new ASTReal(0);

        // 0/x is 0
        if (op == "/" && (lv && lv->GetValue()==0))
            return new ASTReal(0);

        // turn division into multiplication
        if (op == "/" && rv)
        {
            return new ASTBinaryExpression("*",
                                           lhs->OneStepSimplified(),
                                           new ASTReal(1. / rv->GetValue()));
        }

        // distribute * over + if one of the * and one of the + terms is a value
        //  1: lhs is binary add, rhs is value
        if (op == "*" && rv && (lb && lb->op == "+"))
        {
            if (lblv)
                return new ASTBinaryExpression("+",
                                               new ASTReal(lblv->GetValue() *
                                                           rv->GetValue()),
                                               new ASTBinaryExpression("*",
                                                                       lb->rhs->OneStepSimplified(),
                                                                       rv->Cloned()));
            if (lbrv)
                return new ASTBinaryExpression("+",
                                               new ASTReal(lbrv->GetValue() *
                                                           rv->GetValue()),
                                               new ASTBinaryExpression("*",
                                                                       lb->lhs->OneStepSimplified(),
                                                                       rv->Cloned()));
        }
        //  2: rhs is binary add, lhs is value
        if (op == "*" && lv && (rb && rb->op == "+"))
        {
            if (rblv)
                return new ASTBinaryExpression("+",
                                               new ASTReal(rblv->GetValue() *
                                                           lv->GetValue()),
                                               new ASTBinaryExpression("*",
                                                                       rb->rhs->OneStepSimplified(),
                                                                       lv->Cloned()));
            if (rbrv)
                return new ASTBinaryExpression("+",
                                               new ASTReal(rbrv->GetValue() *
                                                           lv->GetValue()),
                                               new ASTBinaryExpression("*",
                                                                       rb->lhs->OneStepSimplified(),
                                                                       lv->Cloned()));
        }

        // if we have two values, just evaluate them (don't need a paramMap)
        if (lv && rv)
        {
            return new ASTReal(Evaluate());
        }

        // combine chained factors/terms into explicit sequences
        // and convert binary ops with matching op child sequences
        // into single sequences.
        // NOTE!!!  This is the ONLY place sequence expressions are
        // used (as of 5/21/2014).  If we want to get rid of them entirely,
        // simply remove these next few simplifications.
        if ((op == "+" || op == "*") && rb && rb->op == op && lb && lb->op == op)
        {
            return new ASTSequenceExpression(op,
                                             lb->lhs->OneStepSimplified(),
                                             lb->rhs->OneStepSimplified(),
                                             rb->lhs->OneStepSimplified(),
                                             rb->rhs->OneStepSimplified());
        }
        if ((op == "+" || op == "*") && lb && lb->op == op)
        {
            return new ASTSequenceExpression(op,
                                             lb->lhs->OneStepSimplified(),
                                             lb->rhs->OneStepSimplified(),
                                             rhs->OneStepSimplified());
        }
        if ((op == "+" || op == "*") && rb && rb->op == op)
        {
            return new ASTSequenceExpression(op,
                                             lhs->OneStepSimplified(),
                                             rb->lhs->OneStepSimplified(),
                                             rb->rhs->OneStepSimplified());
        }

        if (ls && ls->op == op)
        {
            // currently a safe assumption since we're using cloned instead of simplified
            ASTSequenceExpression *seq = dynamic_cast<ASTSequenceExpression*>(ls->Cloned());
            if (!seq)
                THROW(GrammaticalError, "Cloned sequence wasn't a sequence");
            seq->entries.push_back(rhs->OneStepSimplified());
            return seq;
        }

        if (rs && rs->op == op)
        {
            // currently a safe assumption since we're using cloned instead of simplified
            ASTSequenceExpression *seq = dynamic_cast<ASTSequenceExpression*>(rs->Cloned());
            if (!seq)
                THROW(GrammaticalError, "Cloned sequence wasn't a sequence");
            seq->entries.push_back(lhs->OneStepSimplified());
            return seq;
        }


        // no easy simplification
        return new ASTBinaryExpression(op, lhs->OneStepSimplified(), rhs->OneStepSimplified());
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Binary Op "<<op<<endl;
        out << Indent(indent+1) << "LHS"<<endl;
        lhs->Print(out,indent+2);
        out << Indent(indent+1) << "RHS"<<endl;
        rhs->Print(out,indent+2);
    }
    virtual double Evaluate() const
    {
        double l = lhs->Evaluate();
        double r = rhs->Evaluate();
        if (op == "+") return l + r;
        if (op == "-") return l - r;
        if (op == "*") return l * r;
        if (op == "/") return l / r;
        if (op == "^") return pow(l,r);
        THROW(GrammaticalError, "Unknown operator: ", op);
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        ///\todo: is this too lenient on parentheses?
        // do some work to try to minimize unnecessary parentheses
        ASTValue *lv = dynamic_cast<ASTValue*>(lhs);
        ASTValue *rv = dynamic_cast<ASTValue*>(rhs);
        ASTIdentifier *li = dynamic_cast<ASTIdentifier*>(lhs);
        ASTIdentifier *ri = dynamic_cast<ASTIdentifier*>(rhs);
        ASTBinaryExpression *lb = dynamic_cast<ASTBinaryExpression*>(lhs);
        ASTBinaryExpression *rb = dynamic_cast<ASTBinaryExpression*>(rhs);

        bool commutative = (op=="+" || op=="*");
        bool lneedparens = false;
        bool rneedparens = false;
        if (lb && lb->GetPrecedence()<=GetPrecedence())
            lneedparens = true;
        if (rb && rb->GetPrecedence()<=GetPrecedence())
            rneedparens = true;
        if ((!lv && !li) && !commutative)
            lneedparens = true;
        if ((!rv && !ri) && !commutative)
            rneedparens = true;

        string lt = lhs->GetText(style);
        string rt = rhs->GetText(style);

        if (rneedparens)
            rt = "("+rt+")";
        if (lneedparens)
            lt = "("+lt+")";

        // adjust operator symbols for other systems (e.g. ** vs ^)
        // adjust spacing for visual clarity, too
        if (op == "^")
        {
            switch (style)
            {
              case ASPEN:   return lt + "^" + rt;
              case C:       return string("pow(") + lt + "," + rt + ")";
              case GNUPLOT: return lt + "**" + rt;
              default:
                  std::stringstream ss;
                  ss << "Unknown enum TextStyle : " << style;
                  THROW(LogicError, ss.str());
            }
        }
        else if (op == "*")
        {
            return lt + op + rt;
        }
        else
        {
            return lt + " " + op + " " + rt;
        }
    }
};

class ASTUnaryExpression : public ASTExpression
{
  public:
    string op;
    ASTExpression *expr;
    ASTUnaryExpression(string op, ASTExpression *expr)
        : op(op), expr(expr)
    {
    }
    virtual ~ASTUnaryExpression()
    {
        delete expr;
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTUnaryExpression *u = dynamic_cast<const ASTUnaryExpression*>(e);
        if (!u)
            return false;
        if (u->op != op)
            return false;
        if (! u->expr->Equivalent(expr))
            return false;
        return true;
    }
    virtual ASTExpression *Cloned() const
    {
        return new ASTUnaryExpression(op, expr->Cloned());
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        return new ASTUnaryExpression(op, expr->Expanded(paramMap, expandedset));
    }
    virtual ASTExpression *OneStepSimplified() const
    {
        if (dynamic_cast<ASTValue*>(expr))
            return new ASTReal(Evaluate());

        return new ASTUnaryExpression(op, expr->OneStepSimplified());
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Unary Op "<<op<<endl;
        expr->Print(out,indent+1);
    }
    virtual double Evaluate() const
    {
        if (op == "-") return -expr->Evaluate();
        THROW(LogicError, "A unary expression must be '-'.");
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        return "(" + op + " " + expr->GetText(style) + ")";
    }
};

class ASTFunctionCall : public ASTExpression
{
  public:
    string name;
    vector<ASTExpression*> arguments;
    unsigned int NumArgs() const
    {
        if (name == "log"   ||
            name == "log2"  ||
            name == "floor" ||
            name == "ceil"  ||
            name == "sqrt")
            return 1;
        if (name == "max"   ||
            name == "min")
            return 2;
        THROW(GrammaticalError, "Unknown function call : ", name);
    }
  public:
    ASTFunctionCall(string name, ParseVector<ASTExpression*> args) : name(name), arguments(args.begin(),args.end()) { }
    ASTFunctionCall(string name, vector<ASTExpression*> args) : name(name), arguments(args.begin(),args.end()) { }
    ASTFunctionCall(string name, ASTExpression *arg0, ASTExpression *arg1=NULL)
        : name(name)
    {
        arguments.push_back(arg0);
        if (arg1)
            arguments.push_back(arg1);
    }
    virtual ~ASTFunctionCall()
    {
        for (unsigned int i=0; i<arguments.size(); ++i)
            delete arguments[i];
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTFunctionCall *s = dynamic_cast<const ASTFunctionCall*>(e);
        if (!s)
            return false;
        if (s->arguments.size() != arguments.size())
            return false;
        if (s->name != name)
            return false;
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            if (! arguments[i]->Equivalent(s->arguments[i]))
                return false;
        }
        return true;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Call "<<name<< " with "<<arguments.size()<<" args:" << endl;
        for (unsigned int i=0; i<arguments.size(); i++)
        {
            arguments[i]->Print(out, indent+1);
        }
    }
    virtual ASTExpression *Cloned() const
    {
        vector<ASTExpression*> clonedargs;
        for (unsigned int i=0; i<arguments.size(); ++i)
            clonedargs.push_back(arguments[i]->Cloned());
        return new ASTFunctionCall(name, clonedargs);
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        vector<ASTExpression*> expandedargs;
        for (unsigned int i=0; i<arguments.size(); ++i)
            expandedargs.push_back(arguments[i]->Expanded(paramMap, expandedset));
        return new ASTFunctionCall(name, expandedargs);
    }
    virtual ASTExpression *OneStepSimplified() const
    {
        // if we only have value contents, we can just return the evaluation
        // as our simplification
        bool allvalues = true;
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            if (dynamic_cast<ASTValue*>(arguments[i]) == NULL)
                allvalues = false;
        }
        if (allvalues)
        {
            return new ASTReal(Evaluate());
        }

        // otherwise, simplify our contents
        vector<ASTExpression*> simplifiedargs;
        for (unsigned int i=0; i<arguments.size(); ++i)
            simplifiedargs.push_back(arguments[i]->OneStepSimplified());
        return new ASTFunctionCall(name, simplifiedargs);
    }
    virtual double Evaluate() const
    {
        if (arguments.size() != NumArgs())
        {
            std::stringstream msg;
            msg << "Unexpected number of arguments to function call: "
                << arguments.size() << " for " << NumArgs() << " in function " << name;
            THROW(GrammaticalError, msg.str());
        }
        
        vector<double> v;
        for (unsigned int i=0; i<arguments.size(); ++i)
            v.push_back(arguments[i]->Evaluate());

        if (name == "log")    return log(v[0]);
        if (name == "log2")   return log(v[0]) / log(2);
        if (name == "max")    return v[0] > v[1] ? v[0] : v[1];
        if (name == "min")    return v[0] < v[1] ? v[0] : v[1];
        if (name == "floor")  return floor(v[0]);
        if (name == "ceil")   return ceil(v[0]);
        if (name == "sqrt")   return sqrt(v[0]);

        THROW(GrammaticalError, "Unknown function call: ", name);
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        string ret = name;
        if (name == "log2")
            ret = "(log";

        ret += "(";
        for (unsigned int i=0; i<arguments.size(); ++i)
        {
            ret += arguments[i]->GetText(style);
            if (i<arguments.size()-1)
                ret += ", ";
        }
        ret += ")";

        if (name == "log2")
            ret += "/log(2))";

        return ret;
    }
};

class ASTComparisonExpression : public ASTExpression
{
  public:
    string op;
    ASTExpression *lhs;
    ASTExpression *rhs;
    ASTComparisonExpression(string op, ASTExpression *lhs, ASTExpression *rhs)
        : op(op), lhs(lhs), rhs(rhs)
    {
    }
    virtual ~ASTComparisonExpression()
    {
        delete lhs;
        delete rhs;
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTComparisonExpression *c = dynamic_cast<const ASTComparisonExpression*>(e);
        if (!c)
            return false;
        if (c->op != op)
            return false;
        if (! c->lhs->Equivalent(lhs))
            return false;
        if (! c->rhs->Equivalent(rhs))
            return false;
        return true;
    }
    virtual ASTExpression *Cloned() const
    {
        return new ASTComparisonExpression(op, lhs->Cloned(), rhs->Cloned());
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        return new ASTComparisonExpression(op, lhs->Expanded(paramMap, expandedset), rhs->Expanded(paramMap, expandedset));
    }
    virtual ASTExpression *OneStepSimplified() const
    {
        ASTValue *lv = dynamic_cast<ASTValue*>(lhs);
        ASTValue *rv = dynamic_cast<ASTValue*>(rhs);

        // if we have two values, just evaluate them (don't need a paramMap)
        if (lv && rv)
        {
            return new ASTReal(Evaluate());
        }

        // no easy simplification
        return new ASTComparisonExpression(op, lhs->OneStepSimplified(), rhs->OneStepSimplified());
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Comparison "<<op<<endl;
        out << Indent(indent+1) << "LHS"<<endl;
        lhs->Print(out,indent+2);
        out << Indent(indent+1) << "RHS"<<endl;
        rhs->Print(out,indent+2);
    }
    virtual double Evaluate() const
    {
        double l = lhs->Evaluate();
        double r = rhs->Evaluate();

        if (op == "and") return l && r;
        if (op == "or")  return l || r;

        if (op == "==") return l == r;
        if (op == "!=") return l != r;
        if (op == "<")  return l <  r;
        if (op == ">")  return l >  r;
        if (op == "<=") return l <= r;
        if (op == ">=") return l >= r;
        THROW(GrammaticalError, "Unknown operator : ", op);
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        ASTValue *lv = dynamic_cast<ASTValue*>(lhs);
        ASTValue *rv = dynamic_cast<ASTValue*>(rhs);
        ASTIdentifier *li = dynamic_cast<ASTIdentifier*>(lhs);
        ASTIdentifier *ri = dynamic_cast<ASTIdentifier*>(rhs);

        bool lneedparens = false;
        bool rneedparens = false;
        if (!lv && !li)
            lneedparens = true;
        if (!rv && !ri)
            rneedparens = true;

        string lt = lhs->GetText(style);
        string rt = rhs->GetText(style);

        if (rneedparens)
            rt = "("+rt+")";
        if (lneedparens)
            lt = "("+lt+")";

        return lt + " " + op + " " + rt;
    }
};

class ASTIfThenExpression : public ASTExpression
{
  public:
    ASTExpression *iftest;
    ASTExpression *thenexpr;
    ASTExpression *elseexpr;
    ASTIfThenExpression(ASTExpression *iftest, ASTExpression *thenexpr, ASTExpression *elseexpr)
        : iftest(iftest), thenexpr(thenexpr), elseexpr(elseexpr)
    {
    }
    virtual ~ASTIfThenExpression()
    {
        delete iftest;
        delete thenexpr;
        delete elseexpr;
    }
    virtual bool Equivalent(const ASTExpression *e) const
    {
        const ASTIfThenExpression *i = dynamic_cast<const ASTIfThenExpression*>(e);
        if (!i)
            return false;
        if (! i->iftest->Equivalent(iftest))
            return false;
        if (! i->thenexpr->Equivalent(thenexpr))
            return false;
        // neither has an else; equivalent so far, return true
        if (!i->elseexpr && !elseexpr)
            return true;
        // only one does not have an else; can't be equivalent, return false
        if (!i->elseexpr || !elseexpr)
            return false;
        // both have else; test for equivalency
        if (! i->elseexpr->Equivalent(elseexpr))
            return false;
        return true;
    }
    virtual ASTExpression *Cloned() const
    {
        return new ASTIfThenExpression(iftest->Cloned(), thenexpr->Cloned(), elseexpr->Cloned());
    }
    virtual ASTExpression *Expanded(const NameMap<const ASTExpression*> &paramMap, set<string> expandedset = set<string>()) const
    {
        return new ASTIfThenExpression(iftest->Expanded(paramMap, expandedset), thenexpr->Expanded(paramMap, expandedset), elseexpr->Expanded(paramMap, expandedset));
    }
    virtual ASTExpression *OneStepSimplified() const
    {
        ASTValue *ifval = dynamic_cast<ASTValue*>(iftest);

        // if our conditional is just a value, we know which case to return
        if (ifval)
        {
            if (ifval->Evaluate() != 0)
                return thenexpr->OneStepSimplified();
            else
                return elseexpr->OneStepSimplified();
        }

        // no easy simplification
        return new ASTIfThenExpression(iftest->OneStepSimplified(),
                                       thenexpr->OneStepSimplified(),
                                       elseexpr->OneStepSimplified());
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "If test:"<<endl;
        out << Indent(indent+1) << "Condition:"<<endl;
        iftest->Print(out,indent+2);
        out << Indent(indent+1) << "Then:"<<endl;
        thenexpr->Print(out,indent+2);
        out << Indent(indent+1) << "Else:"<<endl;
        elseexpr->Print(out,indent+2);
    }
    virtual double Evaluate() const
    {
        double condition = iftest->Evaluate();
        if (condition)
            return thenexpr->Evaluate();
        else
            return elseexpr->Evaluate();
    }
    virtual string GetText(TextStyle style = GNUPLOT) const
    {
        bool ternary = style==GNUPLOT || style==C;
        if (ternary)
            return string("(") +
                iftest->GetText(style) + " ? " +
                thenexpr->GetText(style) + " : " +
                elseexpr->GetText(style) + ")";
        else
            return string("if(") + 
                iftest->GetText(style) + ", " +
                thenexpr->GetText(style) + ", " +
                elseexpr->GetText(style) + ")";
    }
};

#endif
