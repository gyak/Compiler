/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode* declaration_list(void);
static TreeNode* declaration(void);
static TreeNode* var_declaration(void);
static ExpType type_specifier(void);
static TreeNode* params(void);
static TreeNode* param_list(ExpType);
static TreeNode* param(ExpType);
static TreeNode* compound_stmt(void);
static TreeNode* local_declaration(void);
static TreeNode* stmt_list(void);
static TreeNode* stmt(void);
static TreeNode* expression_stmt(void);
static TreeNode* selection_stmt(void);
static TreeNode* iteration_stmt(void);
static TreeNode* return_stmt(void);
static TreeNode* expression(void);
static TreeNode* simple_expression(TreeNode*);
static TreeNode* op(TokenType);
static TreeNode* additive_expression(TreeNode*);
static TreeNode* term(TreeNode*);
static TreeNode* factor(TreeNode*);
static TreeNode* call(void);
static TreeNode* args(void);
static TreeNode* args_list(void);


static void syntaxError()
{ 
    fprintf(listing,"\nSyntax error at line %d: syntax error\n",lineno);
    Error = TRUE;
    fprintf(listing,"\nCurrent token: ");
    printToken(token,tokenString);
    fprintf(listing, "\nSyntax tree:\n");
    exit(-1);
}

static void match(TokenType expected)
{ 
    if (token == expected) 
        token = getToken();
    else 
        syntaxError();
      
}

TreeNode * declaration_list(void)
{ 
    TreeNode* ret = declaration();
    TreeNode* temp = ret;
    while (token!=ENDFILE)
    { 
        TreeNode* nd = declaration();
        if (nd != NULL) {
            if(ret != NULL)
                temp->sibling = nd;
            else
                ret = nd;
            temp = nd;
        }
    }
    return ret;
}

TreeNode* declaration(void)
{
    TreeNode* ret;
    ExpType type = type_specifier();
    char* name = copyString(tokenString);
    match(ID);
    switch (token){
        case LPAREN:
            match(LPAREN);
            ret = newExpNode(FuncDeclK);
            ret->attr.name = name;
            ret->type = type;
            ret->child[0] = params();
            match(RPAREN);
            if(ret->child[0] == NULL)
                ret->child[0] = compound_stmt();
            else
                ret->child[1] = compound_stmt();
            break;
        case SEMI:
            match(SEMI);
            ret = newExpNode(VarDeclK);
            ret->attr.name = name;
            ret->type = type;
            break;
        case ENDFILE:
            match(ENDFILE);
            break;
        default: 
            syntaxError();
    }
    return ret;
}

TreeNode* var_declaration(void)
{
    TreeNode* ret;
    ExpType type = type_specifier();
    char* name = copyString(tokenString);
    match(ID);
    switch (token){
        case LBRACE:
            match(LBRACE);
            ret = newExpNode(ArrayDeclK);
            ret->attr.name = name;
            ret->type = type;
            ret->size = atoi(tokenString);
            match(NUM);
            match(RBRACE);
            match(SEMI);
            break;
        case SEMI:
            match(SEMI);
            ret = newExpNode(VarDeclK);
            ret->attr.name = name;
            ret->type = type;
            break;
        case ENDFILE:
            match(ENDFILE);
            break;
        default: 
            syntaxError();
    }
    return ret;
}

ExpType type_specifier(void)
{
    switch (token){
        case INT:
            token = getToken();
            return Integer;
        case VOID:
            token = getToken();
            return Void;
        default: 
            syntaxError();
    }
}

TreeNode * params(void)
{
    ExpType type = type_specifier();
    TreeNode* ret = NULL;
    if(type != Void || token != RPAREN)
        ret = param_list(type);
    return ret;
}

TreeNode * param_list(ExpType type)
{
    TreeNode* ret = param(type);
    TreeNode* temp = ret;
    while (token == COMMA){
        match(COMMA);
        TreeNode* nd = param(type_specifier());
        if (nd != NULL) {     
            if(ret != NULL)
                temp->sibling = nd;
            else
                ret = nd;
            temp = nd;
        }
    }
    return ret;
}

TreeNode * param(ExpType type)
{
    TreeNode* ret;
    char* name = copyString(tokenString);
    match(ID);
    if (token == LBRACE){
        match(LBRACE);
        match(RBRACE);
        ret = newExpNode(ArrayDeclK);
    }
    else
        ret = newExpNode(VarDeclK);
    ret->attr.name = name;
    ret->type = type;
    ret->isParam = TRUE;
    return ret;
}

TreeNode * compound_stmt(void)
{
    TreeNode* ret = newStmtNode(CompoundStmtK);
    match(LCURLY);
    ret->child[0] = local_declaration();
    ret->child[1] = stmt_list();
    match(RCURLY);
    return ret;
}

TreeNode * local_declaration(void)
{
    TreeNode* ret = NULL;
    if (token == INT || token == VOID)
        ret = var_declaration();
    TreeNode* temp = ret;
    if (ret != NULL){
        while (token == INT || token == VOID ){
            TreeNode* nd = var_declaration();
            if (nd != NULL) {
                if(ret != NULL)
                    temp->sibling = nd;
                else
                    ret = nd;
                temp = nd;
            }
        }
    }
    return ret;
}

TreeNode * stmt_list(void)
{
    if (token == RCURLY)
        return NULL;
    TreeNode* ret = stmt();
    TreeNode* temp = ret;
    while (token != RCURLY){
        TreeNode* nd = stmt();
        if (nd != NULL) {
            if(ret != NULL)
                temp->sibling = nd;
            else
                ret = nd;
            temp = nd;
        }
    }

    return ret;
}

