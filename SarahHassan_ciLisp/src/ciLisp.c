#include "ciLisp.h"

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
        "greater"
};


OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed size and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE

    node->type = NUM_NODE_TYPE;
    node->data.number.type = type;
    node->data.number.value = value;

    node->parent = NULL;

    return node;
}

// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.

    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);

    node->parent = NULL;

    if(node->data.function.oper  == CUSTOM_OPER) {
        node->data.function.ident = malloc(sizeof(char));
        node->data.function.ident = funcName;
    }
    else {
        free(funcName);
    }

    node->data.function.op1 = op1;
    node->data.function.op2 = op2;

    if(op1 != NULL) {
        node->data.function.op1->parent = node;
    }
    if(op2 != NULL) {
        node->data.function.op2->parent = node;
    }

    return node;
}

AST_NODE *createSymbolNode(char *symbolTableNode){
    // copied AST_NODE createFunctionNode
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    node->type = SYMBOL_NODE_TYPE;
    node->parent = NULL;
    node->data.symbol.ident = symbolTableNode;

    return node;
}
AST_NODE *createSymbolTableNode(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr)
{
    //SYMBOL_TABLE_NODE *node = symbolTable;
    s_expr->symbolTable = symbolTable;

//    while ( node != NULL )
//    {
//        node->val->parent = s_expr;
//        node = node->next;
//    }

    return s_expr;
}

SYMBOL_TABLE_NODE *addSymbolToList(SYMBOL_TABLE_NODE *list, SYMBOL_TABLE_NODE *elem) // let_list
{
    elem->next = list;

    return elem;
}

SYMBOL_TABLE_NODE *createSymbol(NUM_TYPE type, char *symbol, AST_NODE *s_expr) // let_elem
{
    // followed AST_NODE createFunctionNode
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // Updated for Task 3
    node->val_type = type;
    node->ident = symbol;
    node->val = s_expr;
    node->next = NULL;

    return node;
}


// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        case SYMBOL_NODE_TYPE:
            result = evalSymbolNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
    }

    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, NAN};

    //RET_VAL result = {INT_TYPE, NAN};
    //RET_VAL result = {DOUBLE_TYPE, NAN};

    RET_VAL result; // Updated for Task 3

    // TODO populate result with the values stored in the node.
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE

    //result.value = numNode->value;
    result = *numNode; // Updated for Task 3

    return result;
}

RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};

    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);

    // TODO DONE populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    // Updated from previous submission
    switch(funcNode->oper)
    {
        case NEG_OPER:
            result.value = -op1.value;
            break;
        case ABS_OPER:
            result.value = fabs(op1.value);
            break;
        case EXP_OPER:
            result.value = exp(op1.value);
            break;
        case SQRT_OPER:
            result.value = sqrt(op1.value);
            break;
        case ADD_OPER:
            result.value = op1.value + op2.value;
            break;
        case SUB_OPER:
            result.value = op1.value - op2.value;
            break;
        case MULT_OPER:
            result.value = op1.value * op2.value;
            break;
        case DIV_OPER:
            result.value = op1.value / op2.value;
            break;
        case REMAINDER_OPER:
            result.value = fmod(op1.value, op2.value);
            break;
        case LOG_OPER:
            result.value = log(op1.value);
            break;
        case POW_OPER:
            result.value = pow(op1.value, op2.value);
            break;
        case MAX_OPER:
            result.value = fmax(op1.value, op2.value);
            break;
        case MIN_OPER:
            result.value = fmin(op1.value, op2.value);
            break;
        case EXP2_OPER:
            result.value = exp2(op1.value);
            break;
        case CBRT_OPER:
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            result.value = hypot(op1.value, op2.value);
            break;
        default:
            break;
    }
    return result;
}

// Task 2 Implementation
RET_VAL evalSymbolNode(AST_NODE *symbolNode) {
    // following RET_VAL evalFuncNode
    if (!symbolNode)
        return (RET_VAL){DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};
    char *symbol = symbolNode->data.symbol.ident;
    AST_NODE *currentNode = symbolNode;
    SYMBOL_TABLE_NODE *currentTable;
    SYMBOL_TABLE_NODE *found = NULL;
    RET_VAL temp;

    while (currentNode != NULL)
    {
        currentTable = currentNode->symbolTable;

        while (currentTable != NULL)
        {
            if (strcmp(symbol, currentTable->ident) == 0)
            {
                found = currentTable;
                break;
            }
            currentTable = currentTable->next;
        }
        currentNode = currentNode->parent;
    }
    // Task 3
    if (found != NULL) {
        temp = eval(found->val);
        if(temp.type == DOUBLE_TYPE && found->val_type == INT_TYPE) {
            temp.type = INT_TYPE;
            temp.value = floor(temp.value);
            printf("WARNING: precision loss in the assignment for variable %s\n", symbol);
        }
        return temp;
    }

    return result;

}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO DONE print the type and value of the value passed in.

    // Updated from previous submission
    printf("Data: %f\n", val.value);
    if (val.type == INT_TYPE) {
        printf("Type: Integer");
    }
    else if (val.type == DOUBLE_TYPE) {
        printf("Type: Double");
    }
    else {
        printf("I don't know what type you are");
    }

}