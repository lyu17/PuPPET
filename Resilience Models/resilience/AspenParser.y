// Copyright 2012-2013 UT-Battelle, LLC.  See LICENSE.txt for more information.
%{
    #include "AST.h"
    #include "AspenParseNode.h"
    ASTAppModel *globalapp = NULL;
    ASTMachModel *globalmach = NULL;
    
    /* resilience modeling, added by Li Yu in May 2014 */
    ASTResilienceModel *globalresilience = NULL;

    extern int yylex();
    extern int yylineno;
    extern char *yytext;
    void yyerror(const char *);
%}

%locations

%code requires {
#include "AspenParseNode.h"

typedef struct YYLTYPE {
  int first_line;
  int first_column;
  int last_line;
  int last_column;
  int first_filepos;
  int last_filepos;
  string filename;
} YYLTYPE;

# define YYLTYPE_IS_DECLARED 1 /* alert the parser that we have our own definition */

# define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                 \
      if (N)                                                           \
        {                                                              \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;       \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;     \
          (Current).first_filepos= YYRHSLOC (Rhs, 1).first_filepos;    \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;        \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;      \
          (Current).last_filepos = YYRHSLOC (Rhs, N).last_filepos;     \
          (Current).filename     = YYRHSLOC (Rhs, 1).filename;         \
        }                                                              \
      else                                                             \
        { /* empty RHS */                                              \
          (Current).first_line   = (Current).last_line   =             \
            YYRHSLOC (Rhs, 0).last_line;                               \
          (Current).first_column = (Current).last_column =             \
            YYRHSLOC (Rhs, 0).last_column;                             \
          (Current).first_filepos = (Current).last_filepos =           \
            YYRHSLOC (Rhs, 0).last_filepos;                            \
          (Current).filename  = "";                                    \
        }                                                              \
    while (0)

}
/* Define our terminal symbols (tokens). This should
   match our tokens.l lex file. We also define the node type
   they represent.
 */
%token <token> TKW_PARAM
%token <token> TKW_IN
%token <token> TKW_WITH
%token <token> TKW_OF
%token <token> TKW_SIZE

%token <token> TKW_MODEL
%token <token> TKW_KERNEL
%token <token> TKW_DATA
%token <token> TKW_IMPORT
%token <token> TKW_AS
%token <token> TKW_TO
%token <token> TKW_FROM
%token <token> TKW_SET
%token <token> TKW_ITERATE
%token <token> TKW_MAP
%token <token> TKW_PAR
%token <token> TKW_SEQ
%token <token> TKW_EXECUTE

%token <token> TKW_IF
%token <token> TKW_ELSE
%token <token> TKW_PROBABILITY

%token <token> TKW_INCLUDE
%token <token> TKW_NODE
%token <token> TKW_MACHINE
%token <token> TKW_SOCKET
%token <token> TKW_CACHE
%token <token> TKW_LINK
%token <token> TKW_RESOURCE
%token <token> TKW_CAPACITY
%token <token> TKW_LATENCY
%token <token> TKW_BANDWIDTH
%token <token> TKW_CORE
%token <token> TKW_MEMORY
%token <token> TKW_INTERCONNECT
%token <token> TKW_CONFLICT
%token <token> TKW_PRIVATE
%token <token> TKW_SHARED
%token <token> TKW_POWER
%token <token> TKW_STATIC
%token <token> TKW_DYNAMIC

%token <str> TIDENT TSTRING TINT TREAL TKW_10POWER
%token <token> TDOTDOT TARROW TCEQ TCNE TCLT TCLE TCGT TCGE TEQUAL
%token <token> TLPAREN TRPAREN TLBRACE TRBRACE TLBRACKET TRBRACKET
%token <token> TCOMMA TSEMI TDOT
%token <token> TPLUS TMINUS TMUL TDIV TEXP
%token <token> TNEWLINE
%token <token> TCOMPEQ TCOMPNE TCOMPLT TCOMPGT TCOMPLE TCOMPGE
%token <token> TAND TOR

/* Define the type of node our nonterminal symbols represent.
   The types refer to the %union declaration above. Ex: when
   we call an ident (defined by union type ident) we are really
   calling an (NIdentifier*). It makes the compiler happy.
 */
