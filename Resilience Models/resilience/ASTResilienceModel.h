#ifndef AST_RESILIENCE_MODEL_H
#define AST_RESILIENCE_MODEL_H

#include "Exception.h"
#include "AST.h"
#include "ASTResilienceStatement.h"
#include "ASTExpression.h"
#include "NameMap.h"

// ****************************************************************************
// Class:  ASTResilienceModel
//
// Purpose:
///   Encapsulates an Aspen Resilience Model.
//
// Programmer:  Li Yu
// Creation:    May 01, 2014
//
// Modifications:
// ****************************************************************************
class ASTResilienceModel : public ASTNode
{
  public:
    const string name;
    const vector<ASTResilienceStatement*> resiliencestmts;

  public:
    ASTResilienceModel(const string &n, ParseVector<ASTResilienceStatement*> resiliencestmts);
    virtual ~ASTResilienceModel();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
};

#endif
