#ifndef __cilisp_h_
#define __cilisp_h_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "ciLispParser.h"

int yyparse(void);

int yylex(void);

void yyerror(char *);

// Enum of all operators.
// must be in sync with funcs in resolveFunc()
typedef enum oper {
    NEG_OPER, // 0
    ABS_OPER,
    EXP_OPER,
    SQRT_OPER,
    ADD_OPER,
    SUB_OPER,
    MULT_OPER,
    DIV_OPER,
    REMAINDER_OPER,
    LOG_OPER,
    POW_OPER,
    MAX_OPER,
    MIN_OPER,
    EXP2_OPER,
    CBRT_OPER,
    HYPOT_OPER, //eval func until here
    READ_OPER,
    RAND_OPER,
    PRINT_OPER,
    EQUAL_OPER,
    LESS_OPER,
    GREATER_OPER,
    CUSTOM_OPER =255
} OPER_TYPE;

OPER_TYPE resolveFunc(char *);

// Types of Abstract Syntax Tree nodes.
// Initially, there are only numbers and functions.
// You will expand this enum as you build the project.
typedef enum {
    NUM_NODE_TYPE,
    FUNC_NODE_TYPE,
    SYMBOL_NODE_TYPE,
    COND_NODE_TYPE,
} AST_NODE_TYPE;

// Types of numeric values
typedef enum {
    INT_TYPE,
    DOUBLE_TYPE,
    NO_TYPE
} NUM_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
//    union{
//        double dval;
//        long ival;
//    } value;
    double value;
} NUM_AST_NODE;

// Values returned by eval function will be numbers with a type.
// They have the same structure as a NUM_AST_NODE.
// The line below allows us to give this struct another name for readability.
typedef NUM_AST_NODE RET_VAL;

// Node to store a function call with its inputs
typedef struct {
    OPER_TYPE oper;
    char* ident; // only needed for custom functions
    struct ast_node *op1;
    struct ast_node *op2;
    struct ast_node *opList;
} FUNC_AST_NODE;

// Generic Abstract Syntax Tree node. Stores the type of node,
// and reference to the corresponding specific node (initially a number or function call).
typedef struct symbol_table_node {
    NUM_TYPE val_type;
    char *ident;
    struct ast_node *val;
    struct symbol_table_node *next;
} SYMBOL_TABLE_NODE;

// TASK 6:
typedef struct {
    struct ast_node *cond;
    struct ast_node *trueCond; // to eval if cond is nonzero
    struct ast_node *falseCond; // to eval if cond is zero
} COND_AST_NODE;

typedef struct symbol_ast_node {
    char *ident;
} SYMBOL_AST_NODE;

typedef struct ast_node {
    AST_NODE_TYPE type;
    SYMBOL_TABLE_NODE *symbolTable;
    struct ast_node *parent;
    union {
        NUM_AST_NODE number;
        FUNC_AST_NODE function;
        SYMBOL_AST_NODE symbol;
        COND_AST_NODE condition; // TASK 6
    } data;
    struct ast_node *next;
} AST_NODE;

AST_NODE *createNumberNode(double value, NUM_TYPE type);
AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList);
AST_NODE *createSymbolNode( char *symbolTableNode ); // SYMBOL
AST_NODE *createSymbolTableNode( SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr );
AST_NODE *createCondNode( AST_NODE *cond, AST_NODE *trueNode, AST_NODE *falseNode );
AST_NODE *s_expr_list( AST_NODE *s_expr, AST_NODE *s_expr_list );

SYMBOL_TABLE_NODE *addSymbolToList( SYMBOL_TABLE_NODE *list, SYMBOL_TABLE_NODE *elem ); // let_list
SYMBOL_TABLE_NODE *createSymbol( NUM_TYPE type, char *symbol, AST_NODE *s_expr ); // let_elem

void printRetVal(RET_VAL val);
void freeNode(AST_NODE *node);
RET_VAL eval(AST_NODE *node);

#endif