%type <app> app
%type <mach> mach
%type <expr> expr value quantity constant comparison
%type <exprlist> arguments
%type <stmt> globalstmt paramstmt datastmt importstmt
%type <stmtlist> globalstatements
%type <execstmt> execstmt
%type <execstmtlist> execstatements
%type <assignlist> namedarguments
%type <kernel> kernel
%type <kernellist> kernels
%type <controlstmt> kernelstmt controlstmt kernelcall ifthencontrol probabilitycontrol
%type <controlstmtlist> kernelstatements
%type <vardecl> vardecl
%type <vardecllist> vardecllist
%type <asitem> asitem
%type <asitemlist> as asitemlist
%type <probabilitylist> probabilityitems
%type <ident> ident
%type <str> tofrom
%type <call> functioncall
%type <component> component
%type <power> optionalpower
%type <resource> resource
%type <resourcelist> optionalresources resources
%type <traitlist> traits
%type <conflictlist> optionalconflicts conflicts
%type <propertylist> optionalproperties properties
%type <socketlist> sockets
%type <machcontent> machcontent

/* resilience modeling, added by Li Yu in May 2014 */
%token <token> TKW_DATAINF
%token <token> TKW_DATASTR

%type <resilience> resilience
%type <rstmt> resiliencestmt
%type <rstmtlist> resiliencestmts

/* we need some sort of associativity for the dependency arrow */
%left TARROW

/* Operator associativity and precedence for mathematical operators */
%left TAND TOR
%left TCOMPEQ TCOMPNE TCOMPLT TCOMPGT TCOMPLE TCOMPGE
%left TPLUS TMINUS
%left TMUL TDIV
%left NEG
%right TEXP

%start begin

%%

begin
: mach
{
    globalmach = $1;
}
| app
{
    globalapp = $1;
}
/* resilience modeling, added by Li Yu in May 2014 */
| resilience
{
    globalresilience = $1;
}
;

mach
: machcontent
{
    $$ = new ASTMachModel($1.first, $1.second);
    $1.first.release();
    $1.second.release();
}
;

machcontent
: globalstmt
{
    // allocate/clear both vectors, because we don't know
    // if the next one might be a component
    $$.first.clear();
    $$.second.clear();
    $$.first.push_back($1);
}
| component
{
    // allocate/clear both vectors, because we don't know
    // if the next one might be a globalstmt
    $$.first.clear();
    $$.second.clear();
    $$.second.push_back($1);
}
| machcontent globalstmt
{
    $$ = $1;
    $$.first.push_back($2);
}
| machcontent component
{
    $$ = $1;
    $$.second.push_back($2);
}
;

component
: TKW_SOCKET TIDENT TLBRACE
  quantity TIDENT TKW_CORE
  TIDENT TKW_MEMORY
  TIDENT TIDENT TKW_CACHE
  TKW_LINK TKW_WITH TIDENT
  optionalpower
  TRBRACE
{
    $$ = new ASTMachSocket($2,
                           $4,$5,
                           $7,
                           $9,$10,
                           $14,
                           $15);
}
| TKW_CORE TIDENT TLBRACE optionalproperties optionalresources optionalconflicts TRBRACE
{
    $$ = new ASTMachCore($2, $4, $5, $6);
    $4.release();
    $5.release();
    $6.release();
}
| TKW_LINK TIDENT TLBRACE optionalproperties optionalresources optionalconflicts TRBRACE
{
    $$ = new ASTMachLink($2, $4, $5, $6);
    $4.release();
    $5.release();
    $6.release();
}
| TKW_MEMORY TIDENT TLBRACE optionalproperties optionalresources optionalconflicts TRBRACE
{
    $$ = new ASTMachMem($2, $4, $5, $6);
    $4.release();
    $5.release();
    $6.release();
}
| TKW_CACHE TIDENT TLBRACE optionalproperties optionalresources optionalconflicts TRBRACE
{
    $$ = new ASTMachCache($2, $4, $5, $6);
    $4.release();
    $5.release();
    $6.release();
}
| TKW_INTERCONNECT TIDENT TLBRACE optionalproperties optionalresources optionalconflicts TRBRACE
{
    $$ = new ASTMachInterconnect($2, $4, $5, $6);
    $4.release();
    $5.release();
    $6.release();
}
| TKW_NODE TIDENT TLBRACE sockets TRBRACE
{
    $$ = new ASTMachNode($2, $4);
    $4.release();
}
| TKW_MACHINE TIDENT TLBRACE
  quantity TIDENT TKW_NODE
  TIDENT TKW_INTERCONNECT as
  optionalresources
  TRBRACE
{
    $$ = new ASTMachine($2,
                        $4, $5,
                        $7, $9,
                        $10);
    $9.release();
    $10.release();
}
| TKW_MACHINE TIDENT TLBRACE
  quantity TIDENT TKW_NODE
  optionalresources
  TRBRACE
{
    ParseVector<ASTAsItem*> empty;
    empty.clear();
    $$ = new ASTMachine($2,
                        $4, $5,
                        "", empty,
                        $7);
    empty.release();
    $7.release();
}
;

