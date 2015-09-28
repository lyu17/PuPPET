#include "ASTResilienceModel.h"

ASTResilienceModel::ASTResilienceModel(const string &n, ParseVector<ASTResilienceStatement*> resiliencestmts)
    : name(n), resiliencestmts(resiliencestmts.begin(),resiliencestmts.end())
{
	resiliencestmts[0]->Export(std::cout, 0);
}

ASTResilienceModel::~ASTResilienceModel()
{
    for (unsigned int i=0; i<resiliencestmts.size(); i++)
        delete resiliencestmts[i];
}


void
ASTResilienceModel::Print(ostream &out, int indent) const
{
    out << Indent(indent) << "Resilience model "<<name<< ":" << endl;
    out << Indent(indent+1) << "Statements:" << endl;
    for (unsigned int i=0; i<resiliencestmts.size(); i++)
    {
    	resiliencestmts[i]->Print(out, indent+2);
    }
}

void
ASTResilienceModel::Export(ostream &out, int indent) const
{
    out << "model " <<name<<" {" << endl;
    out << endl;
    for (unsigned int i=0; i<resiliencestmts.size(); i++)
    {
    	resiliencestmts[i]->Export(out, indent+1);
    }
    out << "}" << endl;
}

