// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_KERNEL_H
#define AST_KERNEL_H

#include "AST.h"
#include "ASTExpression.h"
#include "ASTControlStatement.h"
#include "ASTMachComponent.h"

class ASTMachModel;
class ASTAppModel;

// ****************************************************************************
// Class:  ASTKernel
//
// Purpose:
///   Encapsulates a kernel, the main component of control flow and execution
///   within an Aspen application model.
//
// Programmer:  Jeremy Meredith
// Creation:    September 20, 2013
//
// Modifications:
// ****************************************************************************
class ASTKernel : public ASTNode
{
  public:
    const string name;
    const vector<const ASTVarDecl*> args;
    const vector<const ASTControlStatement*> statements;
  public:
    ASTKernel(const string &n,
              ParseVector<ASTControlStatement*> statements,
              ParseVector<ASTVarDecl*> args);
    virtual ~ASTKernel();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;

    ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                    string resource) const;

    ASTExpression *GetDataSizeExpression(const ASTAppModel *app, set<string> arrays) const;

    ASTExpression *GetDynamicEnergyExpressionForResource(string resource,
                                                         string sockettype,
                                                         ASTMachModel *mach) const;

    ASTExpression *GetSerialTimeExpression(ASTAppModel *app,
                                           ASTMachModel *mach,
                                           string sockettype) const;

    ASTExpression *GetTimeExpression(ASTAppModel *app,
                                     ASTMachModel *mach,
                                     string sockettype,
                                     NameMap<string> socketoverride=NameMap<string>()) const;

    set<string> GetInclusiveDataArrays(const ASTAppModel *app) const;
    set<string> GetExclusiveDataArrays(string blockname = "") const;

    ASTExpression *GetInclusiveDataSizeExpression(const ASTAppModel *app) const;
    ASTExpression *GetExclusiveDataSizeExpression(const ASTAppModel *app, string blockname="") const;

    ASTExpression *GetSingleMapInclusiveDataSizeExpression(const ASTAppModel *app, string mapname) const;
};


#endif