sockets
: quantity TIDENT TKW_SOCKET
{
    $$.clear();
    $$.push_back(pair<ASTExpression*,string>($1, $2));
}
| sockets quantity TIDENT TKW_SOCKET
{
    $$ = $1;
    $$.push_back(pair<ASTExpression*,string>($2, $3));
}
;

optionalproperties
: /*blank*/ { $$.clear(); }
| properties { $$ = $1; }
;

properties
: TIDENT quantity   { $$.clear(); $$.push_back(new ASTMachProperty($1, $2)); }
| properties TIDENT quantity   { $$=$1; $$.push_back(new ASTMachProperty($2, $3)); }
;

optionalconflicts
: /*blank*/ { $$.clear(); }
| conflicts { $$ = $1; }
;

conflicts
: TKW_CONFLICT TIDENT TCOMMA TIDENT
{
    $$.clear();
    $$.push_back(pair<string,string>($2, $4));
}
| conflicts TKW_CONFLICT TIDENT TCOMMA TIDENT
{
    $$ = $1;
    $$.push_back(pair<string,string>($3, $5));
}
;

optionalresources
: /*blank*/ { $$.clear(); }
| resources { $$ = $1; }
;

resources
: resource { $$.clear(); $$.push_back($1); }
| resources resource { $$ = $1; $$.push_back($2); }
;

resource
: TKW_RESOURCE TIDENT TLPAREN TIDENT TRPAREN quantity TKW_WITH traits optionalpower
{
    $$ = new ASTMachResource($2, $4, $6, $8, $9);
    $8.release();
}
| TKW_RESOURCE TIDENT  TLPAREN TIDENT TRPAREN quantity optionalpower
{
    ParseVector<ASTTrait*> empty;
    empty.clear();
    $$ = new ASTMachResource($2, $4, $6, empty, $7);
    empty.release();
}
;

traits
: TIDENT quantity
{
    $$.clear();
    $$.push_back(new ASTTrait($1, $2));
}
| TIDENT TLPAREN TIDENT TRPAREN quantity
{
    $$.clear();
    $$.push_back(new ASTTrait($1, $3, $5));
}
| traits TCOMMA TIDENT quantity
{
    $$ = $1;
    $$.push_back(new ASTTrait($3, $4));
}
| traits TCOMMA TIDENT TLPAREN TIDENT TRPAREN quantity
{
    $$ = $1;
    $$.push_back(new ASTTrait($3, $5, $7));
}
;

optionalpower
: /*blank*/ { $$ = new ASTMachPower(NULL, NULL); }
| TKW_STATIC TKW_POWER quantity { $$ = new ASTMachPower($3, NULL); }
| TKW_DYNAMIC TKW_POWER quantity { $$ = new ASTMachPower(NULL, $3); }
| TKW_STATIC TKW_POWER quantity TKW_DYNAMIC TKW_POWER quantity { $$ = new ASTMachPower($3, $6); }
| TKW_DYNAMIC TKW_POWER quantity TKW_STATIC TKW_POWER quantity { $$ = new ASTMachPower($6, $3); }
;


 /* ---------------------- */

