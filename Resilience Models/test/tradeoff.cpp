// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include <iostream>
#include <deque>
#include <cstdio>
#include <map>

#include "ASTAppModel.h"
#include "ASTMachModel.h"
#include "Parser.h"

using namespace std;

int main(int argc, char **argv)
{
    try {

        if (argc != 7)
        {
            cerr << "Usage: "<<argv[0]<<" [model.aspen] [machine.aspen] kernel socket0 socket1 param" << endl;
            return 1;
        }

        ASTAppModel *app = NULL;
        ASTMachModel *mach = NULL;
        bool success = false;
        success = LoadAppAndMachineModels(argv[1], argv[2], app, mach);
        if (!success)
            THROW(ModelError, "Unknown error parsing model");

        string kernel = argv[3];
        string socket0 = argv[4];
        string socket1 = argv[5];
        string param = argv[6];

        const ASTKernel *k = app->kernelMap[kernel];
        ASTExpression *expr0 = k->GetTimeExpression(app, mach, socket0);
        ASTExpression *expr1 = k->GetTimeExpression(app, mach, socket1);

        ASTExpression *dssize = k->GetInclusiveDataSizeExpression(app);
        ASTExpression *xfer0 = mach->GetMachine()->GetTimeExpression("intracomm",
                                                                     vector<string>(),
                                                                     false,
                                                                     socket0,
                                                                     dssize->Cloned());
        ASTExpression *xfer1 = mach->GetMachine()->GetTimeExpression("intracomm",
                                                                     vector<string>(),
                                                                     false,
                                                                     socket1,
                                                                     dssize->Cloned());

        NameMap<const ASTExpression*> app_expansions(app->paramMap);
        NameMap<const ASTExpression*> mach_expansions(mach->paramMap);
        app_expansions.Erase(param);
        mach_expansions.Erase(param);

        expr0 = expr0->Expanded(app_expansions)->Expanded(mach_expansions)->Simplified();
        expr1 = expr1->Expanded(app_expansions)->Expanded(mach_expansions)->Simplified();
        xfer0 = xfer0->Expanded(app_expansions)->Expanded(mach_expansions)->Simplified();
        xfer1 = xfer1->Expanded(app_expansions)->Expanded(mach_expansions)->Simplified();

        string rt0 = "rt_" + socket0;
        string rt1 = "rt_" + socket1;
        string xf0 = "xf_" + socket0;
        string xf1 = "xf_" + socket1;

        cout << "#include <math.h>" << endl;
        cout << "double min(double a, double b) {return a>b ? b : a;}" << endl;
        cout << "double min3(double a, double b, double c) {return min(min(a,b),c);}" << endl;
        cout << "double max(double a, double b) {return a<b ? b : a;}" << endl;
        cout << "double max3(double a, double b, double c) {return max(max(a,b),c);}" << endl;
        cout << endl;
        cout << "#define " << rt0 << " 0 + " << expr0->GetText(ASTExpression::C) << "" << endl;
        cout << "#define " << rt1 << " .2 + " << expr1->GetText(ASTExpression::C) << "" << endl;
        cout << "#define " << xf0 << "  " << xfer0->GetText(ASTExpression::C) << "" << endl;
        cout << "#define " << xf1 << "  " << xfer1->GetText(ASTExpression::C) << "" << endl;
        cout << endl;
        cout << "int check(double "<<param<<")" << endl;
        cout << "{" << endl;
        cout << "    return " << rt0 << " + " << xf0 << " < " << rt1 << " + " << xf1 << ";" << endl;
        cout << "}" << endl;
        cout << endl;
        cout << "#ifdef DEBUG" << endl;
        cout << "#include <iostream>" << endl;
        cout << "using namespace std;" << endl;
        cout << "#ifndef VALUES" << endl;
        cout << "#define VALUES {16,32,64,128,256,768,512,1024,1536,2048,3072,4096,8192}" << endl;
        cout << "#endif //VALUES" << endl;
        cout << "int main()" << endl;
        cout << "{" << endl;
        cout << "    cout << \""<<param<<", runtime "<<socket0<<", xfer "<<socket0<<", runtime "<<socket1<<", xfer "<<socket1<<", total "<<socket0<<", total "<<socket1<<", faster\" << endl;" << endl;
        cout << "    const double values[] = VALUES;" << endl;
        cout << "    int nvalues = sizeof(values) / sizeof(values[0]);" << endl;
        cout << "    for (int i=0; i<nvalues; ++i)" << endl;
        cout << "    {" << endl;
        cout << "        double "<<param<<" = values[i];" << endl;
        //cout << "        cout << endl;" << endl;
        //cout << "        cout << \"at "<<param<<"=\"<<"<<param<<"<<endl;" << endl;
        //cout << "        cout << \"    " << rt0 << " = \" << " << rt0 << " << endl;" << endl;
        //cout << "        cout << \"    " << rt1 << " = \" << " << rt1 << " << endl;" << endl;
        //cout << "        cout << \"    " << xf0 << " = \" << " << xf0 << " << endl;" << endl;
        //cout << "        cout << \"    " << xf1 << " = \" << " << xf1 << " << endl;" << endl;
        //cout << "        cout << \"    total " << socket0 << " = \" << " << rt0 << " + " << xf0 << " << endl;" << endl;
        //cout << "        cout << \"    total " << socket1 << " = \" << " << rt1 << " + " << xf1 << " << endl;" << endl;
        //cout << "        cout << \"at "<<param<<"=\"<<"<<param<<"<<\", check() returns \"<<check("<<param<<")<<endl;" << endl;
        cout << "        cout << "<<param<<" << \", \" << "<<rt0<<" << \", \" << "<<xf0<<" << \", \" << "<<rt1<<" << \", \" << "<<xf1<<" << \", \" << ("<<rt0<<"+"<<xf0<<") << \", \" << ("<<rt1<<"+"<<xf1<<") << \", \" << check("<<param<<") << endl;" << endl;
        cout << "    }" << endl;
        cout << "    return 0;" << endl;
        cout << "}" << endl;
        cout << "#endif" << endl;
    }
    catch (const AspenException &exc)
    {
        cerr << exc.PrettyString() << endl;
        return -1;
    }
}
