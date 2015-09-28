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
    if (argc == 2)
    {
        success = LoadAppOrMachineModel(argv[1], app, mach);
    }
    else if (argc == 3)
    {
        success = LoadAppAndMachineModels(argv[1], argv[2], app, mach);
    }
    else
    {
        cerr << "Usage: "<<argv[0]<<" [model.aspen] [machine.amm]" << endl;
        return 1;
    }

    if (!success)
    {
        cerr << "Errors encountered during parsing.  Aborting.\n";
        return -1;
    }

    cout << "\n------------------- Syntax Trees ---------------------\n\n";
    if (mach)
    {
        cout << "----- Main Machine Model -----\n";
        mach->Print(cout);
    }
    cerr << "\n";
    if (app)
    {
        cout << "----- Main Application Model -----\n";
        app->Print(cout);
    }
    cerr << "\n";
    // For the moment, we're not going to print out the detailed imported models.
    //for (multimap<string,ASTAppModel*>::iterator it = imports.begin(); it != imports.end(); ++it)
    //{
    //    cout << "----- Imported Application Model (within "<<it->first<<") -----\n";
    //    it->second->Print(cout);
    //}
    cout << "\n-----------------------------------------------------\n\n";


    vector<string> names;
    vector<double> defvals, minvals, maxvals;
    int nparams = app ? app->FindParametersWithRanges(names,defvals,minvals,maxvals) : 0;
    if (app)
    {
        try
        {
            cout <<"\n ------  Application Analysis ------\n";
            cout << ">> Basic control flow expression\n";
            ExprPtr ctrl(app->GetControlFlowExpression());
            ExprPtr simpctrl(ctrl->Simplified());
            cout << "flops    = " << simpctrl->GetText()  << endl;;
            cout << endl;

            cout << ">> Raw expression without variable expansion:\n";
            cout << "flops    = " << app->GetResourceRequirementExpressionText("flops",    false, false) << "\n";
            cout << "messages = " << app->GetResourceRequirementExpressionText("messages", false, false) << "\n";
            cout << "loads    = " << app->GetResourceRequirementExpressionText("loads",    false, false) << "\n";
            cout << "stores   = " << app->GetResourceRequirementExpressionText("stores",   false, false) << "\n";
            cout << endl;

            cout << ">> Raw expression with variable expansion:\n";
            cout << "flops    = " << app->GetResourceRequirementExpressionText("flops",    true, false) << "\n";
            cout << "messages = " << app->GetResourceRequirementExpressionText("messages", true, false) << "\n";
            cout << "loads    = " << app->GetResourceRequirementExpressionText("loads",    true, false) << "\n";
            cout << "stores   = " << app->GetResourceRequirementExpressionText("stores",   true, false) << "\n";
            cout << endl;

            cout << ">> as values, With parameters using default values\n";
            cout << "flops    = " << app->Count("flops")  << endl;;
            cout << "messages = " << app->Count("messages") << endl;;
            cout << "loads    = " << app->Count("loads")  << endl;;
            cout << "stores   = " << app->Count("stores") << endl;;
            cout << endl;

            cout << ">> Simplification test:\n";
            cout << "flops (noexp)       = " << app->GetResourceRequirementExpressionText("flops", false, false) << "\n";
            cout << "flops (noexp,simpl) = " << app->GetResourceRequirementExpressionText("flops", false, true) << "\n";
            cout << "flops (exp)         = " << app->GetResourceRequirementExpressionText("flops", true, false) << "\n";
            cout << "flops (exp,simpl)   = " << app->GetResourceRequirementExpressionText("flops", true, true) << "\n";

            // parameters
            cout << "\nThere are "<<nparams<<" parameters with ranges.\n";

            for (int i=0; i<nparams; ++i)
            {
                cout << ">> with parameter ''"<<names[i]<<"'' set to its minimum of "<<minvals[i]<<":"<<endl;
                cout << "flops    = " << app->Count("flops",    names[i], minvals[i]) << endl;;
                cout << "messages = " << app->Count("messages", names[i], minvals[i]) << endl;;
                cout << "loads    = " << app->Count("loads",    names[i], minvals[i]) << endl;;
                cout << "stores   = " << app->Count("stores",   names[i], minvals[i]) << endl;;
                cout << endl;

                cout << "-> and now with ''"<<names[i]<<"'' set to its maximum of "<<maxvals[i]<<":"<<endl;
                cout << "flops    = " << app->Count("flops",    names[i], maxvals[i]) << endl;;
                cout << "messages = " << app->Count("messages", names[i], maxvals[i]) << endl;;
                cout << "loads    = " << app->Count("loads",    names[i], maxvals[i]) << endl;;
                cout << "stores   = " << app->Count("stores",   names[i], maxvals[i]) << endl;;
                cout << endl;

                cout << ">> Expression with parameter ''"<<names[i]<<"'' left as a variable (named x) (and simplified):\n";
                NameMap<const ASTExpression*> subst;
                subst[names[i]] = new ASTIdentifier("x");
                cout << "flops = " << app->GetResourceRequirementExpressionText("flops", true, true, subst) << "\n";
                delete subst[names[i]];
                cout << endl;
            }
            cout << endl;

            for (unsigned int i=0; i<app->kernels.size(); ++i)
            {
                try
                {
                    ASTKernel *k = app->kernels[i];
                    cout << "\n\n>> Kernel "<<k->name<<endl<<endl;;

                    ExprPtr flops(k->GetResourceRequirementExpression(app,"flops"));
                    ExprPtr loads(k->GetResourceRequirementExpression(app,"loads"));
                    ExprPtr stores(k->GetResourceRequirementExpression(app,"stores"));
                    ExprPtr bytes(k->GetResourceRequirementExpression(app,"bytes"));
                    cout << "Raw flops for kernel '"<<k->name<<"' = "
                         << ExprPtr(flops->Expanded(app->paramMap))->Evaluate() << endl;
                    cout << "Raw loads for kernel '"<<k->name<<"' = "
                         << ExprPtr(loads->Expanded(app->paramMap))->Evaluate() << endl;
                    cout << "Raw stores for kernel '"<<k->name<<"' = "
                         << ExprPtr(stores->Expanded(app->paramMap))->Evaluate() << endl;
                    cout << endl;

                    
                    cout << "Exclusive set size is " << ExprPtr(k->GetExclusiveDataSizeExpression(app))->GetText() << endl;
                    cout << "Inclusive set size is " << ExprPtr(k->GetInclusiveDataSizeExpression(app))->GetText() << endl;
                    cout << endl;

                    cout << "Calculating flops/byte intensity for kernel '"<<k->name<<"':"<<endl;
                    ExprPtr intensity(new ASTBinaryExpression("/",
                                                              flops->Cloned(),
                                                              bytes->Cloned()));


                    cout << "  = " << intensity->GetText() << endl;

                    // note: we can't simply expand once for the substitution,
                    // then expand again for the kernel parameters, because
                    // the variable to substitute for x may not appear
                    // until we start expanding the other variables.
                    // so take the app's paramMap and override the var in that.
                    ///\todo: to be safe, these kinds of substitutions should
                    /// at least ensure that 'x' doesn't appear as a parameter
                    /// in the app, or else we might get odd results.
                    NameMap<const ASTExpression*> subst = app->paramMap;
                    // nAtom for CoMD, tf for echelon, n for fft
                    subst["nAtom"] = new ASTIdentifier("x");

                    cout << "  expanding, but in terms of x:\n";
                    cout << "  = " << ExprPtr(intensity->Expanded(subst))->GetText() << endl;
                    delete subst["nAtom"];
                }
                catch (const AspenException &exc)
                {
                    cerr << exc.PrettyString() <<endl;
                }
            }
        }
        catch (const AspenException &exc)
        {
            cerr << exc.PrettyString() << endl;
        }
    }

    if (mach)
    {
        cout << "\n ------  Machine Analysis ------\n";
        const char *sockettypes[] = {"nvidia_m2090","intel_xeon_x5660",NULL};
        //const char *sockettypes[] = {"amd_a8_3850_cpu","amd_a8_3850_gpu",NULL};
        //const char *sockettypes[] = {"intel_xeon_e5_2680",NULL};
        for (int i=0; sockettypes[i]; ++i)
        {
          try
          {
            string socket = sockettypes[i];
            cout << "\n\n>> for socket type '"<<socket<<"' <<\n";
            ExprPtr tcexpr(mach->GetMachine()->GetTotalCoresExpression(socket));
            cout << "  totalcores = "<<tcexpr->GetText()<<endl;
            cout << "  totalcores = "<<ExprPtr(tcexpr->Expanded(mach->paramMap))->Evaluate()<<endl;
            cout << endl;

            ASTReal *value = new ASTReal(1.e9);

            vector<string> traits;
            cout << "  peak sp gflops: " << 1. / mach->CountTime("flops", traits, socket, value) << endl;
            traits.push_back("simd");
            cout << "  peak sp/simd gflops: " << 1. / mach->CountTime("flops", traits, socket, value) << endl;
            traits.push_back("fmad");
            cout << "  peak sp/simd/fmad gflops: " << 1. / mach->CountTime("flops", traits, socket, value) << endl;
            traits.clear();
            traits.push_back("dp");
            cout << "  peak dp gflops: " << 1. / mach->CountTime("flops", traits, socket, value)  << endl;
            traits.push_back("simd");
            cout << "  peak dp/simd gflops: " << 1. / mach->CountTime("flops", traits, socket, value)  << endl;
            traits.push_back("fmad");
            cout << "  peak dp/simd/fmad gflops: " << 1. / mach->CountTime("flops", traits, socket, value)  << endl;

            vector<string> memtraits;
            cout << "  ...\n";
            cout << "  peak bw in GB/sec: " << 1. / mach->CountTime("loads", memtraits, socket, value) << endl;

            cout << "\n\n>> testing expressions\n";
            traits.clear();
            cout << "  time to process 1e9 sp flops in sec: " << mach->GetTimeExprText("flops", traits, socket, false, value) << endl;
            cout << "  time to process 1e9 sp flops in sec (expanded): " << mach->GetTimeExprText("flops", traits, socket, true, value) << endl;
            traits.push_back("simd");
            cout << "  time to process 1e9 sp/simd flops in sec: " << mach->GetTimeExprText("flops", traits, socket, false, value) << endl;
            cout << "  time to process 1e9 sp/simd flops in sec (expanded): " << mach->GetTimeExprText("flops", traits, socket, true, value) << endl;
            cout << "  time to read 1e9 bytes in sec: " << mach->GetTimeExprText("loads", memtraits, socket, false, value) << endl;
            cout << "  time to read 1e9 bytes in sec (expanded): " << mach->GetTimeExprText("loads", memtraits, socket, true, value) << endl;

            delete value;
          }
          catch (const AspenException& exc)
          {
              cerr << exc.PrettyString() <<endl;
          }
        }
    }

    if (app && mach)
    {
        cout << "\n ------  Combined Analysis ------\n";
        const char *sockettypes[] = {"nvidia_m2090","intel_xeon_x5660",NULL};
        //const char *sockettypes[] = {"amd_a8_3850_cpu","amd_a8_3850_gpu",NULL};
        //const char *sockettypes[] = {"intel_xeon_e5_2680",NULL};
        for (int i=0; sockettypes[i]; ++i)
        {
            string socket = sockettypes[i];
            for (unsigned int i=0; i<app->kernels.size(); ++i)
            {
                try
                {
                    ASTKernel *k = app->kernels[i];
                    ExprPtr datasize(k->GetInclusiveDataSizeExpression(app));
                    cout << endl << endl << "++Predicting runtime on kernel '"<<k->name<<"' for socket type "<<socket<<endl;
                    ExprPtr expr(k->GetTimeExpression(app, mach, socket));
                    cout << "run time (expression) = " << expr->GetText() << endl;
                    cout << "run time (value)      = " << ExprPtr(ExprPtr(expr->Expanded(app->paramMap))->Expanded(mach->paramMap))->Evaluate() << endl;
                    ExprPtr intracomm(mach->GetMachine()->GetTimeExpression("intracomm",
                                                                            vector<string>(),
                                                                            false,
                                                                            socket,
                                                                            datasize->Cloned()));

                    /*
                    ///\todo: big manual hack to check per-block socket override
                    cerr << "run time base = " << k->GetTimeExpression(mach,socket)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    map<string,string> f1,f2,w1,w2,fb,wb;
                    f1["firstblock"] = "nvidia_m2090";
                    w1["firstblock"] = "westmere";
                    f2["secondblock"] = "nvidia_m2090";
                    w2["secondblock"] = "westmere";
                    fb["firstblock"] = "nvidia_m2090";
                    fb["secondblock"] = "nvidia_m2090";
                    wb["firstblock"] = "westmere";
                    wb["secondblock"] = "westmere";
                    cerr << "run time with firstblock=nvidia_m2090     = " << k->GetTimeExpression(mach,socket,f1)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    cerr << "run time with secondblock=nvidia_m2090    = " << k->GetTimeExpression(mach,socket,f2)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    cerr << "run time with both blocks=nvidia_m2090    = " << k->GetTimeExpression(mach,socket,fb)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    cerr << "run time with firstblock=westmere  = " << k->GetTimeExpression(mach,socket,w1)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    cerr << "run time with secondblock=westmere = " << k->GetTimeExpression(mach,socket,w2)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    cerr << "run time with both blocks=westmere = " << k->GetTimeExpression(mach,socket,wb)->Expanded(app->paramMap)->Expanded(mach->paramMap)->Evaluate()<< endl;
                    */

                    cout << "app model data transfer time = "<<intracomm->GetText()<<endl;
                    int nsteps = 10;
                    for (int i=0; i<nparams; ++i)
                    {
                        cout << "Scaling over param '"<<names[i]<<"':\n";
                        cout << "value\truntime \tdatatime\tsum\n";
                        double scale = maxvals[i] / minvals[i];
                        for (int step=0; step<nsteps; step++)
                        {
                            double val = minvals[i] * exp(log(scale) * double(step)/double(nsteps-1));
                            NameMap<const ASTExpression*> valmap;
                            valmap[names[i]] = new ASTReal(val);
                            double runtime = ExprPtr(ExprPtr(ExprPtr(expr->Expanded(valmap))->Expanded(app->paramMap))->Expanded(mach->paramMap))->Evaluate();
                            double xfertime = ExprPtr(ExprPtr(ExprPtr(intracomm->Expanded(valmap))->Expanded(app->paramMap))->Expanded(mach->paramMap))->Evaluate();
                            cout << val<<" \t"<<runtime<<" \t"<<xfertime<<" \t"<<runtime+xfertime<<endl;
                            delete valmap[names[i]];
                        }
                    }


                    ASTExpression *flopsDE =
                        k->GetDynamicEnergyExpressionForResource("flops",
                                                                 socket,
                                                                 mach);
                    cout << "flops dynamic power (J) = " << (flopsDE ? flopsDE->GetText() : "(nothing)") << endl;
                    cout << "flops dynamic power (J) = " << (flopsDE ? ExprPtr(flopsDE->Expanded(mach->paramMap))->GetText() : "(nothing)") << endl;
                    ASTExpression *loadsDE =
                        k->GetDynamicEnergyExpressionForResource("loads",
                                                                 socket,
                                                                 mach);
                    cout << "loads dynamic power (J) = " << (loadsDE ? loadsDE->GetText() : "(nothing)")  << endl;
                    cout << "loads dynamic power (J) = " << (loadsDE ? ExprPtr(loadsDE->Expanded(mach->paramMap))->GetText() : "(nothing)")  << endl;
                    ASTExpression *storesDE =
                        k->GetDynamicEnergyExpressionForResource("stores",
                                                                 socket,
                                                                 mach);
                    cout << "stores dynamic power (J) = " << (storesDE ? storesDE->GetText() : "(nothing)")  << endl;
                    cout << "stores dynamic power (J) = " << (storesDE ? ExprPtr(storesDE->Expanded(mach->paramMap))->GetText() : "(nothing)")  << endl;

                    if (flopsDE)
                        delete flopsDE;
                    if (loadsDE)
                        delete loadsDE;
                    if (storesDE)
                        delete storesDE;

                }
                catch (const AspenException &exc)
                {
                    cerr << exc.PrettyString() <<endl;
                }
            }
        }
    }

    if (app)
        delete app;
    if (mach)
        delete mach;

    return 0;
  }
  catch (const AspenException &exc)
  {
      cerr << exc.PrettyString() << endl;
      return -1;
  }
}
