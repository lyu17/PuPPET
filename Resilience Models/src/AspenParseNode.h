// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef ASPEN_PARSE_NODE_H
#define ASPEN_PARSE_NODE_H

#include "AST.h"
#include "ASTStatement.h"
#include "ASTKernel.h"
#include "ASTExpression.h"
#include "ASTAppModel.h"
#include "ASTMachModel.h"
#include "ASTControlStatement.h"
#include "ASTExecutionBlock.h"
#include "ASTExecutionStatement.h"
#include "ParseVector.h"

// ****************************************************************************
// Struct:  AspenParseNode
//
// Purpose:
///   C++ equivalent of the union type used in Bison parsers.
//
// Programmer:  Jeremy Meredith
// Creation:    May 29, 2013
//
// Modifications:
//   Jeremy Meredith, Wed Nov  6 11:43:04 EST 2013
//   Change to ParseVector instead of vector for much faster parsing speed.
//
// ****************************************************************************
struct AspenParseNode {
    int token;
    ASTAppModel *app;
    ASTMachModel *mach;

    ASTStatement *stmt;
    ASTExecutionStatement *execstmt;
    ASTExpression *expr;
    ASTKernel *kernel;
    ASTControlStatement *controlstmt;
    ASTVarDecl *vardecl;
    ASTAsItem *asitem;

    ParseVector<ASTStatement*> stmtlist;
    ParseVector<ASTExecutionStatement*> execstmtlist;
    ParseVector<ASTExpression*> exprlist;
    ParseVector<ASTAssignStatement*> assignlist;
    ParseVector<ASTKernel*> kernellist;
    ParseVector<ASTControlStatement*> controlstmtlist;
    ParseVector<ASTVarDecl*> vardecllist;
    ParseVector<ASTAsItem*> asitemlist;
    ParseVector<ASTProbabilityItem*> probabilitylist;

    ASTMachComponent *component;
    ASTMachPower *power;
    ASTMachResource *resource;

    ParseVector<ASTMachResource*> resourcelist;
    ParseVector<ASTMachProperty*> propertylist;
    ParseVector<ASTTrait*> traitlist;
    ParseVector<pair<string,string> > conflictlist;
    ParseVector<pair<ASTExpression*,string> > socketlist;

    pair<ParseVector<ASTStatement*>, ParseVector<ASTMachComponent*> > machcontent;

    ASTIdentifier *ident;
    ASTFunctionCall *call;
    string str;
};

#define YYSTYPE AspenParseNode


#endif
