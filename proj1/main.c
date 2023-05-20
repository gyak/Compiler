/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
#define NO_PARSE TRUE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
#define NO_ANALYZE TRUE

/* set NO_CODE to TRUE to get a compiler that does not
 * generate code
 */
#define NO_CODE TRUE

#include "util.h"
#if NO_PARSE
#include "scan.h"
#else
#include "parse.h"
#if !NO_ANALYZE
#include "analyze.h"
#if !NO_CODE
#include "cgen.h"
#endif
#endif
#endif

/* allocate global variables */
int lineno = 0;
FILE * source;
FILE * listing;
FILE * code;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = FALSE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = TRUE;

int main( int argc, char * argv[] )
{ 
    TreeNode * syntaxTree;
    char pgm[120] = "hw1.c"; /* source code file name */
    //if (argc != 2){ 
    //    fprintf(stderr,"usage: %s <filename>\n",argv[0]);
    //    exit(1);
    //}
    //strcpy(pgm,argv[1]) ;
    if (strchr (pgm, '.') == NULL)
        strcat(pgm,".c");
    source = fopen(pgm,"r");
    if (source==NULL){ 
        fprintf(stderr,"File %s not found\n",pgm);
        exit(-1);
    }
    listing = stdout; /* send listing to screen */
    listing = fopen("hw1_20181623.txt","w");
    fprintf(listing,"\nTINY COMPILATION: %s\n",pgm);
    fprintf(listing,"\tline number\t\ttoken\t\tlexeme\n");
    fprintf(listing,"==========================================================\n");
    //printf("11111\n");
#if NO_PARSE
    //printf("222\n");
    while (getToken()!=ENDFILE)
        ;
#else
    //printf("3333\n");
    syntaxTree = parse();
    if (TraceParse) {
        fprintf(listing,"\nSyntax tree:\n");
        printTree(syntaxTree);
    }
#if !NO_ANALYZE
    //printf("44444\n");
    if (! Error){ 
        if (TraceAnalyze) 
            fprintf(listing,"\nBuilding Symbol Table...\n");
        buildSymtab(syntaxTree);
        if (TraceAnalyze) 
            fprintf(listing,"\nChecking Types...\n");
        typeCheck(syntaxTree);
        if (TraceAnalyze) 
            fprintf(listing,"\nType Checking Finished\n");
    }
#if !NO_CODE
    //printf("5555\n");
    if (! Error){ 
        char * codefile;
        int fnlen = strcspn(pgm,".");
        codefile = (char *) calloc(fnlen+4, sizeof(char));
        strncpy(codefile,pgm,fnlen);
        strcat(codefile,".tm");
        code = fopen(codefile,"w");
        if (code == NULL){ 
            printf("Unable to open %s\n",codefile);
            exit(1);
        }
        codeGen(syntaxTree,codefile);
        fclose(code);
    }
#endif
#endif
#endif
    //printf("666\n");
    fclose(source);
    return 0;
}