///\todo: allow models with no statements and no kernels, or
/// possibly intermixed statements and kernels
app
: TKW_MODEL TIDENT TLBRACE globalstatements kernels TRBRACE
{
    $$ = new ASTAppModel($2, $4, $5);
    $4.release();
    $5.release();
}
;

kernels
: kernel { $$.clear(); $$.push_back($1); }
| kernels kernel { $$ = $1; $$.push_back($2); }
;

kernel
: TKW_KERNEL TIDENT TLBRACE kernelstatements TRBRACE
{
    ParseVector<ASTVarDecl*> empty;
    empty.clear();
    $$ = new ASTKernel($2, $4, empty);
    $4.release();
    empty.release();
}
| TKW_KERNEL TIDENT TLPAREN vardecllist TRPAREN TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTKernel($2, $7, $4);
    $4.release();
    $7.release();
}
;

ident : TIDENT                        { $$ = new ASTIdentifier($1); }
      ;

value : TINT                             { $$ = new ASTInteger(atol($1.c_str())); }
      | TREAL                            { $$ = new ASTReal(atof($1.c_str())); }
      ;

constant
: TKW_10POWER
{
    double v = 1;
    if ($1 == "nano")  v = 1e-9;
    if ($1 == "micro") v = 1e-6;
    if ($1 == "milli") v = 1e-3;
    if ($1 == "kilo")  v = 1e3;
    if ($1 == "mega")  v = 1e6;
    if ($1 == "giga")  v = 1e9;
    if ($1 == "tera")  v = 1e12;
    if ($1 == "peta")  v = 1e15;
    if ($1 == "exa")   v = 1e18;
    $$ = new ASTReal(v);
}
;

//
// Lists of statements of various types
//

globalstatements
: globalstmt { $$.clear(); $$.push_back($1); }
| globalstatements globalstmt { $$=$1; $$.push_back($2); }
;

kernelstatements
: kernelstmt { $$.clear(); $$.push_back($1); }
| kernelstatements kernelstmt { $$=$1; $$.push_back($2); }
;

execstatements
: execstmt { $$.clear(); $$.push_back($1); }
| execstatements execstmt { $$=$1; $$.push_back($2); }
;

//
// Define each section of statements
//

globalstmt
: importstmt { $$ = $1; }
| paramstmt { $$ = $1; }
| datastmt { $$ = $1; }
;

kernelstmt
///\todo: not the cleanest implementation to have four versions.
: TKW_EXECUTE TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock("", NULL, $3);
    $3.release();
}
| TKW_EXECUTE quantity TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock("", $2, $4);
    $4.release();
}
| TKW_EXECUTE TIDENT TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock($2, NULL, $4);
    $4.release();
}
| TKW_EXECUTE TIDENT quantity TLBRACE execstatements TRBRACE
{
    $$ = new ASTExecutionBlock($2, $3, $5);
    $5.release();
}
| controlstmt { $$ =  $1; }
///\todo: we eventually may want params inside kernels:
//| paramstmt { $$ = $1; }
;

execstmt
: TIDENT quantity tofrom as
{
    $$ = new ASTRequiresStatement($1, $2, NULL, $3, $4);
    $4.release();
}
| TIDENT quantity TKW_OF TKW_SIZE quantity tofrom as
{
    $$ = new ASTRequiresStatement($1, $2, $5, $6, $7);
    $7.release();
}
;

//
// Define each statement type
//

paramstmt
: TKW_PARAM TIDENT TEQUAL expr
{
    $$ = new ASTAssignStatement($2,$4);
}
| TKW_PARAM TIDENT TEQUAL expr TKW_IN expr TDOTDOT expr
{
    $$ = new ASTAssignStatement($2,
                                new ASTRangeExpression($4, $6, $8));
}
; 

datastmt
: TKW_DATA TIDENT TKW_AS functioncall
{
    $$ = new ASTDataStatement($2,NULL,$4);
}
| TKW_DATA TIDENT quantity
{
    $$ = new ASTDataStatement($2,$3,NULL);
}
;

