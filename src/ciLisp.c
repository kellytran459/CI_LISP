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
        "greater",
        ""
};

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE - done
    node->type = NUM_NODE_TYPE;
    //node->data.number = malloc(sizeof(NUM_AST_NODE));
    node->data.number.type = type;
    node->data.number.value = value;

    // eval(node);
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
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE - done
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.

    OPER_TYPE operand = resolveFunc(funcName);
    if(operand == CUSTOM_OPER)
    {
        node->data.function.ident = funcName;
    }
    else
    {
        free(funcName);
    }
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = operand;
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;

    op1->parent = node;

    //  Adding parantage to op1 and op2
    //  Because op2 can be null, needs special check (NULL POINTER EXCEPTION)
    //  Functions are the only place that cause parantage
    //node->data.function.op1->parent = node;

    if (op2 != NULL) {
        //  node->data.function.op2->parent = node;
        op2->parent = node;
    }
//    else
//    {
//        printf("op2 is null - for debugging purposes\n");
//    }
    return node;
}

//TODO: create SymbolNode - done
AST_NODE *createSymbolNode(char *symbolName)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(1, nodeSize )) == NULL)
        yyerror("Memory allocation failed!");
    //set node type for reading during evaluation
    node->type = SYMBOL_NODE_TYPE;
    //NULL - for garbage collection
    node->parent = NULL;
    //setting the ident pointer to the symbolName to use the same address
    node->data.symbol.ident = symbolName;
    return node;
}

AST_NODE *attachLetSection(SYMBOL_TABLE_NODE *let_list, AST_NODE *s_expr)
//TODO attachLetSection - task 3
{
    s_expr->symbolTable = let_list;

    SYMBOL_TABLE_NODE *temp = let_list;

    while(temp != NULL)
    {
        temp->val->parent = s_expr;
        temp = temp->next;
    }

    return s_expr;
}

SYMBOL_TABLE_NODE *createLetList(SYMBOL_TABLE_NODE *let_list, SYMBOL_TABLE_NODE *let_elem)
//TODO createLetList
{
    //setting new elem as head
    let_elem->next = let_list;
    return let_elem;
}

SYMBOL_TABLE_NODE *createSymbolTableNode(char *symbol, AST_NODE *s_expr, NUM_TYPE type)
//TODO createSymbolTableNode
{
    SYMBOL_TABLE_NODE *node;
    size_t nodeSize;
    nodeSize = sizeof(SYMBOL_TABLE_NODE);
    if ((node = calloc(1, nodeSize)) == NULL)
        yyerror("Memory allocation failed!");
    //set node type for reading during evaluation
    node->val = s_expr;
    node->ident = symbol;
    node->val_type = type;
    //this will change once I start building the let list
    node->next = NULL;
    return node;
}


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
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch. - done
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
            result = evalSymNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
            break;
    }
    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the values stored in the node. - done
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    // assign result.value to be equal to numNode.value and thereforth
    result.value = numNode->value;
    result.type = numNode->type;
    return result;
}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
    if (!funcNode)
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    // TODO populate result with the result of running the function on its operands.  Might need to further adjust
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE


    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);
    RET_VAL temp = eval(funcNode->op1);

    result.type = INT_TYPE; // Adding, subtracting, multiplying, or negating operands of type int should yield an s-expression with type int
    if(op1.type == DOUBLE_TYPE || op2.type == DOUBLE_TYPE)
    {
        result.type = DOUBLE_TYPE;
    }
    //result.type = DOUBLE_TYPE;
    switch (funcNode->oper)
    {
        case ADD_OPER:
            result.value = op1.value + op2.value;
            break;
        case NEG_OPER:
            result.value = op1.value*-1;
            break;
        case ABS_OPER:
            result.value = fabs(op1.value);
            break;
        case EXP_OPER:
            result.value = exp(op1.value);
            result.type = DOUBLE_TYPE;
            break;
        case SQRT_OPER:
            result.type = DOUBLE_TYPE;
            result.value = sqrt(op1.value);
            // result.type = DOUBLE_TYPE;
            break;
        case SUB_OPER:
            result.value = op1.value - op2.value;
            break;
        case MULT_OPER:
            result.value = op1.value * op2.value;
            break;
        case DIV_OPER:
            //if result is a double: result.type = double
            if(op1.value == 0)
            {
                printError();
            }
            if((op1.type == INT_TYPE && op2.type == INT_TYPE))
            {
                result.value = floor(result.value);
                result.type = INT_TYPE;
            }
            result.value = op1.value / op2.value;
            break;
        case REMAINDER_OPER:
            result.type = DOUBLE_TYPE;
            result.value = remainder(op1.value,op2.value);
            break;
        case LOG_OPER:
            if(op1.value == 0)
            {
                printError();
            }
            result.type = DOUBLE_TYPE;
            //log or log10
            result.value = log10(op1.value);
            break;
        case POW_OPER: // verify that this is correct
            result.type = DOUBLE_TYPE;
            result.value = pow(op1.value, op1.value);
            break;
        case MAX_OPER:
            if(op1.value > op2.value)
            {
                result.value = op1.value;
            }
            else if(op2.value >= op1.value)
            {
                result.value = op2.value;
            }
            break;
        case MIN_OPER:
            if(op1.value < op2.value)
            {
                result.value = op1.value;
            }
            else if(op2.value <= op1.value)
            {
                result.value = op2.value;
            }
            break;
        case EXP2_OPER:
            result.type = DOUBLE_TYPE;
            result.value = exp2(op1.value);
            break;
        case CBRT_OPER:
            result.type = DOUBLE_TYPE;
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            result.type = DOUBLE_TYPE;
            result.value = hypot(op1.value, op2.value);
            break;
    }
    return result;
}


