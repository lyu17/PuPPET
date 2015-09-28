// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_SOCKET_H
#define AST_MACH_SOCKET_H

#include "ASTMachComponent.h"
#include "ASTMachElement.h"

// ****************************************************************************
// Class:  ASTMachSocket
//
// Purpose:
///   A socket (e.g. GPU or CPU) in a machine.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachSocket : public ASTMachComponent
{
  public:
    const ASTExpression *corequant;
    const string coretype;
    const string memtype;
    const bool cacheprivate;
    const string cachetype;
    const string linktype;
    const ASTMachPower *power;

    //
    // Acceleration structures
    //
    const ASTMachCore  *core;
    const ASTMachMem   *mem;
    const ASTMachCache *cache;
    const ASTMachLink  *link;
  public:
    ASTMachSocket(string name, 
                  ASTExpression *coreq, string coret,
                  string memt,
                  string cachep, string cachet,
                  string linkt,
                  ASTMachPower *pwr)
        : ASTMachComponent(name),
          corequant(coreq),
          coretype(coret),
          memtype(memt),
          cacheprivate(cachep=="private"),
          cachetype(cachet),
          linktype(linkt),
          power(pwr)
    {
        core  = NULL;
        mem   = NULL;
        cache = NULL;
        link  = NULL;
    }
    virtual ~ASTMachSocket()
    {
        delete corequant;
        delete power;
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Socket '"<<name<<"'" << endl;
        out << Indent(indent+1) << "Cores: type="<<coretype<<"' quantity="<<endl;
        corequant->Print(out, indent+2);
        out << Indent(indent+1) << "Memory: type="<<memtype<<endl;
        out << Indent(indent+1) << "Cache: type="<<cachetype<<" private="<<(cacheprivate?"true":"false")<<endl;
        out << Indent(indent+1) << "Link: type="<<linktype<<endl;
        power->Print(out, indent+1);
    }
    virtual void CompleteAndCheck(const NameMap<const ASTMachComponent*> &compMap)
    {
        core = dynamic_cast<const ASTMachCore*>(compMap[coretype]);
        if (!core)
            THROW(LogicError, "coretype was not the name of a type of core");

        mem = dynamic_cast<const ASTMachMem*>(compMap[memtype]);
        if (!mem)
            THROW(LogicError, "memtype was not the name of a type of mem");

        cache = dynamic_cast<const ASTMachCache*>(compMap[cachetype]);
        if (!cache)
            THROW(LogicError, "cachetype was not the name of a type of cache");

        link = dynamic_cast<const ASTMachLink*>(compMap[linktype]);
        if (!link)
            THROW(LogicError, "linktype was not the name of a type of link");
    }
    ASTExpression *GetSerialResourceDynamicPowerExpression(const string &resource) const
    {
        ASTExpression *percore = core->GetSerialResourceDynamicPowerExpression(resource);
        if (percore)
        {
            return percore;
        }

        ASTExpression *memtime = mem->GetSerialResourceDynamicPowerExpression(resource);
        if (memtime)
        {
            ///\todo: this first line is wrong, but it's what matches aspen....
            return memtime;
            ///\todo: instead, use this line:
            //return new ASTBinaryExpression("/", memtime, corequant->Cloned());
        }

        THROW(GrammaticalError,
              "We expected either memory or core to use a given resource within a socket.");
    }
    ASTExpression *GetTimeExpression(const string &resource,
                                     const vector<string> &traits, 
                                     bool serialtime,
                                     const ASTExpression *value) const
    {
        ///\todo: are there any resources that require time from more than
        /// one type of element?  If so, I guess we should max() these two???

        ASTExpression *percore = core->GetTimeExpression(resource, traits, value);
        if (percore)
        {
            // this must be a core-type resource.
            if (serialtime)
                return percore;
            else
                return new ASTBinaryExpression("/", percore, corequant->Cloned());
        }

        ASTExpression *memtime = mem->GetTimeExpression(resource, traits, value);
        if (memtime)
        {
            if (serialtime)
                return new ASTBinaryExpression("*", memtime, corequant->Cloned());
            else
                return memtime;
        }

        ASTExpression *linktime = link->GetTimeExpression(resource, traits, value);
        if (linktime)
        {
            ///\todo: don't know what to do with serial time.....
            return linktime;
        }

        // Generate an error message
        string msg = string("Cannot construct a time expression: failed to evaluate resource ")
            + "\"" + resource + "\"";
        THROW(GrammaticalError, msg);
    }
    ASTExpression *GetCoresExpression() const
    {
        return corequant->Cloned();
    }
    bool CheckConflict(string ra, string rb) const
    {
        return (core->CheckConflict(ra,rb) ||
                mem->CheckConflict(ra,rb) ||
                cache->CheckConflict(ra,rb) ||
                link->CheckConflict(ra,rb));
                
    }
};

#endif
