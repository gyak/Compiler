/****************************************************/
/* File: main.c                                     */
/* Main program for TINY compiler                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
// CHANGED TO 'TRUE'
#define NO_PARSE FALSE
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
// CHANGED SOME TO 'TRUE'
int EchoSource = FALSE;
int TraceScan = FALSE;
int TraceParse = TRUE;
int TraceAnalyze = FALSE;
int TraceCode = FALSE;

int Error = FALSE;

int main(int argc, char * argv[])
{
	TreeNode * syntaxTree;
	char pgm[120]; 
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	strcpy(pgm, argv[1]);
	if (strchr(pgm, '.') == NULL)
		strcat(pgm, ".tny");
	source = fopen(pgm, "r");
	if (source == NULL)
	{
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}
	char temp[120];
	for(int i=0; i<strlen(pgm); i++){
		temp[i] = pgm[i];
		if(pgm[i] == '.'){
			temp[i] = '\0';
			break;
		}
	}
	strcat(temp, "_20181623.txt");
	listing = fopen(temp, "w");
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);
#if NO_PARSE
	while (getToken() != ENDFILE);
#else
	syntaxTree = parse();
	if (TraceParse) {
		fprintf(listing, "\nSyntax tree:\n");
		printTree(syntaxTree);
	}
#endif
	fclose(source);
	return 0;
}