SYMBOL_TABLE_NODE *findSymbolTableNode(char *ident, AST_NODE *ast_Node) {
    if (ast_Node == NULL)
        return NULL;

    SYMBOL_TABLE_NODE *symbolTableNode_temp = ast_Node->symbolTable;
    // RET_VAL result;

    while (symbolTableNode_temp != NULL) {
        if (strcmp(symbolTableNode_temp->ident, ident) == 0)
            return symbolTableNode_temp;
        symbolTableNode_temp = symbolTableNode_temp->next;
    }
    return findSymbolTableNode(ident, ast_Node->parent);
}

RET_VAL evalSymType (SYMBOL_TABLE_NODE * node)
{
    RET_VAL symbol = eval(node->val);
    if(node->val_type == INT_TYPE && symbol.type == DOUBLE_TYPE)
    {
        printf("Precision loss\n");
        freeNode((node->val));
        node->val = createNumberNode((symbol.value), INT_TYPE);
        return (RET_VAL){INT_TYPE, floor(symbol.value)};
    }
    if(node->val_type == DOUBLE_TYPE && symbol.type == INT_TYPE)
    {
        printf("No precision loss, add .0 to make it a double");
        freeNode((node->val));
        node->val = createNumberNode((symbol.value), DOUBLE_TYPE);
        return (RET_VAL){DOUBLE_TYPE, (symbol.value)};
    }
//    if(node->val_type == INT_TYPE && symbol.type == INT_TYPE)
//    {
//        printf("Precision loss\n");
//        freeNode((node->val));
//        node->val = createNumberNode((symbol.value), INT_TYPE);
//        return (RET_VAL){INT_TYPE, floor(symbol.value)};
//    }

    return symbol;
}

RET_VAL evalSymNode(AST_NODE *symNode)
//TODO evalSymNode - implement further for task 3
{
    if (!symNode)
        return (RET_VAL) {INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN};

    SYMBOL_TABLE_NODE *node = findSymbolTableNode(symNode->data.symbol.ident, symNode);
    // eval the value fo node and return
    if (node != NULL)
        return evalSymType(node);

    return evalSymNode(symNode->parent);
}


void printError()
{
    printf("This function is not allowed");
    exit(0);
}
// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in. - done
    // when is it a double type, for testing purposes it always goes into the first if statement and prints Type: int
    if(val.type == INT_TYPE)
    {
        printf("Type: int\n");
        printf("%ld",(long)val.value);
    }
    else
    {
        printf("Type: double\n");
        printf("%lf",val.value);
    }
}