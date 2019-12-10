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


OPER_TYPE resolveFunc(char *funcName) {
    int i = 0;
    while (funcNames[i][0] != '\0') {
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
AST_NODE *createNumberNode(double value, NUM_TYPE type) {
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
AST_NODE *createFunctionNode(char *funcName, AST_NODE *opList) {
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
    node->next = NULL;


    if (node->data.function.oper == CUSTOM_OPER) {
        node->data.function.ident = malloc(sizeof(char));
        node->data.function.ident = funcName;
    } else {
        free(funcName);
    }

    // Task 5
    node->data.function.opList = opList;

    // Task 5
    AST_NODE *currentOp = opList;
    while (currentOp != NULL) {
        currentOp->parent = node;
        currentOp = currentOp->next;
    }

    return node;
}

AST_NODE *createSymbolNode(char *symbolTableNode) {
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

AST_NODE *createSymbolTableNode(SYMBOL_TABLE_NODE *symbolTable, AST_NODE *s_expr) {
    s_expr->symbolTable = symbolTable;

    return s_expr;
}

AST_NODE *s_expr_list(AST_NODE *s_expr, AST_NODE *s_expr_list) {
    s_expr->next = s_expr_list;
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
void freeNode(AST_NODE *node) {
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE) {
        // Recursive calls to free child nodes
//        freeNode(node->data.function.op1);
//        freeNode(node->data.function.op2);

        // Task 5
        AST_NODE *curr;
        while (node->data.function.opList != NULL) {
            curr = node->data.function.opList;
            node->data.function.opList = node->data.function.opList->next;
            free(curr);
        }

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER) {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node) {
    if (!node)
        return (RET_VAL) {DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type) {
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
RET_VAL evalNumNode(NUM_AST_NODE *numNode) {
    if (!numNode)
        return (RET_VAL) {INT_TYPE, NAN};

    //RET_VAL result = {INT_TYPE, NAN};
    //RET_VAL result = {DOUBLE_TYPE, NAN};

    RET_VAL result; // Updated for Task 3

    // TODO populate result with the values stored in the node.
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE

    // Updated for Task 4
    result.value = numNode->value;
    result.type = numNode->type;

    return result;
}

RET_VAL negFunction(FUNC_AST_NODE *negNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(negNode);
    result.value = -1 * op1.value;
    result.type = op1.type;
    return result;

}

RET_VAL absFunction(FUNC_AST_NODE *absNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(absNode);
    result.value = fabs(op1.value);
    result.type = op1.type;
    return result;

}

RET_VAL expFunction(FUNC_AST_NODE *expNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(expNode);
    if (result.type == INT_TYPE) {
        result.value = round(exp(op1.value));
        return result;
    }
    result.value = exp(op1.value);
    return result;

}

RET_VAL sqrtFunction(FUNC_AST_NODE *sqrtNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(sqrtNode);
    if (result.type == INT_TYPE) {
        result.value = round(sqrt(op1.value));
        return result;
    }
    result.value = sqrt(op1.value);
    return result;

}

RET_VAL addFunction(FUNC_AST_NODE *addNode, RET_VAL op1) {
    RET_VAL op2;
    AST_NODE *node = addNode->opList;
    int value = (addNode->oper == ADD_OPER) ? 0 : 1;
    op1.value = value;
    while (node != NULL) {
        op2 = eval(node);
        op1.value = op1.value + op2.value;
        if (op2.type == DOUBLE_TYPE) {
            op1.type = DOUBLE_TYPE;
        }
        node = node->next;
    }
    return op1;
}

RET_VAL subFunction(FUNC_AST_NODE *subNode, RET_VAL op1, RET_VAL op2) {
    evalBi(subNode);
    op1 = eval(subNode->opList);
    op2 = eval(subNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    result.value = op1.value - op2.value;
    return result;
}

RET_VAL multFunction(FUNC_AST_NODE *multNode, RET_VAL op1) {
    RET_VAL op2;
    AST_NODE *node = multNode->opList;
    int value = (multNode->oper == ADD_OPER) ? 0 : 1;
    op1.value = value;
    while (node != NULL) {
        op2 = eval(node);
        op1.value = op1.value + op2.value;
        if (op2.type == DOUBLE_TYPE) {
            op1.type = DOUBLE_TYPE;
        }
        node = node->next;
    }
    return op1;
}

RET_VAL divFunction(FUNC_AST_NODE *divNode, RET_VAL op1, RET_VAL op2) {
    evalBi(divNode);
    op1 = eval(divNode->opList);
    op2 = eval(divNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    if (result.type == INT_TYPE) {
        result.value = round(op1.value / op2.value);
        return result;
    }
    result.value = op1.value / op2.value;
    return result;
}

RET_VAL remainderFunction(FUNC_AST_NODE *remainderNode, RET_VAL op1, RET_VAL op2) {
    evalBi(remainderNode);
    op1 = eval(remainderNode->opList);
    op2 = eval(remainderNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    if (result.type == INT_TYPE) {
        result.value = round(remainder(op1.value, op2.value));
        return result;
    }
    result.value = remainder(op1.value, op2.value);
    return result;
}

RET_VAL logFunction(FUNC_AST_NODE *logNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(logNode);

    if (result.type == INT_TYPE) {
        result.value = round(log(op1.value));
        return result;
    }
    result.value = log(op1.value);
    return result;
}

RET_VAL powFunction(FUNC_AST_NODE *powNode, RET_VAL op1, RET_VAL op2) {
    evalBi(powNode);
    op1 = eval(powNode->opList);
    op2 = eval(powNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    result.value = pow(op1.value, op2.value);
    return result;
}

RET_VAL maxFunction(FUNC_AST_NODE *maxNode, RET_VAL op1, RET_VAL op2) {
    evalBi(maxNode);
    op1 = eval(maxNode->opList);
    op2 = eval(maxNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    result.value = fmax(op1.value, op2.value);
    return result;
}

RET_VAL minFunction(FUNC_AST_NODE *minNode, RET_VAL op1, RET_VAL op2) {
    evalBi(minNode);
    op1 = eval(minNode->opList);
    op2 = eval(minNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    result.value = fmin(op1.value, op2.value);
    return result;
}

RET_VAL exp2Function(FUNC_AST_NODE *exp2Node, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    evalUni(exp2Node);
    op1 = eval(exp2Node->opList);

    if (result.type == INT_TYPE) {
        result.value = round(exp(op1.value));
        return result;
    }
    result.value = exp(op1.value);
    return result;


}

RET_VAL cbrtFunction(FUNC_AST_NODE *cbrtNode, RET_VAL op1) {
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};
    op1 = evalUni(cbrtNode);

    if (result.type == INT_TYPE) {
        result.value = round(cbrt(op1.value));
        return result;
    }
    result.value = exp(op1.value);
    return result;
}

RET_VAL hypotFunction(FUNC_AST_NODE *hypotNode, RET_VAL op1, RET_VAL op2) {
    evalBi(hypotNode);
    op1 = eval(hypotNode->opList);
    op2 = eval(hypotNode->opList->next);
    RET_VAL result = (RET_VAL) {DOUBLE_TYPE, NAN};

    if (result.type == INT_TYPE) {
        result.value = round(hypot(op1.value, op2.value));
        return result;
    }
    result.value = hypot(op1.value, op2.value);
    return result;
}

RET_VAL evalUni(FUNC_AST_NODE *evalUni) {
    RET_VAL oper = eval(evalUni->opList);
    if (evalUni->opList == NULL) {
        printf("ERROR: too few parameters for the function\n");
        exit(-1);
    }
    if (evalUni->opList->next != NULL) {
        printf("WARNING: too many parameters for the function\n");
    }
    return oper;
}

RET_VAL evalBi(FUNC_AST_NODE *evalBi) {
    RET_VAL op = eval(evalBi->opList);
    if (evalBi->opList == NULL) {
        printf("ERROR: too few parameters for the function\n");
        exit(-1);
    }

    if (evalBi->opList->next == NULL) {
        printf("ERROR: too few parameters for the function\n");
        exit(-1);
    }

    if (evalBi->opList->next->next != NULL) {
        printf("WARNING: too many parameters for the function\n");
    }
    return op;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};

    RET_VAL op1 = result;
    RET_VAL op2 = result;

    // TODO DONE populate result with the result of running the function on its operands.
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    // Updated from previous submission
    switch (funcNode->oper) {
        case NEG_OPER:
            result = negFunction(funcNode, op1);
            break;
        case ABS_OPER:
            result = absFunction(funcNode, op1);
            break;
        case EXP_OPER:
            result = expFunction(funcNode, op1);
            break;
        case SQRT_OPER:
            result = sqrtFunction(funcNode, op1);
            break;
        case ADD_OPER:
            result = addFunction(funcNode, op1);
            break;
        case SUB_OPER:
            result = subFunction(funcNode, op1, op2);
            break;
        case MULT_OPER:
            result = multFunction(funcNode, op1);
            break;
        case DIV_OPER:
            result = divFunction(funcNode, op1, op2);
            break;
        case REMAINDER_OPER:
            result = remainderFunction(funcNode, op1, op2);
            break;
        case LOG_OPER:
            result = logFunction(funcNode, op1);
            break;
        case POW_OPER:
            result = powFunction(funcNode, op1, op2);
            break;
        case MAX_OPER:
            result = maxFunction(funcNode, op1, op2);
            break;
        case MIN_OPER:
            result = minFunction(funcNode, op1, op2);
            break;
        case EXP2_OPER:
            result = exp2Function(funcNode, op1);
            break;
        case CBRT_OPER:
            result = cbrtFunction(funcNode, op1);
            break;
        case HYPOT_OPER:
            result = hypotFunction(funcNode, op1, op2);
            break;
        case PRINT_OPER:
            result = printFunction(funcNode, result);
            break;
        case EQUAL_OPER:
            break;
        case LESS_OPER:
            break;
        case GREATER_OPER:
            break;
        case CUSTOM_OPER:
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
        return (RET_VAL) {DOUBLE_TYPE, NAN};

    RET_VAL result = {DOUBLE_TYPE, NAN};
    char *symbol = symbolNode->data.symbol.ident;
    AST_NODE *currentNode = symbolNode;
    SYMBOL_TABLE_NODE *currentTable;
    SYMBOL_TABLE_NODE *found = NULL;
    RET_VAL temp;

    while (currentNode != NULL) {
        currentTable = currentNode->symbolTable;

        while (currentTable != NULL) {
            if (strcmp(symbol, currentTable->ident) == 0) {
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
        if (temp.type == DOUBLE_TYPE && found->val_type == INT_TYPE) {
            temp.type = INT_TYPE;
            temp.value = floor(temp.value);
            printf("WARNING: precision loss in the assignment for variable %s\n", symbol);
        }
        return temp;
    }

    return result;

}

RET_VAL printFunction(FUNC_AST_NODE *print, RET_VAL result) {
    printf("=> ");
    while (print->opList != NULL) {
        result = eval(print->opList);
        printRetVal(result);
        print->opList = print->opList->next;
        if (print->opList == NULL) {
            return (RET_VAL) {DOUBLE_TYPE, NAN};
        }
    }
    return result;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val) {
    // TODO DONE print the type and value of the value passed in.

    //Updated from previous submission

    //printf("Data: %lf\n", val.value);
    printf("%.2lf", val.value);
    if (val.type == INT_TYPE) {
        printf("\t");
       // printf("Type: Integer \n");
    } else if (val.type == DOUBLE_TYPE) {
        printf("\t");
        //printf("Type: Double \n");
    } else {
        printf("I don't know what type you are \n");
    }

}

