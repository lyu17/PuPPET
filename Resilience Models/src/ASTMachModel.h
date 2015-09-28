// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_MODEL_H
#define AST_MACH_MODEL_H

#include "AST.h"
#include "ASTMisc.h"
#include "ASTMachComponent.h"
#include "ASTExpression.h"
#include "NameMap.h"
#include "ASTMachine.h"

class ASTMachine;

// ****************************************************************************
// Class:  ASTMachModel
//
// Purpose:
///   Encapsulates an Aspen Machine Model.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachModel : public ASTNode
{
  public:
    const vector<ASTStatement*> globals;
    const vector<ASTMachComponent*> components;

    //
    // Acceleration structures
    //
    const ASTMachine *machine;
    NameMap<const ASTExpression*> paramMap;
    NameMap<const ASTMachComponent*> compMap;
  public:
    ASTMachModel(ParseVector<ASTStatement*> globals,
                 ParseVector<ASTMachComponent*> components);
    virtual ~ASTMachModel();
    virtual void CompleteAndCheck();
    virtual void Print(ostream &out, int indent = 0) const;
    const ASTMachine *GetMachine() const;
    ASTExpression *GetTimeExpression(const string &resource,
                                     const vector<string> &traits,
                                     string sockettype,
                                     ASTExpression *value) const;
    double CountTime(const string &resource,
                     const vector<string> &traits,
                     string sockettype,
                     ASTExpression *value) const;
    string GetTimeExprText(const string &resource,
                           const vector<string> &traits,
                           string sockettype,
                           bool expand,
                           ASTExpression *value,
                           const NameMap<const ASTExpression*> &subst=NameMap<const ASTExpression*>()) const;
};

#endif
