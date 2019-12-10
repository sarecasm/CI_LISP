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
    HYPOT_OPER,
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
    SYMBOL_NODE_TYPE
} AST_NODE_TYPE;

// Types of numeric values
typedef enum {
    INT_TYPE,
    DOUBLE_TYPE
} NUM_TYPE;

// Node to store a number.
typedef struct {
    NUM_TYPE type;
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
    struct ast_node *name;
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
    } data;
    struct ast_node *next;
} AST_NODE;


AST_NODE *createNumberNode(double value, NUM_TYPE type);

// AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2);
AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList); // Task 5

AST_NODE *createSymbolNode(char *symbolTableNode);
AST_NODE *createSymbolTableNode(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr);
AST_NODE *s_expr_list(AST_NODE *s_expr, AST_NODE *s_expr_list);
SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *list, SYMBOL_TABLE_NODE *elem); // let_list
SYMBOL_TABLE_NODE *createSymbol(NUM_TYPE type, char *symbol, AST_NODE *s_expr); // let_elem

void freeNode(AST_NODE *node);

RET_VAL eval(AST_NODE *node);
RET_VAL evalNumNode(NUM_AST_NODE *numNode);
RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode);
RET_VAL evalSymbolNode(AST_NODE *symbolNode);
RET_VAL printFunction(FUNC_AST_NODE *print, RET_VAL result);
RET_VAL evalUni (FUNC_AST_NODE *evalUni); // Task 5
RET_VAL evalBi (FUNC_AST_NODE *evalBi);


RET_VAL negFunction(FUNC_AST_NODE *negNode, RET_VAL op1); // Task 5
RET_VAL absFunction(FUNC_AST_NODE *absNode, RET_VAL op1);
RET_VAL expFunction(FUNC_AST_NODE *expNode, RET_VAL op1);
RET_VAL sqrtFunction(FUNC_AST_NODE *sqrtNode, RET_VAL op1);
RET_VAL addFunction(FUNC_AST_NODE *addNode, RET_VAL op1);
RET_VAL subFunction(FUNC_AST_NODE *subNode, RET_VAL op1, RET_VAL op2);
RET_VAL multFunction(FUNC_AST_NODE *multNode, RET_VAL op1);
RET_VAL divFunction(FUNC_AST_NODE *divNode, RET_VAL op1, RET_VAL op2);
RET_VAL remainderFunction(FUNC_AST_NODE *remainderNode, RET_VAL op1, RET_VAL op2);
RET_VAL logFunction(FUNC_AST_NODE *logNode, RET_VAL op1);
RET_VAL powFunction(FUNC_AST_NODE *powNode, RET_VAL op1, RET_VAL op2);
RET_VAL maxFunction(FUNC_AST_NODE *maxNode, RET_VAL op1, RET_VAL op2);
RET_VAL minFunction(FUNC_AST_NODE *minNode, RET_VAL op1, RET_VAL op2);
RET_VAL exp2Function(FUNC_AST_NODE *exp2Node, RET_VAL op1);
RET_VAL cbrtFunction(FUNC_AST_NODE *cbrtNode, RET_VAL op1);
RET_VAL hypotFunction(FUNC_AST_NODE *hypotNode, RET_VAL op1, RET_VAL op2);

void printRetVal(RET_VAL val);

#endif