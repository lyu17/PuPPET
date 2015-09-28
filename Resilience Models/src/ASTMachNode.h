// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_NODE_H
#define AST_MACH_NODE_H

#include "ASTMachComponent.h"
#include "ASTExpression.h"
#include "ASTMachSocket.h"

// ****************************************************************************
// Class:  ASTMachNode
//
// Purpose:
///   A node in a machine, which contains a set of sockets of various types.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachNode : public ASTMachComponent
{
  public:
    const vector< pair<const ASTExpression*,string> > sockets;

    //
    // Acceleration structures
    //
    NameMap<const ASTMachSocket*> socketMap;
    NameMap<const ASTExpression*> socketCounts;
  public:
    ASTMachNode(string name, ParseVector< pair<ASTExpression*,string> > sockets) : ASTMachComponent(name), sockets(sockets.begin(), sockets.end())
    {
    }
    virtual ~ASTMachNode()
    {
        for (unsigned int i=0; i<sockets.size(); ++i)
            delete sockets[i].first;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Node '"<<name<<"'"<< endl;
        for (unsigned int i=0; i<sockets.size(); ++i)
        {
            out << Indent(indent+1) << "Socket type='"<<sockets[i].second<<"' quantity=" << endl;
            sockets[i].first->Print(out,indent+2);
        }
    }
    virtual void CompleteAndCheck(const NameMap<const ASTMachComponent*> &compMap)
    {
        for (unsigned int i=0; i<sockets.size(); ++i)
        {
            string socketname = sockets[i].second;
            const ASTMachSocket *socket = dynamic_cast<const ASTMachSocket*>(compMap[socketname]);
            if (!socket)
                THROW(LogicError, "Error: socket name was not the name of a type of socket");

            socketMap[socketname] = socket;
            socketCounts[socketname] = sockets[i].first;
        }
    }
    ASTExpression *GetSerialResourceDynamicPowerExpression(const string &resource,
                                                           string sockettype) const
    {
        return socketMap[sockettype]->GetSerialResourceDynamicPowerExpression(resource);
    }
    ASTExpression *GetTimeExpression(const string &resource,
                                     const vector<string> &traits, 
                                     bool serialtime,
                                     string sockettype,
                                     const ASTExpression *value) const
    {
        ///\todo: we assume one socket type.  If we want to aggregate over
        /// all socket types, we need to convert times per socket into rates (1/t),
        /// add the rates, then convert back to time (1/r).  E.g. 
        /// combinedrate = 1 / ( 1/fermitime + 1/westmeretime )
        const ASTMachSocket *socket = socketMap[sockettype];
        const ASTExpression *numsock = socketCounts[sockettype];
        ASTExpression *persock = socket->GetTimeExpression(resource, traits, serialtime, value);
        if (serialtime)
            return persock;
        else
            return new ASTBinaryExpression("/", persock, numsock->Cloned());
    }
    ASTExpression *GetCoresExpression(string sockettype) const
    {
        const ASTMachSocket *socket = socketMap[sockettype];
        const ASTExpression *numsock = socketCounts[sockettype];
        ASTExpression *corespersock = socket->GetCoresExpression();
        return new ASTBinaryExpression("*", corespersock, numsock->Cloned());
    }

    bool CheckConflict(string ra, string rb,
                       string sockettype) const
    {
        return socketMap[sockettype]->CheckConflict(ra, rb);
    }

    std::vector<std::string> GetSocketNames() const {
        return socketMap.Keys();
    }
};

#endif
