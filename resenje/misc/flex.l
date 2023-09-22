/* this will include some C code directly, so we can pull in the
 * header files we want */
%{
#include <iostream>
#include <string>
using namespace std;
#include "parser.hpp"
#include "instrukcije.hpp"
%}

/* this tells us that we're going to output our lexer implementation
 * to `lexer.c` and the associated header to `lexer.h`.
 */
%option outfile="misc/lexer.cpp" header-file="misc/lexer.hpp"

/* the int yywrap(void) is used internally by the lexer for the case
 *  where you want to parse multiple files in sequence with the same
 * parser state. Usually, you'd supply your own implementation that
 * would set up the next input stream and let the lexer know whether
 * to keep going. In our case, we disable it; this is effectively
 * the same as a dummy implementation like #define yywrap() 0.
 */
%option noyywrap

/* these define a few literals to use later on */
DOLAR "$"
DVOTACKA ":"
PLUS  "+"
ZAPETA ","
KRAJ "\n"
LEVAZ "["
DESNAZ "]"


LITERALHEX   0[Xx][0-9A-Fa-f]+
LITERALOCT 0[0-7]+
LITERALBIN 0[Bb][01]+
LITERALDEC [1-9][0-9]*
IDENT [A-Za-z_][A-Za-z0-9_]*
DIREKTIVA \.[A-Za-z]+
COM   "#"[^\n\r]*
WS    [ \r\t]*
REG %r(1[0-5]|[0-9])
STRING \".+\"


%%

{WS}        { /*preskoci beline*/ }
{COM}       { /*preskoci komentare*/ }
{LITERALHEX}		{
	          sscanf(yytext, "%x", &yylval.num);
			  return TOKEN_NUM;
            }
{LITERALOCT} {
	          sscanf(yytext, "%o", &yylval.num);
			  return TOKEN_NUM;
}
{LITERALBIN} {
  char *c=strdup(yytext);
  yylval.num=stoi(c+2,NULL,2);
  return TOKEN_NUM;
}
{LITERALDEC} {
 sscanf(yytext, "%d", &yylval.num);
			  return TOKEN_NUM;
}
{REG} {
  char *c=strdup(yytext);
  sscanf(c+2,"%d",&yylval.num);
  return TOKEN_REG;
}
{DIREKTIVA} {
  yylval.ident = strdup(yytext);
  if(strcmp(yylval.ident,".global")==0){
    return TOKEN_GLOBAL;
  }else if (strcmp(yylval.ident,".extern")==0){
    return TOKEN_EXTERN;
  }else if (strcmp(yylval.ident,".section")==0){
    return TOKEN_SECTION;
  }else if (strcmp(yylval.ident,".word")==0){
    return TOKEN_WORD;
  }else if (strcmp(yylval.ident,".skip")==0){
    return TOKEN_SKIP;
  }else if (strcmp(yylval.ident,".ascii")==0){
    return TOKEN_ASCII;
  }else if (strcmp(yylval.ident,".end")==0){
    return TOKEN_END;
  }else{

  }
}
{STRING} {
  yylval.ident=strdup(yytext);
  return TOKEN_STRING;
}
"%status" {
  yylval.ident = strdup(yytext+1);
  return TOKEN_CREG;
}
"%handler" {
  yylval.ident = strdup(yytext+1);
  return TOKEN_CREG;
}
"%cause" {
  yylval.ident = strdup(yytext+1);
  return TOKEN_CREG;
}
"%sp" {
  yylval.num=14;
  return TOKEN_REG;
}
"%pc" {
  yylval.num=14;
  return TOKEN_REG;
}
{IDENT}     {
	          yylval.ident = strdup(yytext);
            if(strcmp(yylval.ident,"halt")==0){
              return TOKEN_HALT;
            }else if(strcmp(yylval.ident,"int")==0){
              return TOKEN_INT;
            }else if(strcmp(yylval.ident,"iret")==0){
              return TOKEN_IRET;
            }else if(strcmp(yylval.ident,"call")==0){
              return TOKEN_CALL;
            }else if(strcmp(yylval.ident,"ret")==0){
              return TOKEN_RET;
            }else if(strcmp(yylval.ident,"jmp")==0){
              return TOKEN_JMP;
            }else if(strcmp(yylval.ident,"beq")==0){
              return TOKEN_BEQ;
            }else if(strcmp(yylval.ident,"bne")==0){
              return TOKEN_BNE;
            }else if(strcmp(yylval.ident,"bgt")==0){
              return TOKEN_BGT;
            }else if(strcmp(yylval.ident,"push")==0){
              return TOKEN_PUSH;
            }else if(strcmp(yylval.ident,"pop")==0){
              return TOKEN_POP;
            }else if(strcmp(yylval.ident,"xchg")==0){
              return TOKEN_XCHG;
            }else if(strcmp(yylval.ident,"add")==0){
              return TOKEN_ADD;
            }else if(strcmp(yylval.ident,"sub")==0){
              return TOKEN_SUB;
            }else if(strcmp(yylval.ident,"mul")==0){
              return TOKEN_MUL;
            }else if(strcmp(yylval.ident,"div")==0){
              return TOKEN_DIV;
            }else if(strcmp(yylval.ident,"not")==0){
              return TOKEN_NOT;
            }else if(strcmp(yylval.ident,"and")==0){
              return TOKEN_AND;
            }else if(strcmp(yylval.ident,"or")==0){
              return TOKEN_OR;
            }else if(strcmp(yylval.ident,"xor")==0){
              return TOKEN_XOR;
            }else if(strcmp(yylval.ident,"shl")==0){
              return TOKEN_SHL;
            }else if(strcmp(yylval.ident,"shr")==0){
              return TOKEN_SHR;
            }else if(strcmp(yylval.ident,"ld")==0){
              return TOKEN_LD;
            }else if(strcmp(yylval.ident,"st")==0){
              return TOKEN_ST;
            }else if(strcmp(yylval.ident,"csrrd")==0){
              return TOKEN_CSRRD;
            }else if(strcmp(yylval.ident,"csrwr")==0){
              return TOKEN_CSRWR;
            }else{
              return TOKEN_IDENT;
            }
			      
            }
"0" {
  sscanf(yytext, "%d", &yylval.num);
			  return TOKEN_NUM;
}
{LEVAZ}      { return TOKEN_LEVAZ; }
{DESNAZ}      { return TOKEN_DESNAZ; }
{PLUS}      { return TOKEN_PLUS; }
{ZAPETA}      { return TOKEN_ZAPETA; }
{DOLAR} {return TOKEN_DOLAR;}
{KRAJ} {return TOKEN_KRAJ;}
{DVOTACKA} {return TOKEN_DVOTACKA;}

%%

/* In the event of some kind of error, this will get called. */
int
yyerror(const char* msg)
{
    fprintf(stderr, "lexer error: %s\n", msg);
    return 0;
}