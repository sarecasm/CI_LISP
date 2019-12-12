#include "ciLisp.h"

#define PRECISION_LOSS_WARNING \
    "WARNING: precision loss in the assignment for variable %s\n"
#define DEFAULT_RET_VAL \
    (RET_VAL){DOUBLE_TYPE, NAN}

NUM_AST_NODE readFunction();
AST_NODE * specialFunctions(char * funcName);

RET_VAL evalNumNode(AST_NODE *node);
RET_VAL evalFuncNode(AST_NODE *node);
RET_VAL evalSymbolNode(AST_NODE *symbolNode);
RET_VAL evalCondNode(AST_NODE *node);

RET_VAL evalUnary(AST_NODE *node);
RET_VAL evalBinary(AST_NODE *node);

RET_VAL negFunction(AST_NODE *node);
RET_VAL absFunction(AST_NODE *node);
RET_VAL expFunction(AST_NODE *node);
RET_VAL sqrtFunction(AST_NODE *node);
RET_VAL logFunction(AST_NODE *node);
RET_VAL exp2Function(AST_NODE * node);
RET_VAL cbrtFunction(AST_NODE * node);
RET_VAL subFunction(AST_NODE * node);
RET_VAL divFunction(AST_NODE * node);
RET_VAL remaindFunction(AST_NODE * node);
RET_VAL powFunction(AST_NODE * node);
RET_VAL maxFunction(AST_NODE * node);
RET_VAL minFunction(AST_NODE * node);
RET_VAL hypotFunction(AST_NODE * node);
RET_VAL equalFunction(AST_NODE * node);
RET_VAL lessFunction(AST_NODE * node);
RET_VAL greaterFunction(AST_NODE * node);

RET_VAL addFunction(AST_NODE * node);
RET_VAL multFunction(AST_NODE * node);
RET_VAL printFunction(AST_NODE * node);

void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

AST_NODE * createNode(AST_NODE_TYPE type) {
    AST_NODE * node = calloc(1, sizeof(AST_NODE));
    if (node == NULL) {
        yyerror("Memory allocation failed!");
        exit(1);
    }
    node->type = type;
    return node;
}


AST_NODE *createNumberNode(double value, NUM_TYPE type) {
    AST_NODE *node = createNode(NUM_NODE_TYPE);
    node->data.number.value = value;
    node->data.number.type = type;
    return node;
}


void setParents(AST_NODE * parent, AST_NODE * opList)
{
    while (opList != NULL) {
        opList->parent = parent;
        opList = opList->next;
    }
}

NUM_AST_NODE readFunction() {
    NUM_AST_NODE result = (NUM_AST_NODE){NO_TYPE, 0.0};
    char * buffer = NULL;
    size_t size = 0;
    getline(&buffer, &size, stdin);
    char * end = buffer;
    result.value = strtod(buffer, &end);
    *end = '\0';
    result.type = (strchr(buffer, '.') == NULL) ? INT_TYPE : DOUBLE_TYPE;
    return result;
}

AST_NODE * specialFunctions(char * funcName)
{
    if (strcmp(funcName, funcNames[RAND_OPER]) == 0) {
        return createNumberNode(rand()/(double)RAND_MAX, INT_TYPE); //NOLINT
    }
    if (strcmp(funcName, funcNames[READ_OPER]) == 0) {
        NUM_AST_NODE num = readFunction();
        return createNumberNode(num.value, num.type);
    }
    return NULL;
}

AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList) {
    AST_NODE * node = specialFunctions(funcName);
    if (node != NULL)
        return node;

    node = createNode(FUNC_NODE_TYPE);
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.opList = opList;
    setParents(node, opList);
    free(funcName);
    return node;
}

// TODO | TASK 2 ... X
AST_NODE *createSymbolNode(char *symbolTableNode) {
    // copied AST_NODE createFunctionNode
    AST_NODE *node = createNode(SYMBOL_NODE_TYPE);
    node->data.symbol.ident = symbolTableNode;
    return node;
}

