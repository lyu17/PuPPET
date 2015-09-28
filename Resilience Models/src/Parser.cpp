// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
#include "Parser.h"

#include <iostream>
#include <deque>
#include <cstdio>
#include <map>
#include <cstring>
#include <libgen.h>

#include "ASTAppModel.h"
#include "ASTMachModel.h"

using namespace std;

// flex and bison are funny; you just define a bunch
// of externs instead of having an include file....

extern vector<int> lineno;
extern vector<int> column;
extern vector<int> filepos;
extern vector<string> filename;
extern string parsedir;

extern int yyparse();
extern FILE *yyin;
extern void yyrestart(FILE*);
extern void yy_scan_string(const char*);
extern ASTAppModel *globalapp;
extern ASTMachModel *globalmach;

static multimap<string,ASTAppModel*> imports;

bool ParseSingleModelString(const std::string &str, ASTAppModel *&app, ASTMachModel *&mach)
{
    // We'll return either the app model or the mach model.
    globalapp = NULL;
    globalmach = NULL;

    // set the state we use in the parser for
    // tracking position (for errors, etc.)
    parsedir = "";
    filename.push_back("<in-memory>");
    lineno.push_back(1);
    column.push_back(1);
    filepos.push_back(0);

    yy_scan_string(str.c_str());
    int err = yyparse();
    if (err)
    {
        cerr << "Error parsing string." << endl;
        return false;
    }

    // Set output values.
    app = globalapp;
    mach = globalmach;

    if (app)
        app->CompleteAndCheck();
    if (mach)
        mach->CompleteAndCheck();

    // Success!
    return true;
}

bool ParseSingleFile(const std::string &fn, ASTAppModel *&app, ASTMachModel *&mach)
{
    // We'll return either the app model or the mach model.
    globalapp = NULL;
    globalmach = NULL;

    // set the state we use in the parser for
    // tracking position (for errors, etc.)
    parsedir = dirname(strdup(fn.c_str()));
    filename.push_back(fn);
    lineno.push_back(1);
    column.push_back(1);
    filepos.push_back(0);

    FILE *f = fopen(fn.c_str(), "r");
    if (!f)
    {
        cerr << "Error: could not open file '"<<fn<<"' for parsing." << endl;
        return false;
    }

    yyin = f;
    // Need yyrestart because of the <<EOF>> manual handling 
    // due to include file parsing in lexer.
    yyrestart(yyin);
    int err = yyparse();
    fclose(f);
    if (err)
    {
        cerr << "Error parsing file '"<<fn<<"'." << endl;
        return false;
    }

    // Set output values.
    app = globalapp;
    mach = globalmach;

    if (app)
        app->CompleteAndCheck();
    if (mach)
        mach->CompleteAndCheck();

    // Success!
    return true;
}

bool ResolveImports(ASTAppModel *app, string basedir)
{
    // If we have an app model, we need to resolve any of is import
    // statements, which in turn may also have import statements.
    // Keep a queue of app models to walk to resolve their imports.
    for (unsigned int i=0; i<app->globals.size(); ++i)
    {
        ASTImportStatement *imp = dynamic_cast<ASTImportStatement*>(app->globals[i]);
        if (imp)
        {
            // We used to only import each imported file once.
            // however; we are lacking a way to clone the imported
            // app models, which means we can't safely override
            // the parameters specified in the import statemtn.
            // As such, re-import each time we encounter a file;

            cerr << "While parsing "<<app->name<<" (basedir="<<basedir<<") "<<": need to import '"<<imp->ident<<"' from file \""<<imp->file<<"\"\n";
            ASTAppModel *importedapp;
            ASTMachModel *importedmach;
            string filename = (basedir == "") ? imp->file : basedir + "/" + imp->file;
            bool success = ParseSingleFile(filename, importedapp, importedmach);

            if (!success)
            {
                cerr << "Error parsing file '"<<filename<<"'.\n";
                return false;
            }
            if (!importedapp)
            {
                cerr << "Could not parse '"<<filename<<"' as an app model.\n";
                return false;
            }
            if (importedapp->name != imp->ident)
            {
                cerr << "Error: Expected to find "<<imp->ident<<" in file \""<<filename<<"\"\n";
                return false;
            }

            // Override the parameters from import statement
            // (we assume the model has been checked and its maps filled)
            for (unsigned int arg=0; arg<imp->arguments.size(); ++arg)
            {
                ///\todo: some of the assignment statements are
                /// for *data* volume idents, not for parameters.....
                const ASTAssignStatement *assign = imp->arguments[arg];
                // I guess we need to expand the assignment value before passing it in....
                // E.g. if two models use "n" for problem size, we override the imported
                // one with the global one by using "with n=n" during the import. If
                // we don't have good name spaces (and I don't think we do yet), then
                // this "n=n" just makes a recursive expansion.

                ///\todo: This isn't right.  We can't simply do:
                ///importedapp->paramMap[assign->name] = assign->value->Expanded(app->paramMap);
                /// because if the overridden variable is a range, then
                /// we're losing our chance to override it in the imported model.
                /// So we're just going to override based on raw value alone.
                /// This causes recursion in some cases because we don't
                /// have completely correct namespace support.
                importedapp->paramMap[assign->name] = assign->value;
            }

            //   add it to the module-kernel-maps
            cerr << "Adding to model "<<app->name<<" import named "<<imp->ident<<endl;
            app->importedModels[imp->ident] = importedapp;

            // keep a copy here in main, so we can print them all
            // without recursing through the main app
            imports.insert(pair<string,ASTAppModel*>(app->name,importedapp));

            // This imported app model might have its own problems
            bool resolved = ResolveImports(importedapp, parsedir);
            if (!resolved)
                return false;
        }
    }
    return true;
}

bool LoadAppOrMachineModel(const std::string &fn,
                           ASTAppModel *&app, ASTMachModel *&mach)
{
    app = NULL;
    mach = NULL;

    bool success = ParseSingleFile(fn, app, mach);
    if (!success)
        return false;

    if (mach)
        return true;

    if (app)
    {
        // take the directory where the app model was
        // parsed, and use it as the base directory
        // for any import statements.
        return ResolveImports(app, parsedir);
    }

    // no machine or app?  logic error....
    return false;
}

bool LoadAppAndMachineModels(const std::string &fn1,
                             const std::string &fn2,
                             ASTAppModel *&app, ASTMachModel *&mach)
{
    ASTAppModel *app1=NULL, *app2=NULL;
    ASTMachModel *mach1=NULL, *mach2=NULL;

    bool success1 = LoadAppOrMachineModel(fn1, app1, mach1);
    if (!success1)
        return false;

    bool success2 = LoadAppOrMachineModel(fn2, app2, mach2);
    if (!success2)
        return false;

    if ((app1 && app2) || (mach1 && mach2))
    {
        cerr << "Error: expected one app model and one machine model.\n";
        return false;
    }

    if (app1 && mach2)
    {
        app = app1;
        mach = mach2;
    }
    else if (app2 && mach1)
    {
        app = app2;
        mach = mach1;
    }
    else
    {
        cerr << "Logic error: expected an app model from one file and a "
             << "machine model from the other.\n";
        return false;
    }

    return true;
}