importstmt
: TKW_IMPORT TIDENT TKW_FROM TSTRING TKW_WITH namedarguments
{
    $$ = new ASTImportStatement($2,$4,$6);
    $6.release();
}
;

kernelcall
: TIDENT
{
    ParseVector<ASTExpression*> empty;
    empty.clear();
    $$ = new ASTControlKernelCallStatement($1, empty);
    empty.release();
}
| TIDENT TLPAREN arguments TRPAREN
{
    $$ = new ASTControlKernelCallStatement($1, $3);
    $3.release();
}
| TIDENT TDOT TIDENT
{
    ParseVector<ASTExpression*> empty;
    empty.clear();
    $$ = new ASTControlKernelCallStatement($1, $3, empty);
    empty.release();
}
| TIDENT TDOT TIDENT TLPAREN arguments TRPAREN
{
    $$ = new ASTControlKernelCallStatement($1, $3, $5);
    $5.release();
}
;

comparison
: comparison TAND comparison { $$ = new ASTComparisonExpression("and",$1,$3); }
| comparison TOR comparison  { $$ = new ASTComparisonExpression("or",$1,$3); }
| expr TCOMPEQ expr  { $$ = new ASTComparisonExpression("==",$1,$3); }
| expr TCOMPNE expr  { $$ = new ASTComparisonExpression("!=",$1,$3); }
| expr TCOMPLT expr  { $$ = new ASTComparisonExpression("<", $1,$3); }
| expr TCOMPGT expr  { $$ = new ASTComparisonExpression(">", $1,$3); }
| expr TCOMPLE expr  { $$ = new ASTComparisonExpression("<=",$1,$3); }
| expr TCOMPGE expr  { $$ = new ASTComparisonExpression(">=",$1,$3); }
;

ifthencontrol
: TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement($6), NULL);
    $6.release();
}
| TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE TKW_ELSE TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement($6), new ASTControlSequentialStatement($10));
    $6.release();
    $10.release();
}
| TKW_IF TLPAREN comparison TRPAREN TLBRACE kernelstatements TRBRACE TKW_ELSE ifthencontrol
{
    $$ = new ASTControlIfThenStatement($3, new ASTControlSequentialStatement($6), $9);
    $6.release();
}
;

probabilityitems
: quantity TLBRACE kernelstatements TRBRACE
{
    $$.clear();
    $$.push_back(new ASTProbabilityItem($1, new ASTControlSequentialStatement($3)));
    $3.release();
}
| probabilityitems quantity TLBRACE kernelstatements TRBRACE
{
    $$ = $1;
    $$.push_back(new ASTProbabilityItem($2, new ASTControlSequentialStatement($4)));
    $4.release();
}
;

probabilitycontrol
: TKW_PROBABILITY TLBRACE probabilityitems TRBRACE
{
    $$ = new ASTControlProbabilityStatement($3, NULL);
    $3.release();
}
| TKW_PROBABILITY TLBRACE probabilityitems TKW_ELSE TLBRACE kernelstatements TRBRACE TRBRACE
{
    $$ = new ASTControlProbabilityStatement($3, new ASTControlSequentialStatement($6));
    $3.release();
}
;

