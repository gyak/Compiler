/****************************************************/
/* File: tiny.l                                     */
/* Lex specification for TINY                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#include "globals.h"
#include "util.h"
#include "scan.h"
/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN+1];
%}

digit       [0-9]
number      {digit}+
letter      [a-zA-Z]
identifier  {letter}+
newline     \n
whitespace  [ \t]+

%%

"if"            {return IF;}
"else"          {return ELSE;}
"return"        {return RETURN;}
"while"         {return WHILE;}
"int"           {return INT;}
"void"          {return VOID;}
"+"             {return PLUS;}
"-"             {return MINUS;}
"*"             {return TIMES;}
"<="            {return LE;}
"<"             {return LT;}
">="            {return GE;}
">"             {return GT;}
"=="            {return EQ;}
"!="            {return NE;}
"="             {return ASSIGN;}
";"             {return SEMI;}
","             {return COMMA;}
"("             {return LPAREN;}
")"             {return RPAREN;}
"{"             {return LMIDPAREN;}
"}"             {return RMIDPAREN;}
"["             {return LBIGPAREN;}
"]"             {return RBIGPAREN;}
{number}        {return NUM;}
{identifier}    {return ID;}
{newline}       {lineno++;}
{whitespace}    {/* skip whitespace */}
"/*"            { char c;
                    while(1){
                        c = input();
                        if(c == EOF)
                            return ERROR;
                        else if(c == '*'){
                            c = input();
                            if(c == '/')
                                break;
                            else if(c == EOF)
                                return ERROR;
                            else if(c == '\n')
                                lineno++;
                        }
                        else if(c == '\n')
                            lineno++;
                    }
                    
                }
"/"             {return OVER;}
.               {return ERROR;}

%%

TokenType getToken(void)
{ 
    //printf("tiny.l\n");
    static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { 
      firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  //printf("yytext: %s\n", yytext);
  currentToken = yylex();
  /*if(yytext == NULL)
      printf("yytext: NULL\n");
  else if(yytext[0] == '\n')
      printf("yytext: NEWLINE\n");
  else if(yytext[0] == ' ')
      printf("yytext: SPACE\n");
  else if(yytext[0] == '\t')
      printf("yytext: TAB\n");
  else
    printf("yytext: |%s|%d|\n", yytext, strlen(yytext));*/
  strncpy(tokenString,yytext,MAXTOKENLEN);
  if (TraceScan) {
    fprintf(listing,"\t%d: ",lineno);
    printToken(currentToken,tokenString);
  }
  return currentToken;
}

        //    fprintf(listing, ",%s\n", tokenString);
         //   break;
