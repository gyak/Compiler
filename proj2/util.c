/****************************************************/
/* File: util.c                                     */
/* Utility function implementation                  */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"

/* Procedure printToken prints a token
 * and its lexeme to the listing file
 */
void printToken(TokenType token, const char* tokenString)
{
    switch (token)
    {
        case IF:
            break;
        case WHILE:
            break;
        case RETURN:
            break;
        case INT:
            break;
        case VOID:
            break;
        case ELSE:
            break;
        case ASSIGN: 
            fprintf(listing, "=\n"); 
            break;
        case EQ: 
            fprintf(listing, "==\n"); 
            break;
        case NE: 
            fprintf(listing, "!=\n"); 
            break;
        case LT: 
            fprintf(listing, "<\n"); 
            break;
        case LE: 
            fprintf(listing, "<=\n"); 
            break;
        case GT: 
            fprintf(listing, ">\n"); 
            break;
        case GE: 
            fprintf(listing, ">=\n"); 
            break;
        case LPAREN: 
            fprintf(listing, "(\n"); 
            break;
        case RPAREN: 
            fprintf(listing, ")\n"); 
            break;
        case LBRACE: 
            fprintf(listing, "[\n"); 
            break;
        case RBRACE: 
            fprintf(listing, "]\n"); 
            break;
        case LCURLY: 
            fprintf(listing, "{\n"); 
            break;
        case RCURLY: 
            fprintf(listing, "}\n"); 
            break;
        case SEMI: 
            fprintf(listing, ";\n"); 
            break;
        case COMMA: 
            fprintf(listing, ",\n"); 
            break;
        case PLUS: 
            fprintf(listing, "+\n"); 
            break;
        case MINUS: 
            fprintf(listing, "-\n"); 
            break;
        case TIMES: 
            fprintf(listing, "*\n");
            break;
        case OVER: 
            fprintf(listing, "/\n"); 
            break;
        case ENDFILE: 
            fprintf(listing, "EOF\n"); 
            break;
        case NUM: 
            fprintf(listing, "NUM, val= %s\n", tokenString); 
            break;
        case ID: 
            fprintf(listing, "ID, name= %s\n", tokenString); 
            break;
        case ERROR: 
            fprintf(listing, "ERROR: %s\n", tokenString); 
            break;
        default: 
            fprintf(listing, "Unknown token: %d\n", token);
    }
}

/* Function newStmtNode creates a new statement
 * node for syntax tree construction
 */
TreeNode * newStmtNode(StmtKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    if (t == NULL){
        fprintf(listing, "Out of memory error at line %d\n", lineno);
        exit(-1);
    }
    else {
        t->nodekind = StmtK;
        t->kind.stmt = kind;
        for (int i = 0; i < MAXCHILDREN; i++) 
            t->child[i] = NULL;
        t->sibling = NULL;
        t->lineno = lineno;
    }
    return t;
}

/* Function newExpNode creates a new expression
 * node for syntax tree construction
 */
TreeNode * newExpNode(ExpKind kind)
{
    TreeNode * t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL){
        fprintf(listing, "Out of memory error at line %d\n", lineno);
        exit(-1);
    }
    else {
        t->nodekind = ExpK;
        t->kind.exp = kind;
        for (i = 0; i < MAXCHILDREN; i++) 
            t->child[i] = NULL;
        t->sibling = NULL;
        t->lineno = lineno;
        t->type = Void;
        t->isParam = FALSE;
    }
    return t;
}

/* Function copyString allocates and makes a new
 * copy of an existing string
 */
char * copyString(char * s)
{
    if (s == NULL) 
        return NULL;
    char* t = (char*)malloc(strlen(s) + 1);
    if (t == NULL){
        fprintf(listing, "Out of memory error at line %d\n", lineno);
        exit(-1);
    }
    else 
        strcpy(t, s);
    return t;
}

/* Variable indentno is used by printTree to
 * store current number of spaces to indent
 */
static int indentno = 0;

/* macros to increase/decrease indentation */
#define INDENT indentno+=2
#define UNINDENT indentno-=2

/* printSpaces indents by printing spaces */
static void printSpaces(void)
{
    for (int i = 0; i<indentno;i++)
        fprintf(listing, " ");
}

