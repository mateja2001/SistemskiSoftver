/* If we want to use other functions, we have to put the relevant
 * header includes here. */
%{
	#include "instrukcije.hpp"
	#include <stdio.h>
	extern int yylex(void);
  extern int yyparse();
  extern FILE *yyin;
	extern void yyerror(const char*);
%}

/* These declare our output file names. */
%output "misc/parser.cpp"
%defines "misc/parser.hpp"

/* This union defines the possible return types of both lexer and
 * parser rules. We'll refer to these later on by the field name */
%union {
	int         num;
	char       *ident;
	struct operand *arg;
}

/* These define the tokens that we use in the lexer.
 * All of these have no meaningful return value. */
%token TOKEN_LEVAZ
%token TOKEN_DESNAZ
%token TOKEN_PLUS
%token TOKEN_ZAPETA
%token TOKEN_KRAJ
%token TOKEN_DOLAR
%token TOKEN_DVOTACKA


/* These are ALSO used in the lexer, but in addition to
 * being tokens, they also have return values associated
 * with them. We name those according to the names we used
 * above, in the %union declaration. So, the TOKEN_NUM
 * rule will return a value of the same type as num, which
 * (in this case) is an int. */
%token <num>   TOKEN_NUM
%token <ident> TOKEN_IDENT
%token <ident> TOKEN_STRING
%token <num> TOKEN_REG
%token <ident> TOKEN_CREG
%token <ident> TOKEN_GLOBAL
%token <ident> TOKEN_EXTERN
%token <ident> TOKEN_SECTION
%token <ident> TOKEN_WORD
%token <ident> TOKEN_SKIP
%token <ident> TOKEN_ASCII
%token <ident> TOKEN_END
%token <ident> TOKEN_HALT
%token <ident> TOKEN_INT
%token <ident> TOKEN_IRET
%token <ident> TOKEN_CALL
%token <ident> TOKEN_RET
%token <ident> TOKEN_JMP
%token <ident> TOKEN_BEQ
%token <ident> TOKEN_BNE
%token <ident> TOKEN_BGT
%token <ident> TOKEN_PUSH
%token <ident> TOKEN_POP
%token <ident> TOKEN_XCHG
%token <ident> TOKEN_ADD
%token <ident> TOKEN_SUB
%token <ident> TOKEN_MUL
%token <ident> TOKEN_DIV
%token <ident> TOKEN_NOT
%token <ident> TOKEN_AND
%token <ident> TOKEN_OR
%token <ident> TOKEN_XOR
%token <ident> TOKEN_SHL
%token <ident> TOKEN_SHR
%token <ident> TOKEN_LD
%token <ident> TOKEN_ST
%token <ident> TOKEN_CSRRD
%token <ident> TOKEN_CSRWR


/* These are non-terminals in our grammar, by which I mean, parser
 * rules down below. Each of these also has a meaningful return type,
 * which is declared in the same way. */
%type <arg> arg;
%type <arg> registar;
%type <arg> cRegistar;
%type <arg> simboli;

%%

/* A program is defined recursively as either empty or an instruction
 * followed by another program. In this case, there's nothing meaningful
 * for us to do or return as an action, so we omit any action after the
 * rules. */
prog:
  | TOKEN_KRAJ prog
  | labela
  | labela TOKEN_KRAJ prog
  | labela line
  | labela line TOKEN_KRAJ prog
  | line
  | line TOKEN_KRAJ prog 
;
line:
  instr
  |direktivaBezListe
  |direktivaSaListomLitSimbKraj
  |direktivaSaListomSimbKraj
;
direktivaSaListomSimbKraj:
  direktivaSaListomSimb simboli {
    dodajTrenutnuInstrukciju();  
  }
;
direktivaSaListomLitSimbKraj:
  direktivaSaListomLitSimb literaliSimboli {
    dodajTrenutnuInstrukciju();  
  }
;
labela:
  TOKEN_IDENT TOKEN_DVOTACKA{
    napraviLabelu($1);
  }
