// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_EXECUTION_BLOCK
#define AST_EXECUTION_BLOCK

#include "AST.h"
#include "ASTAppModel.h"
#include "ASTExpression.h"
#include "ASTControlStatement.h"
#include "ASTExecutionStatement.h"

// ****************************************************************************
// Class:  ASTExecutionBlock
//
// Purpose:
///   Encapsulates an execute [n] { ... } block of resource requirements.
//
// Programmer:  Jeremy Meredith
// Creation:    August 20, 2013
//
// Modifications:
// ****************************************************************************
class ASTExecutionBlock : public ASTControlStatement
{
  public:
    const string name;
    const ASTExpression *parallelism;
    const vector<ASTExecutionStatement*> statements;
  public:
    ASTExecutionBlock(string name,
                      ASTExpression *parallelism,
                      ParseVector<ASTExecutionStatement*> statements);
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    ASTExpression *GetResourceRequirementExpression(const ASTAppModel *app,
                                                    string resource) const;
    set<string> GetDataArrays() const;
    ASTExpression *GetParallelExpression() const;
    ASTExpression *GetDynamicEnergyExpressionForResource(string resource,
                                                         string sockettype,
                                                         ASTMachModel *mach) const;
    ASTExpression *GetSerialTimeExpression(ASTAppModel *app,
                                           ASTMachModel *mach,
                                           string sockettype) const;
    ASTExpression *GetDepthExpression(ASTMachModel *mach,
                                      string sockettype) const;
    virtual ASTExpression *GetTimeExpression(ASTAppModel *app,
                                             ASTMachModel *mach,
                                             string sockettype) const;
    virtual set<string>    GetInclusiveDataArrays(const ASTAppModel *app) const;
};


#endif