/* procedure printTree prints a syntax tree to the
 * listing file using indentation to indicate subtrees
 */
void printTree(TreeNode * tree)
{
    INDENT;
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == StmtK){
            switch (tree->kind.stmt) {
                case CompoundStmtK:
                    fprintf(listing, "Compound Statement\n");
                    break;
                case SelectionStmtK:
                    fprintf(listing, "If\n");
                    break;
                case IterationStmtK:
                    fprintf(listing, "While\n");
                    break;
                case ReturnStmtK:
                    fprintf(listing, "Return Statement\n");
                    break;
                case SimpleStmtK:
                    fprintf(listing, "Simple Expression\n");
                    break;
                case AdditiveStmtK:
                    fprintf(listing, "Additive Expression\n");
                    break;
                case TermK:
                    fprintf(listing, "Term\n");
                    break;
                case CallK:
                    {
                        int k = 0;
                        TreeNode* temp = tree->child[0];
                        while(temp != NULL){
                            k++;
                            temp = temp->sibling;
                        }    
                        fprintf(listing, "Function Call : %s\n", tree->attr.name);
                        printSpaces();
                        fprintf(listing,"Numbers of Arguments : %d\n", k);
                    }
                    break;
                default:
                    fprintf(listing, "Unknown ExpNode kind\n");
                    break;
            }
        }
        else if (tree->nodekind == ExpK){
            switch (tree->kind.exp) {
                case VarDeclK:
                    if(tree->isParam==TRUE){
                        if(tree->attr.name != NULL){
                            fprintf(listing, "Parameter : %s\n",tree->attr.name);
                            printSpaces();
                            switch(tree->type){
                                case Integer:
                                    fprintf(listing, "Type : int\n");
                                    break;
                                case Void:
                                    fprintf(listing, "Type : void\n");
                                    break;
                            }
                        }
                    }
                    else{
                        fprintf(listing, "Variable Declare : %s\n",tree->attr.name);
                        printSpaces();
                        switch(tree->type){
                            case Integer:
                                fprintf(listing, "Type : int\n");
                                break;
                            case Void:
                                fprintf(listing, "Type : void\n");
                                break;
                        }
                    }
                    break;
                case ArrayDeclK:
                    if(tree->isParam==TRUE){
                        fprintf(listing, "Parameter(Array) : %s\n",tree->attr.name);
                        printSpaces();
                        switch(tree->type){
                            case Integer:
                                fprintf(listing, "Type : int\n");
                                break;
                            case Void:
                                fprintf(listing, "Type : void\n");
                                break;
                        }
                    }
                    else{
                        fprintf(listing, "Array Declare : %s\n",tree->attr.name);
                        printSpaces();
                        switch(tree->type){
                            case Integer:
                                fprintf(listing, "Type : int\n");
                                break;
                            case Void:
                                fprintf(listing, "Type : void\n");
                                break;
                        }
                        printSpaces();
                        fprintf(listing,"Size : %d\n", tree->size);
                    }
                    break;
                case FuncDeclK:
                    fprintf(listing, "Function Declare : %s\n",tree->attr.name);
                    printSpaces();
                    switch(tree->type){
                        case Integer:
                            fprintf(listing, "Type : int\n");
                            break;
                        case Void:
                            fprintf(listing, "Type : void\n");
                            break;
                    }
                    break;
                case AssignK:
                    fprintf(listing, "Assign : = \n");
                    break;
                case OpK:
                    fprintf(listing, "Operator : ");
                    printToken(tree->attr.op, "\0");
                    break;
                case IdK:
                    fprintf(listing, "Variable : %s\n",tree->attr.name);
                    break;
                case ConstK:
                    fprintf(listing, "Constant : %d\n",tree->attr.val);
                    break;
                default:
                    fprintf(listing, "Unknown ExpNode kind\n");
                    break;
            }
        }
        else 
            fprintf(listing, "Unknown node kind\n");
        for (int i = 0;i<MAXCHILDREN;i++)
            printTree(tree->child[i]);
        tree = tree->sibling;
    }
    UNINDENT;
}
