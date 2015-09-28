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
    string socket = "";
    if (argc == 3)
    {
        success = LoadAppOrMachineModel(argv[1], app, mach);
        socket = argv[2];
    }

    if (!success || !mach)
    {
        cerr << "Usage: "<<argv[0]<<" machine.aspen sockettype" << endl;
        return 1;
    }


    ASTReal *value = new ASTReal(1.e9);
    cout << "max(a,b) = (a > b) ? a : b" << endl;
    cout << "min(a,b) = (a > b) ? b : a" << endl;
    cout << "pow(a,b) = a ** b" << endl;
    cout << endl;
    string plotstring = "plot ";
    double minflops = 0;
    double maxflops = 0;
    double minratio = 0;
    double maxratio = 0;
    for (int i=0; i<6; ++i)
    {
        // six combinations of flops traits
        vector<string> flopstraits;
        if ((i/3) == 0)
            flopstraits.push_back("sp");
        else
            flopstraits.push_back("dp");
        if ((i%3) == 1 || (i%3) == 2)
            flopstraits.push_back("simd");
        if ((i%3) == 2)
            flopstraits.push_back("fmad");

        string name;
        for (unsigned int j=0;j<flopstraits.size(); ++j)
            name += flopstraits[j] + "_";
        name += "flops";

        vector<string> memtraits; // no memory traits

        // calculate bandwidth in gb/sec (i.e. value=1e9)
        double bw = 1. / mach->CountTime("loads", memtraits, socket, value);
        // calculate flops in gflop/s (i.e. value=1e9)
        double flops = 1. / mach->CountTime("flops", flopstraits, socket, value);
        
        // calculate the intensity transition point
        double ratio = flops / bw;

        // keep track of x/y limits for the plot
        if (i==0 || ratio < minratio)
            minratio = ratio;
        if (i==0 || ratio > maxratio)
            maxratio = ratio;
        if (i==0 || flops < minflops)
            minflops = flops;
        if (i==0 || flops > maxflops)
            maxflops = flops;

        cout << name << "(x) = min(" << flops << ", x*"<<bw<<")\n";

        plotstring += name + "(x) with lines linewidth 2 ti \""+name+"\"";
        if (i+1<6)
            plotstring += ", ";
    }

    double xmin = pow(2,    floor(log(minratio)/log(2)));
    double xmax = pow(2, 1.+floor(log(maxratio)/log(2)));
    double ymin = pow(10,    floor(log(minflops)/log(10)));
    double ymax = pow(10, 1.+floor(log(maxflops)/log(10)));

    cout << "# Using specified X range" << endl;
    cout << "set xrange ["<<xmin<<":"<<xmax<<"]" << endl;
    cout << endl;
    cout << "# Using specified Y range" << endl;
    cout << "set yrange ["<<ymin<<":"<<ymax<<"]" << endl;
    cout << endl;
    cout << "set logscale x 2" << endl;
    cout << "set logscale y" << endl;
    cout << endl;
    cout << "set key top left" << endl;
    cout << "# X Axis Label" << endl;
    cout << "set xlabel \"Flop:Byte Ratio\"" << endl;
    cout << "# Y Axis Label" << endl;
    cout << "set ylabel \"GFLOPS\"" << endl;
    cout << "set title \"Achievable GFLOPS for "<<socket<<" on "<<mach->GetMachine()->name<<"\"" << endl;
    cout << endl;

    cout << plotstring << endl;

    cout << "pause -1 \"hit any key to continue\"" << endl;

    delete value;
    delete mach;

    return 0;
  }
  catch (const AspenException &exc)
  {
    cerr << exc.PrettyString() << endl;
    return -1;
  }
}