// TODO | TASK 2 ... X
AST_NODE *createSymbolTableNode(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr) {
    SYMBOL_TABLE_NODE *node = symbolTable;
    s_expr->symbolTable = symbolTable;

    while (node != NULL) {
        node->val->parent = s_expr;
        node = node->next;
    }

    return s_expr;
}

// TODO | TASK 5
AST_NODE *s_expr_list(AST_NODE *s_expr, AST_NODE *s_expr_list) {
    AST_NODE *iter = s_expr;

    while (iter->next != NULL) {
        iter = iter->next;
    }
    iter->next = s_expr_list;

    return s_expr;
}

// TODO | TASK 6
AST_NODE *createCondNode(AST_NODE *cond, AST_NODE *trueNode, AST_NODE *falseNode) {
    AST_NODE *node = createNode(COND_NODE_TYPE);
    cond->parent = node;
    node->data.condition.cond = cond;
    trueNode->parent = node;
    node->data.condition.trueCond = trueNode;
    falseNode->parent = node;
    node->data.condition.falseCond = falseNode;
    return node;
}

// TODO | TASK 2 ... X
SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *list, SYMBOL_TABLE_NODE *elem) // let_list
{
    elem->next = list;
    return elem;
}

// TODO | TASK 2 ... X | TASK 3 ... X
//SYMBOL_TABLE_NODE *createSymbol( char *symbol, AST_NODE *s_expr ) // let_elem
SYMBOL_TABLE_NODE *createSymbol(NUM_TYPE type, char *symbol, AST_NODE *s_expr) // let_elem
{
    // followed AST_NODE createFunctionNode
    SYMBOL_TABLE_NODE *node = calloc(1, sizeof(SYMBOL_TABLE_NODE));
    if (node == NULL) {
        yyerror("Memory allocation failed");
        exit(2);
    }
    node->val_type = type;
    node->ident = symbol;
    node->val = s_expr;
    return node;
}


void freeSymbolTable(SYMBOL_TABLE_NODE * symbolTable)
{
    if (symbolTable == NULL)
        return;
    freeSymbolTable(symbolTable->next);
    free(symbolTable->ident);
    freeNode(symbolTable->val);
    free(symbolTable);
}

