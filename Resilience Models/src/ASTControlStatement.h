// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_CONTROL_STATEMENT_H
#define AST_CONTROL_STATEMENT_H

#include "ASTStatement.h"

class ASTAppModel;
class ASTMachModel;

class ASTKernel;

// ****************************************************************************
// Class:  ASTControlStatement
//
// Purpose:
///   A statement which is a type of control flow, such as iterate, 
///   parallel, kernel-call, or execution-block.
//
// Programmer:  Jeremy Meredith
// Creation:    May 17, 2013
//
// Modifications:
// ****************************************************************************
class ASTControlStatement : public ASTStatement
{
  public:
    string nodeid;
    string label; ///\<hack: only used for one experiement; see issue #29
    ASTControlStatement();
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const = 0;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const = 0;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const = 0;
};

class ASTControlKernelCallStatement : public ASTControlStatement
{
  public:
    const string module;
    const string name;
    const vector<const ASTExpression*> arguments;
  public:
    ASTControlKernelCallStatement(string name, ParseVector<ASTExpression*> args);
    ASTControlKernelCallStatement(string module, string name, ParseVector<ASTExpression*> args);
    virtual ~ASTControlKernelCallStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTControlIterateStatement : public ASTControlStatement
{
  public:
    const ASTExpression *quantity;
    const ASTControlStatement *item;
  public:
    ASTControlIterateStatement(ASTExpression *q, ASTControlStatement *item);
    virtual ~ASTControlIterateStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTControlMapStatement : public ASTControlStatement
{
  public:
    const ASTExpression *quantity;
    const ASTControlStatement *item;
  public:
    ASTControlMapStatement(ASTExpression *q, ASTControlStatement *item);
    virtual ~ASTControlMapStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTControlParallelStatement : public ASTControlStatement
{
  public:
    const vector<const ASTControlStatement*> items;
  public:
  public:
    ASTControlParallelStatement(ParseVector<ASTControlStatement*> items);
    virtual ~ASTControlParallelStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTControlSequentialStatement : public ASTControlStatement
{
  public:
    const vector<const ASTControlStatement*> items;
  public:
  public:
    ASTControlSequentialStatement(ParseVector<ASTControlStatement*> items);
    virtual ~ASTControlSequentialStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTControlIfThenStatement : public ASTControlStatement
{
  public:
    const ASTExpression *comparison;
    const ASTControlStatement *thenitem;
    const ASTControlStatement *elseitem;
  public:
    ASTControlIfThenStatement(ASTExpression *comparison,
                              ASTControlStatement *thenitem, ASTControlStatement *elseitem);
    virtual ~ASTControlIfThenStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                         string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};

class ASTProbabilityItem : public ASTNode
{
  public:
    const ASTExpression *quantity;
    const ASTControlStatement *item;
  public:
    ASTProbabilityItem(ASTExpression *quantity, ASTControlStatement *item);
    virtual ~ASTProbabilityItem();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
};

class ASTControlProbabilityStatement : public ASTControlStatement
{
  public:
    const vector<const ASTProbabilityItem*> items;
    const ASTControlStatement *elseitem;
  public:
  public:
    ASTControlProbabilityStatement(ParseVector<ASTProbabilityItem*> items,
                                   ASTControlStatement *elseitem);
    virtual ~ASTControlProbabilityStatement();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    virtual ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                            string resource) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app, ASTMachModel *mach, string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};


#endif
