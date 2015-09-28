// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <map>
#include <set>
#include <sstream>
#include <utility>
#include <memory>

using std::string;
using std::vector;
using std::ostream;
using std::ostringstream;
using std::cerr;
using std::endl;
using std::map;
using std::set;
using std::pair;
using std::auto_ptr;

inline string Indent(int n) { return string(n*3, ' '); }

// ****************************************************************************
// Class:  ASTNode
//
// Purpose:
///   Base class for all nodes in the Aspen abstract syntax tree.
//
// Programmer:  Jeremy Meredith
// Creation:    May 15, 2013
//
// Modifications:
// ****************************************************************************
class ASTNode
{
  public:
    virtual ~ASTNode() {}
    virtual void Print(ostream &out, int indent = 0) const = 0;
    virtual void Export(ostream &out, int indent = 0) const { }
};

#endif
