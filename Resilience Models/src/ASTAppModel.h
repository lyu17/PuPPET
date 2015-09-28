// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_APP_MODEL_H
#define AST_APP_MODEL_H

#include "Exception.h"
#include "AST.h"
#include "ASTStatement.h"
#include "ASTKernel.h"
#include "ASTExpression.h"
#include "NameMap.h"

// ****************************************************************************
// Class:  ASTAppModel
//
// Purpose:
///   Encapsulates an Aspen Application Model.
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTAppModel : public ASTNode
{
  public:
    const string name;
    const vector<ASTStatement*> globals;
    const vector<ASTKernel*> kernels;

    //
    // Acceleration structures
    //
    const ASTKernel *mainKernel;
    NameMap<const ASTExpression*> paramMap;
    NameMap<const ASTKernel*>     kernelMap;
    NameMap<const ASTAppModel*>   importedModels;
  public:
    ASTAppModel(const string &n, ParseVector<ASTStatement*> globals, ParseVector<ASTKernel*> kernels);
    virtual ~ASTAppModel();
    virtual void CompleteAndCheck();
    virtual void Print(ostream &out, int indent = 0) const;
    virtual void Export(ostream &out, int indent = 0) const;
    int FindParametersWithRanges(vector<string> &names,
                                 vector<double> &defvals,
                                 vector<double> &minvals,
                                 vector<double> &maxvals) const;
    ASTExpression *GetSingleArraySize(string array) const;
    ASTExpression *GetGlobalArraySizeExpression() const;
    ASTExpression *GetResourceRequirementExpression(string resource) const;
    ASTExpression *GetControlFlowExpression() const;
    ///\todo: this should probably allow a whole set of substitutions
    double Count(string resource, string withparam="", double withparamvalue=0) const;
    string GetResourceRequirementExpressionText(string resource, bool expand, bool simplify,
                                                const NameMap<const ASTExpression*> &subst=NameMap<const ASTExpression*>()) const;
};



#endif