controlstmt
: kernelcall
{
    $$ = $1;
}
| TKW_PAR TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlParallelStatement($3);
    $3.release();
}
| TKW_SEQ TLBRACE kernelstatements TRBRACE
{
    $$ = new ASTControlSequentialStatement($3);
    $3.release();
}
| TKW_ITERATE quantity TLBRACE kernelstatements TRBRACE
{
    if ($4.size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ($4.size() == 1)
        $$ = new ASTControlIterateStatement($2, $4[0]);
    else // size > 1
        $$ = new ASTControlIterateStatement($2, new ASTControlSequentialStatement($4));
    $4.release();
}
| TKW_MAP quantity TLBRACE kernelstatements TRBRACE
{
    if ($4.size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ($4.size() == 1)
        $$ = new ASTControlMapStatement($2, $4[0]);
    else // size > 1
        $$ = new ASTControlMapStatement($2, new ASTControlSequentialStatement($4));
    $4.release();
}
| TKW_MAP TIDENT quantity TLBRACE kernelstatements TRBRACE
{
    if ($5.size() == 0)
        ; // can't happen in current grammar; if we change grammar to allow it, then error here
    else if ($5.size() == 1)
        $$ = new ASTControlMapStatement($3, $5[0]);
    else // size > 1
        $$ = new ASTControlMapStatement($3, new ASTControlSequentialStatement($5));
    $5.release();
    $$->label = $2;
}
| ifthencontrol
{
    $$ = $1;
}
| probabilitycontrol
{
    $$ = $1;
}
;

tofrom
: /*blank*/ { $$ = ""; }
| TKW_TO TIDENT { $$ = $2; }
| TKW_FROM TIDENT { $$ = $2; }
;

as
: /*blank*/ { $$.clear(); }
| TKW_AS asitemlist { $$ = $2; }
;

asitemlist
: asitem { $$.clear(); $$.push_back($1); }
| asitemlist TCOMMA asitem { $$=$1; $$.push_back($3); }
;

asitem
: TIDENT { $$ = new ASTAsItem($1, NULL); }
| TIDENT TLPAREN expr TRPAREN { $$ = new ASTAsItem($1, $3); }
;

vardecllist
: vardecl { $$.clear(); $$.push_back($1); }
| vardecllist TCOMMA vardecl { $$=$1; $$.push_back($3); }
;

vardecl
: TKW_DATA TIDENT { $$ = new ASTVarDecl("data", $2); }
| TKW_PARAM TIDENT { $$ = new ASTVarDecl("param", $2); }
;

quantity
: TLBRACKET expr TRBRACKET { $$ = $2; }

expr
: value { $$ = $1; }
| constant { $$ = $1; }
| ident { $$ = $1; }
| TLPAREN expr TRPAREN { $$ = $2; }
| expr TPLUS expr { $$ = new ASTBinaryExpression("+", $1, $3); }
| expr TMINUS expr { $$ = new ASTBinaryExpression("-", $1, $3); }
| expr TMUL expr { $$ = new ASTBinaryExpression("*", $1, $3); }
| expr TDIV expr { $$ = new ASTBinaryExpression("/", $1, $3); }
| expr TEXP expr { $$ = new ASTBinaryExpression("^", $1, $3); }
| TMINUS expr %prec NEG { $$ = new ASTUnaryExpression("-", $2); }
| functioncall { $$ = $1; }
;

functioncall
: TIDENT TLPAREN arguments TRPAREN
{
    $$ = new ASTFunctionCall($1, $3);
    $3.release();
}
;

arguments
: /*blank*/ { $$.clear(); }
| expr { $$.clear(); $$.push_back($1); }
| arguments TCOMMA expr { $$=$1; $$.push_back($3); }
;

namedarguments
: TIDENT TEQUAL expr { $$.clear(); $$.push_back(new ASTAssignStatement($1,$3)); }
| namedarguments TCOMMA TIDENT TEQUAL expr { $$=$1; $$.push_back(new ASTAssignStatement($3,$5)); }
;

/* resilience modeling, added by Li Yu in May 2014 */
resilience
: TKW_DATASTR TIDENT TLBRACE resiliencestmts TRBRACE
{
    $$ = new ASTResilienceModel($2, $4);
    $4.release();
}
;

resiliencestmts
: resiliencestmt { $$.clear(); $$.push_back($1); }
| resiliencestmts resiliencestmt { $$=$1; $$.push_back($2); }
;

resiliencestmt
: TKW_DATAINF TIDENT TEQUAL expr
{
    $$ = new ASTResilienceDataStatement($2,$4);
}
;

%%

void yyerror(const char *s)
{
    cerr << "ERROR: " << s << " text=\""<<yytext<<"\" line="<<yylineno<< endl;
    cerr << "(Detailed location:"
        << " file='" << yylloc.filename << "'"
        << ", filepos=" << yylloc.first_filepos << " to " << yylloc.last_filepos
        << ", lines=" << yylloc.first_line << " to " << yylloc.last_line
        << ", columns=" << yylloc.first_column << " to " << yylloc.last_column
        << ")" << endl;
}