TreeNode * stmt(void)
{
    TreeNode *ret = NULL;
    switch (token){
        case IF:
            ret = selection_stmt();
            break;
        case WHILE:
            ret = iteration_stmt();
            break;
        case RETURN:
            ret = return_stmt();
            break;
        case LCURLY:
            ret = compound_stmt();
            break;
        case ID:
            ret = expression_stmt();
            break;
        case LPAREN:
            ret = expression_stmt();
            break;
        case NUM:
            ret = expression_stmt();
            break;
        case SEMI:
            ret = expression_stmt();
            break;
        default: 
            syntaxError();
    }
    return ret;
}

TreeNode * expression_stmt(void)
{
    TreeNode* ret;
    if (token == SEMI)
        match(SEMI);
    else if (token != RCURLY){
        ret = expression();
        match(SEMI);
    }
    return ret;
}

TreeNode * selection_stmt(void)
{
    TreeNode* ret = newStmtNode(SelectionStmtK);
    match(IF);
    match(LPAREN);
    ret->child[0] = expression();
    match(RPAREN);
    ret->child[1] = stmt();
    if (token == ELSE){
        match(ELSE);
        ret->child[2] = stmt();
    }

    return ret;
}

TreeNode * iteration_stmt(void)
{
    TreeNode *ret = newStmtNode(IterationStmtK);
    match(WHILE);
    match(LPAREN);
    ret->child[0] = expression();
    match(RPAREN);
    ret->child[1] = stmt();
    return ret;
}

TreeNode * return_stmt(void)
{
    TreeNode *ret = newStmtNode(ReturnStmtK);
    match(RETURN);
    if (token != SEMI)
        ret->child[0] = expression();
    match(SEMI);
    return ret;
}

TreeNode * expression(void)
{
    TreeNode *ret = NULL, *nd = NULL;
    int flag = FALSE;
    if (token == ID){
        nd = call();
        flag = TRUE;
    }
    if (flag == TRUE && token == ASSIGN){
        if(nd == NULL || nd->nodekind != ExpK || nd->kind.exp != IdK)
            syntaxError();
        match(ASSIGN);
        ret = newExpNode(AssignK);
        ret->child[0] = nd;
        ret->child[1] = expression();
    }
    else
        ret = simple_expression(nd);
    return ret;
}

TreeNode * simple_expression(TreeNode *prev)
{
    TreeNode *ret, *nd = additive_expression(prev);
    TokenType oper = token;
    if (token == LT || token == LE || token == GT || token == GE || token == EQ || token == NE){
        match(token);
        ret = newStmtNode(SimpleStmtK);
        ret->child[0] = nd;
        ret->child[1] = op(oper);
        ret->child[2] = additive_expression(NULL);
    }
    else
        ret = nd;
    return ret;
}

TreeNode* op(TokenType oper)
{
    TreeNode* ret = newExpNode(OpK);
    ret->attr.op = oper;
    return ret;
}

TreeNode * additive_expression(TreeNode *prev)
{
    TreeNode * ret = term(prev);
    if (ret != NULL){
        while (token == PLUS || token==MINUS){
            TreeNode* nd = newStmtNode(AdditiveStmtK);
            nd->child[0] = ret;
            nd->child[1] = op(token);
            ret = nd;
            match(token);
            ret->child[2] = term(NULL);
        }
    }
    return ret;
}

TreeNode * term(TreeNode *prev)
{
    TreeNode* ret = factor(prev);
    if (ret != NULL){
        while (token == TIMES || token == OVER){
            TreeNode* nd = newStmtNode(TermK);
            nd->child[0] = ret;
            nd->child[1] = op(token);
            ret = nd;
            match(token);
            ret->child[2] = factor(NULL);
        }
    }
    return ret;
}

TreeNode * factor(TreeNode *prev)
{
    TreeNode* ret = NULL;
    if (prev != NULL)
        return prev;
    switch (token){
        case ID:
            ret = call();
            break;
        case NUM:
            ret = newExpNode(ConstK);
            ret->attr.val = atoi(tokenString);
            ret->type = Integer;
            match(NUM);
            break;
        case LPAREN:
            match(LPAREN);
            ret = expression();
            match(RPAREN);
            break;
        default: 
            syntaxError();
    }
    return ret;
}

TreeNode * call(void)
{
    TreeNode* ret = NULL;
    char* name = NULL;
    if(token==ID)
        name = copyString(tokenString);
    match(ID);
    if (token == LPAREN){
        match(LPAREN);
        ret = newStmtNode(CallK);
        ret->attr.name = name;
        ret->child[0] = args();
        match(RPAREN);
    }
    else if(token==LBRACE){
        match(LBRACE);
        ret = newExpNode(IdK);
        ret->attr.name = name;
        ret->type = Integer;
        ret->child[0] = expression();
        match(RBRACE);
    }
    else{
        ret = newExpNode(IdK);
        ret->attr.name = name;
        ret->type = Integer;
    }
    return ret;
}

TreeNode * args(void)
{
    TreeNode* ret = NULL;
    if(token != RPAREN)
        ret = args_list();
    return ret;
}

TreeNode * args_list(void)
{
    TreeNode* ret = expression();
    TreeNode* temp = ret;
    while (token == COMMA){
        match(COMMA);
        TreeNode * nd = expression();
        if (nd != NULL) {
            if(ret != NULL)
                temp->sibling = nd;
            else
                ret = nd;
            temp = nd;
        }
    }
    return ret;
}

TreeNode * parse(void)
{ 
    token = getToken();
    TreeNode* ret = declaration_list();
    if (token!=ENDFILE)
        syntaxError();
    return ret;
}
