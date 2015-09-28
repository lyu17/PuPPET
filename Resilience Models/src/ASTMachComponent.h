// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_COMPONENT_H
#define AST_MACH_COMPONENT_H

#include "AST.h"
#include "ASTMisc.h"
#include "ASTExpression.h"
#include "ASTStatement.h" // needed for ASTAsItem

// ****************************************************************************
// Class:  ASTMachComponent
//
// Purpose:
///   A component (node, socket, core, mem, machine, etc.) of a machine model.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************

class ASTMachComponent : public ASTNode
{
  public:
    const string name;
  public:
    ASTMachComponent(string name) : name(name) { }
    virtual ~ASTMachComponent() { }
    virtual void CompleteAndCheck(const NameMap<const ASTMachComponent*>&) = 0;
};

#endif
