// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#ifndef AST_MACH_ELEMENT_H
#define AST_MACH_ELEMENT_H

#include "ASTMachComponent.h"

// ****************************************************************************
// Class:  ASTMachElement
//
// Purpose:
///   The smallest pieces of a machine, such as a core, memory, bus, 
///   cache, or interconnect.
//
// Programmer:  Jeremy Meredith
// Creation:    May 21, 2013
//
// Modifications:
// ****************************************************************************
class ASTMachElement : public ASTMachComponent
{
  public:
    const vector<const ASTMachProperty*> properties;
    const vector<const ASTMachResource*> resources;
    const vector< pair<string, string> > conflicts;
  public:
    ASTMachElement(string name,
                   ParseVector<ASTMachProperty*> properties,
                   ParseVector<ASTMachResource*> resources,
                   ParseVector< pair<string,string> > conflicts)
        : ASTMachComponent(name), properties(properties.begin(), properties.end()), resources(resources.begin(), resources.end()), conflicts(conflicts.begin(), conflicts.end())
    {
    }
    virtual ~ASTMachElement()
    {
        for (unsigned int i=0; i<properties.size(); ++i)
            delete properties[i];
        for (unsigned int i=0; i<resources.size(); ++i)
            delete resources[i];
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        for (unsigned int i=0; i<properties.size(); ++i)
            properties[i]->Print(out, indent+1);
        for (unsigned int i=0; i<resources.size(); ++i)
            resources[i]->Print(out, indent+1);
        for (unsigned int i=0; i<conflicts.size(); ++i)
            out << Indent(indent+1) << "Conflict: "
                << conflicts[i].first << ","
                << conflicts[i].second << endl;
    }
    virtual void CompleteAndCheck(const NameMap<const ASTMachComponent*> &compMap)
    {
    }
    ASTExpression *GetSerialResourceDynamicPowerExpression(const string &resource) const
    {
        for (unsigned int i=0; i<resources.size(); ++i)
        {
            const ASTMachResource *r = resources[i];
            if (r->name == resource)
            {
                if (r->power && r->power->dynamicPower)
                {
                    return r->power->dynamicPower->Cloned();
                }
            }
        }
        return NULL;
    }
    ASTExpression *GetTimeExpression(const string &resource,
                                     const vector<string> &traits, 
                                     const ASTExpression *value) const
    {
        for (unsigned int i=0; i<resources.size(); ++i)
        {
            const ASTMachResource *r = resources[i];
            if (r->name == resource)
            {
                NameMap<const ASTExpression*> valmap(r->arg, value);
                ASTExpression *expr = r->quantity->Expanded(valmap);

                // each trait applies another expression to our
                // current time calculation
                for (unsigned int j=0; j<traits.size(); ++j)
                {
                    string tn = traits[j];
                    const ASTTrait *t = r->GetTrait(tn);
                    if (!t)
                        continue;

                    // we assume traits use "base" as the starting resource value
                    NameMap<const ASTExpression*> basemap("base", expr);
                    ASTExpression *newexpr = t->quantity->Expanded(basemap);
                    delete expr; // we just made a copy of this
                    expr = newexpr;
                }

                return expr;
            }
        }
        // We didn't have this resource.
        // What to return?  0?  No, that would be incorrect.
        // let's make it NULL so callers can check for this condition explicitly.
        return NULL;
    }
    bool CheckConflict(string ra, string rb) const
    {
        for (unsigned int i=0; i<conflicts.size(); ++i)
        {
            if ((conflicts[i].first == ra && conflicts[i].second == rb) ||
                (conflicts[i].second == ra && conflicts[i].first == rb))
            {
                return true;
            }   
        }
        return false;
    }
};


class ASTMachMem : public ASTMachElement
{
  public:
    ASTMachMem(string name,
               ParseVector<ASTMachProperty*> properties,
               ParseVector<ASTMachResource*> resources,
               ParseVector< pair<string,string> > conflicts)
        : ASTMachElement(name,properties,resources,conflicts)
    {
    }
    virtual ~ASTMachMem()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Memory '"<<name<<"':" << endl;
        ASTMachElement::Print(out,indent);
    }
};

class ASTMachCache : public ASTMachElement
{
  public:
    ASTMachCache(string name,
               ParseVector<ASTMachProperty*> properties,
               ParseVector<ASTMachResource*> resources,
               ParseVector< pair<string,string> > conflicts)
        : ASTMachElement(name,properties,resources,conflicts)
    {
    }
    virtual ~ASTMachCache()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Cache '"<<name<<"':" << endl;
        ASTMachElement::Print(out,indent);
    }
};

class ASTMachCore : public ASTMachElement
{
  public:
    ASTMachCore(string name,
               ParseVector<ASTMachProperty*> properties,
               ParseVector<ASTMachResource*> resources,
               ParseVector< pair<string,string> > conflicts)
        : ASTMachElement(name,properties,resources,conflicts)
    {
    }
    virtual ~ASTMachCore()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Core '"<<name<<"':" << endl;
        ASTMachElement::Print(out,indent);
    }
};

class ASTMachLink : public ASTMachElement
{
  public:
    ASTMachLink(string name,
               ParseVector<ASTMachProperty*> properties,
               ParseVector<ASTMachResource*> resources,
               ParseVector< pair<string,string> > conflicts)
        : ASTMachElement(name,properties,resources,conflicts)
    {
    }
    virtual ~ASTMachLink()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Link '"<<name<<"':" << endl;
        ASTMachElement::Print(out,indent);
    }
};


class ASTMachInterconnect : public ASTMachElement
{
  public:
    ASTMachInterconnect(string name,
               ParseVector<ASTMachProperty*> properties,
               ParseVector<ASTMachResource*> resources,
               ParseVector< pair<string,string> > conflicts)
        : ASTMachElement(name,properties,resources,conflicts)
    {
    }
    virtual ~ASTMachInterconnect()
    {
    }
    virtual void Print(ostream &out, int indent = 0) const
    {
        out << Indent(indent) << "Interconnect '"<<name<<"':" << endl;
        ASTMachElement::Print(out,indent);
    }
};


#endif
