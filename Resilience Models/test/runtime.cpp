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
    ASTAppModel *app = NULL;
    ASTMachModel *mach = NULL;

    bool success = false;
    
    if (argc >= 3)
        success = LoadAppAndMachineModels(argv[1], argv[2], app, mach);

    if (argc < 4 || argc == 5 || !success)
    {
        cerr << "Usage: "<<argv[0]<<" <app.aspen> <mach.aspen> <socket> [param value1 [value2 [value3]] ...]" << endl;
        cerr << "   for example: "<<argv[0]<<" md.aspen keeneland.aspen nvidia_m2090 nAtoms 100 1000 10000" << endl;
        cerr << "   or simply:   "<<argv[0]<<" md.aspen keeneland.aspen nvidia_m2090" << endl;
        if (success && argc == 3)
        {
            vector<string> socketnames = mach->GetMachine()->GetSocketNames();
            cerr << endl << "Please select a socket." << endl;
            cerr << "Valid choices for your selected machine model are:" << endl;
            for (unsigned int i=0; i<socketnames.size(); ++i)
            {
                cerr << "  " << socketnames[i] << endl;
            }
        }
        return 1;
    }

    string socket = argv[3];

    const ASTKernel *k = app->mainKernel;

    int niter = 1.;
    string param = "";
    if (argc > 5)
    {
        niter = argc - 5;
        param = argv[4];
        cout << param << "\t" << "runtime" << endl;
        cout << "--\t--" << endl;
    }

    NameMap<const ASTExpression*> app_expansions(app->paramMap);
    NameMap<const ASTExpression*> mach_expansions(mach->paramMap);
    if (param != "")
    {
        app_expansions.Erase(param);
        mach_expansions.Erase(param);

        // debugging printout:
        if (false)
        {
            k->GetTimeExpression(app, mach, socket)
                ->Expanded(app_expansions)
                ->Expanded(mach_expansions)
                ->Simplified()
                ->Print(cerr);
        }

        cout << k->GetTimeExpression(app, mach, socket)
                 ->Expanded(app_expansions)
                 ->Expanded(mach_expansions)
                 ->GetText() << endl;

        cout << k->GetTimeExpression(app, mach, socket)
                 ->Expanded(app_expansions)
                 ->Expanded(mach_expansions)
                 ->Simplified()
                 ->GetText() << endl;
    }
    for (int iter = 0 ; iter < niter; ++iter)
    {
        NameMap<const ASTExpression*> param_expansion;
        if (param != "")
        {
            double val = strtod(argv[iter+5],NULL);
            param_expansion[param] = new ASTReal(val);
            cout << val << "\t";
        }
        
        cout << k->GetTimeExpression(app, mach, socket)
                 ->Expanded(app_expansions)
                 ->Expanded(mach_expansions)
                 ->Expanded(param_expansion)
                 ->Evaluate() << endl;

        // debugging printout to test simplify:
        if (false)
        {
            cout << "check:\t" << k->GetTimeExpression(app, mach, socket)
                ->Expanded(app_expansions)
                ->Expanded(mach_expansions)
                ->Expanded(param_expansion)
                ->Simplified()
                ->Evaluate() << endl;
        }
    }

    delete app;

    return 0;
  }
  catch (const AspenException &exc)
  {
      cerr << exc.PrettyString() << endl;
      return -1;
  }
}
