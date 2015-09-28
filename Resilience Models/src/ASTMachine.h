// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACHINE_H
#define AST_MACHINE_H

#include "ASTMachComponent.h"
#include "ASTExpression.h"
#include "ASTMachNode.h"

// ****************************************************************************
// Class:  ASTMachine
//
// Purpose:
///   A whole machine/system, containing nodes and an interconnect.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachine : public ASTMachComponent
{
  public:
    const ASTExpression *numnodes;
    const string nodetype;
    const string interconnecttype;
    const vector<const ASTAsItem*> interconnectprops;
    const vector<const ASTMachResource*> resources;

    //
    // Acceleration structures
    //
    const ASTMachNode *node;
    const ASTMachInterconnect *interconnect;
  public:
    ASTMachine(string name,
                   ASTExpression *numnodes,
                   string nodetype,
                   string interconnecttype,
                   ParseVector<ASTAsItem*> interconnectprops,
                   ParseVector<ASTMachResource*> resources)
        : ASTMachComponent(name),
          numnodes(numnodes),
          nodetype(nodetype),
          interconnecttype(interconnecttype),
          interconnectprops(interconnectprops.begin(), interconnectprops.end()),
          resources(resources.begin(), resources.end())
    {
        node = NULL;
        interconnect = NULL;
    }
    virtual ~ASTMachine()
    {
        delete numnodes;
        for (unsigned int i=0; i<interconnectprops.size(); ++i)
            delete interconnectprops[i];
        for (unsigned int i=0; i<resources.size(); ++i)
            delete resources[i];
    }
    virtual void CompleteAndCheck(const NameMap<const ASTMachComponent*> &compMap)
    {
        node = dynamic_cast<const ASTMachNode*>(compMap[nodetype]);
        if (!node)
            THROW(GrammaticalError, "Nodetype was not the name of a type of node");

        // we allow single-node machines with no interconnects
        if (interconnecttype != "")
        {
            interconnect = dynamic_cast<const ASTMachInterconnect*>(compMap[interconnecttype]);
            if (!interconnect)
                THROW(GrammaticalError,
                      "Interconnecttype was not the name of a type of interconnect");
        }
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Machine '"<<name<<"'"<< endl;
        out << Indent(indent+1) << "Nodes: type='"<<nodetype<<"' quantity=" << endl;
        numnodes->Print(out, indent+2);
        if (interconnecttype != "")
        {
            out << Indent(indent+1) << "Interconnect: type="<<interconnecttype << endl;
            for (unsigned int i=0; i<interconnectprops.size(); ++i)
                interconnectprops[i]->Print(out, indent+2);
        }
        else
        {
            out << Indent(indent+1) << "Interconnect: none"<< endl;
        }
        for (unsigned int i=0; i<resources.size(); ++i)
            resources[i]->Print(out, indent+1);        
    }
    ASTExpression *GetSerialResourceDynamicPowerExpression(const string &resource,
                                                           string sockettype) const
    {
        ///\todo: to match scala-ASPEN1, it looks like we assume 'serial'.
        /// or, more specifically, that we just take the TDP at the
        /// lowest level.  Is that correct?  Seems like we  multiply the
        /// memory TDP by the number of chunks, but the memory TDP is NOT
        /// per-core, so that would appear to overcount memory power by a
        /// large factor....
        return node->GetSerialResourceDynamicPowerExpression(resource,
                                                             sockettype);
    }
    ASTExpression *GetTimeExpression(const string &resource,
                                     const vector<string> &traits, 
                                     bool serialtime,
                                     string sockettype,
                                     const ASTExpression *value) const
    {
        ///\todo: we need to check interconnect for the resource, too.

        ///\todo: this "serialtime" parameter seems messy.  We set it to true when
        /// predicting kernel runtime because we know the 'depth' of a kernel for a
        /// given machine (i.e. how many times it needs to be executed), and then
        /// multiply that value by its "serial" time, or basically time-per-core.
        /// It's a little confusing, though, because if a kernel reads N values
        /// per instance, since memory is at a socket level, we need to group up
        /// all the individual ones (i.e. multiply per-instance time by the number of
        /// cores) so we can divide it by the per-socket bandwidth. (Or looking at
        /// it a clearer way, only 1/ncorespersocket of the per-socket bandwidth is
        /// available to each instance.)

        ASTExpression *pernode = node->GetTimeExpression(resource, traits, serialtime,
                                                         sockettype, value);
        if (serialtime)
            return pernode;
        else
            return new ASTBinaryExpression("/", pernode, numnodes->Cloned());
    }
    ASTExpression *GetTotalCoresExpression(string sockettype) const
    {
        return new ASTBinaryExpression("*", numnodes->Cloned(),
                                       node->GetCoresExpression(sockettype));
    }
    bool CheckConflict(string ra, string rb,
                       string sockettype) const
    {
        ///\todo: do we need to check interconnect for a conflict too?
        return node->CheckConflict(ra, rb, sockettype);
    }

    std::vector<std::string> GetSocketNames() const {
        return node->GetSocketNames();
    }
};

#endif