void freeNode(AST_NODE *node) {
    if (node == NULL)
        return;

    if (node->type == FUNC_NODE_TYPE) {
        if (node->data.function.ident != NULL)
            free(node->data.function.ident);
        freeNode(node->data.function.opList);
    }
    if (node->type == SYMBOL_NODE_TYPE) {
        free(node->data.symbol.ident);
    }
    if (node->type == COND_NODE_TYPE) {
        freeNode(node->data.condition.cond);
        freeNode(node->data.condition.trueCond);
        freeNode(node->data.condition.falseCond);
    }

    freeSymbolTable(node->symbolTable);
    freeNode(node->next);
    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node) {
    if (!node)
        return DEFAULT_RET_VAL;

    RET_VAL result = DEFAULT_RET_VAL; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch ... | TASK 2: added SYMBOL_NODE_TYPE ...X
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type) {
        case NUM_NODE_TYPE:
            result = evalNumNode(node);
            break;
        case FUNC_NODE_TYPE:
            result = evalFuncNode(node);
            break;
        case SYMBOL_NODE_TYPE:
            result = evalSymbolNode(node);
            break;
        case COND_NODE_TYPE:
            result = evalCondNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(AST_NODE *node) {
    RET_VAL result = DEFAULT_RET_VAL;
    if (!node)
        return result;

    // TODO populate result with the values stored in the node ...X
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    result.value = node->data.number.value;
    result.type = node->data.number.type;
    return result;
}


RET_VAL evalFuncNode(AST_NODE *node) {
    RET_VAL result = DEFAULT_RET_VAL;
    if (!node)
        return result;

    // TODO populate result with the result of running the function on its operands ...
    switch (node->data.function.oper) {
        case NEG_OPER:
        case ABS_OPER:
        case EXP_OPER:
        case SQRT_OPER:
        case LOG_OPER:
        case EXP2_OPER:
        case CBRT_OPER:
            return evalUnary(node);

        case SUB_OPER:
        case DIV_OPER:
        case REMAINDER_OPER:
        case POW_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case HYPOT_OPER:
        case EQUAL_OPER:
        case LESS_OPER:
        case GREATER_OPER:
            return evalBinary(node);

        case ADD_OPER:
            return addFunction(node);
        case MULT_OPER:
            return multFunction(node);
        case PRINT_OPER:
            return printFunction(node);
        case CUSTOM_OPER:
            break;
        default:
            break;
    }
    return result;
}

SYMBOL_TABLE_NODE * findInScope(char * ident, SYMBOL_TABLE_NODE * scope)
{
    while (scope != NULL) {
        if (strcmp(ident, scope->ident) == 0)
            return scope;
        scope = scope->next;
    }
    return NULL;
}

SYMBOL_TABLE_NODE * findSymbol(char * ident, AST_NODE * parent)
{
    if (parent == NULL)
        return NULL;

    SYMBOL_TABLE_NODE * symbol = findInScope(ident, parent->symbolTable);
    if (symbol != NULL)
        return symbol;

    return findSymbol(ident, parent->parent);
}

// TODO | TASK 2 ... X
RET_VAL evalSymbolNode(AST_NODE *symbolNode) {

    RET_VAL result = DEFAULT_RET_VAL;
    if (!symbolNode)
        return result;

    SYMBOL_TABLE_NODE * symbol = findSymbol(symbolNode->data.symbol.ident, symbolNode);
    if (symbol == NULL)
        return result;

    result = eval(symbol->val);
    if (result.type == DOUBLE_TYPE && symbol->val_type == INT_TYPE) {
        result.type = INT_TYPE;
        result.value = floor(result.value);
        printf(PRECISION_LOSS_WARNING, symbol->ident);
    }

    return result;
}

// TASK 6:
RET_VAL evalCondNode(AST_NODE *node) {
    if (!node)
        return (RET_VAL) {DOUBLE_TYPE, NAN};

    RET_VAL result = eval(node->data.condition.cond);

    return eval((result.value ? node->data.condition.trueCond : node->data.condition.falseCond));
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val) {
    // TODO print the type and value of the value passed in ... X

    if (val.type == INT_TYPE) {
        printf("%.0f\n", val.value);
    }
    else {
        printf("%.2f\n", val.value);
    }
}

// TODO | TASK 5: helper functions...should have probably done this earlier ):
RET_VAL evalUnary(AST_NODE *node) {
    if (node->data.function.opList == NULL) {
        printf("ERROR: too few parameters for the function...\n");
        return DEFAULT_RET_VAL;
    }
    if (node->data.function.opList->next != NULL) {
        printf("WARNING: too many parameters for the function...\n");
    }
    switch(node->data.function.oper) {
        case NEG_OPER: return negFunction(node);
        case ABS_OPER: return absFunction(node);
        case EXP_OPER: return expFunction(node);
        case SQRT_OPER: return sqrtFunction(node);
        case LOG_OPER: return logFunction(node);
        case EXP2_OPER: return exp2Function(node);
        case CBRT_OPER: return cbrtFunction(node);
        default: return DEFAULT_RET_VAL;
    }

    return eval(node->data.function.opList);
}

RET_VAL evalBinary(AST_NODE *node)
{
    if (node->data.function.opList == NULL || node->data.function.opList->next == NULL) {
        printf("ERROR: too few parameters for the function...\n");
        return DEFAULT_RET_VAL;
    }
    if (node->data.function.opList->next->next != NULL) {
        printf("WARNING: too many parameters for the function...\n");
    }
    switch(node->data.function.oper) {
        case SUB_OPER: return subFunction(node);
        case DIV_OPER: return divFunction(node);
        case REMAINDER_OPER: return remaindFunction(node);
        case POW_OPER: return powFunction(node);
        case MAX_OPER: return maxFunction(node);
        case MIN_OPER: return minFunction(node);
        case HYPOT_OPER: return hypotFunction(node);
        case EQUAL_OPER: return equalFunction(node);
        case LESS_OPER: return lessFunction(node);
        case GREATER_OPER: return greaterFunction(node);
        default: return DEFAULT_RET_VAL;
    }
}

RET_VAL negFunction(AST_NODE *node) {
    RET_VAL result = eval(node->data.function.opList);
    result.value = -1 * result.value;
    return result;
}
RET_VAL absFunction(AST_NODE *node) {
    RET_VAL result = eval(node->data.function.opList);
    result.value = fabs(result.value);
    return result;
}
RET_VAL expFunction(AST_NODE *node) {
    RET_VAL result = eval(node->data.function.opList);
    result.value = exp(result.value);
    result.type = DOUBLE_TYPE;
    return result;
}
RET_VAL sqrtFunction(AST_NODE *node) {
    RET_VAL result = eval(node->data.function.opList);
    result.value = sqrt(result.value);
    result.type = DOUBLE_TYPE;
    return result;
}
RET_VAL logFunction(AST_NODE *node) {
    RET_VAL result = eval(node);
    result.value = log(result.value);
    result.type = DOUBLE_TYPE;
    return result;
}
RET_VAL exp2Function(AST_NODE * node) {
    RET_VAL result = eval(node->data.function.opList);
    result.value = exp2(result.value);
    result.type = DOUBLE_TYPE;
    return result;
}
RET_VAL cbrtFunction(AST_NODE * node) {
    RET_VAL result = eval(node);
    result.value = exp(result.value);
    result.type = DOUBLE_TYPE;
    return result;
}


NUM_TYPE evalType(RET_VAL op1, RET_VAL op2) {
    if (op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
        return DOUBLE_TYPE;
    if (op1.type == INT_TYPE || op2.type == INT_TYPE)
        return INT_TYPE;
    return NO_TYPE;
}
RET_VAL subFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = op1.value - op2.value;
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL divFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = op1.value / op2.value;
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL remaindFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = fmod(op1.value, op2.value);
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL powFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = pow(op1.value, op2.value);
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL maxFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = fmax(op1.value, op2.value);
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL minFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = fmin(op1.value, op2.value);
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL hypotFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = hypot(op1.value, op2.value);
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL equalFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = op1.value == op2.value;
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL lessFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = op1.value < op2.value;
    result.type = evalType(op1, op2);
    return result;
}
RET_VAL greaterFunction(AST_NODE * node) {
    RET_VAL op1 = eval(node->data.function.opList);
    RET_VAL op2 = eval(node->data.function.opList->next);
    RET_VAL result = DEFAULT_RET_VAL;
    result.value = op1.value > op2.value;
    result.type = evalType(op1, op2);
    return result;
}

RET_VAL addFunction(AST_NODE * node) {
    if (node->data.function.opList == NULL || node->data.function.opList->next == NULL) {
        printf("ERROR: too few parameters for the function...\n");
        return DEFAULT_RET_VAL;
    }

    RET_VAL result = (RET_VAL){NO_TYPE, 0};
    AST_NODE * iter = node->data.function.opList;
    while (iter != NULL) {
        RET_VAL val = eval(iter);
        result.value += val.value;
        result.type = evalType(result, val);
        iter = iter->next;
    }
    return result;
}
RET_VAL multFunction(AST_NODE * node) {
    if (node->data.function.opList == NULL || node->data.function.opList->next == NULL) {
        printf("ERROR: too few parameters for the function...\n");
        return DEFAULT_RET_VAL;
    }

    RET_VAL result = (RET_VAL){NO_TYPE, 1};
    AST_NODE * iter = node->data.function.opList;
    while (iter != NULL) {
        RET_VAL val = eval(iter);
        result.value *= val.value;
        result.type = evalType(result, val);
        iter = iter->next;
    }
    return result;
}
RET_VAL printFunction(AST_NODE * node) {
    if (node->data.function.opList == NULL) {
        printf("ERROR: too few parameters for the function...\n");
        return DEFAULT_RET_VAL;
    }

    RET_VAL result = DEFAULT_RET_VAL;
    printf("=> ");
    AST_NODE * iter = node->data.function.opList;
    while (iter != NULL) {
        result = eval(iter);
        if (result.type==INT_TYPE)
            printf(" %.0f", result.value);
        else
            printf(" %.2f", result.value);
        iter = iter->next;
    }
    printf("\n");
    return result;
}