;
instr:
  TOKEN_HALT
    { napraviInstrukciju($1, NULL, Adresiranje::IMMED); }
  | TOKEN_INT
    { napraviInstrukciju($1, NULL, Adresiranje::IMMED); }
  | TOKEN_IRET
    { napraviInstrukciju($1, NULL, Adresiranje::IMMED); }
  | TOKEN_CALL arg 
    { struct operand *first = $2;
      Adresiranje a= first->adr; 
      napraviInstrukciju($1, $2, a); 
    }
  | TOKEN_RET 
    { napraviInstrukciju($1,NULL,Adresiranje::IMMED);}
  | TOKEN_JMP arg 
    { 
      struct operand *first = $2;
      Adresiranje a= first->adr;
      napraviInstrukciju($1,$2,a);
    }
  | TOKEN_BEQ registar TOKEN_ZAPETA registar TOKEN_ZAPETA arg{
    struct operand *first= $2;
    first->next=$4;
    first->next->next=$6;
    Adresiranje a= first->next->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_BNE registar TOKEN_ZAPETA registar TOKEN_ZAPETA arg{
    struct operand *first= $2;
    first->next=$4;
    first->next->next=$6;
    Adresiranje a= first->next->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_BGT registar TOKEN_ZAPETA registar TOKEN_ZAPETA arg{
    struct operand *first= $2;
    first->next=$4;
    first->next->next=$6;
    Adresiranje a= first->next->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_PUSH registar{
    struct operand *first=$2;
    Adresiranje a=first->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_POP registar{
    struct operand *first=$2;
    Adresiranje a=first->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_XCHG registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_ADD registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_SUB registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_MUL registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_DIV registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_AND registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_OR registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_XOR registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_SHL registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_SHR registar TOKEN_ZAPETA registar{
    struct operand * first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_NOT registar{
    struct operand * first=$2;
    Adresiranje a=first->adr;
    napraviInstrukciju($1,$2,a);
  }
  |TOKEN_LD arg TOKEN_ZAPETA registar{
    struct operand* first=$2;
    first->next=$4;
    Adresiranje a=first->adr;
    napraviInstrukciju($1,$2,a);
  }
  |TOKEN_ST registar TOKEN_ZAPETA arg{
    struct operand* first=$2;
    first->next=$4;
    Adresiranje a=first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_CSRRD cRegistar TOKEN_ZAPETA registar{
    struct operand* first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
  | TOKEN_CSRWR registar TOKEN_ZAPETA cRegistar{
    struct operand* first=$2;
    first->next=$4;
    Adresiranje a= first->next->adr;
    napraviInstrukciju($1,$2,a);
  }
;
direktivaBezListe:
  TOKEN_END{
    napraviDirektivu($1,NULL);
    YYACCEPT;
  }
  | TOKEN_SKIP TOKEN_NUM{
    struct operand * first=literal($2, Adresiranje::IMMED);
    napraviDirektivu($1,first);
  }
  | TOKEN_SECTION TOKEN_IDENT{
    struct operand *first= simbol($2,Adresiranje::IMMED);
    napraviDirektivu($1,first);
  }
  |TOKEN_ASCII TOKEN_STRING{
    struct operand *first= simbol($2,Adresiranje::IMMED);
    napraviDirektivu($1,first);
  }
;
direktivaSaListomSimb:
  TOKEN_GLOBAL {
    napraviTrenutnuDirektivu($1);
  }
  | TOKEN_EXTERN {
    napraviTrenutnuDirektivu($1);
  }
;
direktivaSaListomLitSimb:
  TOKEN_WORD {
    napraviTrenutnuDirektivu($1);
  }
;
simboli:
  TOKEN_IDENT TOKEN_ZAPETA simboli {
    struct operand* op=simbol($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
  |TOKEN_IDENT{
    /*to je poslednji argument, dodajemo i instrukciju u listu*/
    struct operand* op=simbol($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
;
literaliSimboli:
  TOKEN_IDENT TOKEN_ZAPETA literaliSimboli {
    struct operand* op=simbol($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
  | TOKEN_NUM TOKEN_ZAPETA literaliSimboli{
    struct operand* op=literal($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
  | TOKEN_NUM{
    /*to je poslednji argument, dodajemo i instrukciju u listu*/
    struct operand* op=literal($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
  | TOKEN_IDENT{
    /*to je poslednji argument, dodajemo i instrukciju u listu*/
    struct operand* op=simbol($1,Adresiranje::IMMED);
    dodajOperandTrenInstrukciji(op);
  }
;
registar:
  TOKEN_REG {
    $$=registar($1,Adresiranje::REGDIR);
  }
;
cRegistar:
  TOKEN_CREG {
    $$=cRegistar($1, Adresiranje::REGDIR);
  }
;
arg:
  TOKEN_LEVAZ TOKEN_REG TOKEN_PLUS TOKEN_NUM TOKEN_DESNAZ {
    $$=registarPomLit($2,$4);
  }
  |TOKEN_LEVAZ TOKEN_REG TOKEN_PLUS TOKEN_IDENT TOKEN_DESNAZ {
    $$=registarPomSim($2,$4);
  }
  |TOKEN_LEVAZ TOKEN_REG TOKEN_DESNAZ {
    $$=registar($2, Adresiranje::REGIND);
  }
  | TOKEN_REG
    { $$ = registar($1,Adresiranje::REGDIR); }
  | TOKEN_DOLAR TOKEN_IDENT {
    $$= simbol($2,Adresiranje::IMMED);
  }
  | TOKEN_DOLAR TOKEN_NUM {
    $$= literal($2, Adresiranje::IMMED);
  }
  |TOKEN_NUM {
    $$=literal($1, Adresiranje::MEMDIR);
  }
  |TOKEN_IDENT {
    $$=simbol($1, Adresiranje::MEMDIR);
  }
;

%%