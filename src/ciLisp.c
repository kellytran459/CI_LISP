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

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE
    node->type=NUM_NODE_TYPE;
    node->data.number = malloc(sizeof(NUM_AST_NODE));
    node->data.number->type = type;
    node->data.number->value = value;

    eval(node);

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

    OPER_TYPE operand = resolveFunc(funcName);
    if(operand == CUSTOM_OPER)
    {
        node->data.function.ident = (char) funcName;
    }
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = operand;
    node->data.function.op1 = op1;
    node->data.function.op2 = op2;
    node->data.function.ident = funcName;
    // check that operand != CUSTOM_OPER bhen you free
    free(funcName);
    eval(node);

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
        return (RET_VAL){INT_TYPE, NAN};

    RET_VAL result = {INT_TYPE, NAN}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch.
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type)
    {
        case FUNC_NODE_TYPE:
            result = evalFuncNode( &node->data.function);
            break;
        case NUM_NODE_TYPE:
            //evalNumNode(node.data.number)
            result = evalNumNode(node->data.number);
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

    // TODO populate result with the values stored in the node.
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

    // TODO populate result with the result of running the function on its operands. Implement add and negate?
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE


    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval(funcNode->op2);

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
            break;
        case SQRT_OPER:
//            if(op1.type == DOUBLE_TYPE)
//            result.value = sqrt(op1.value);
//            else
//            yyerror("Integer type not allowed");

            result.value = sqrt(op1.value);

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
           // result.value = op1.value % op2.value;
          result.value = remainder(op1.value,op2.value);
            break;
        case LOG_OPER:
            result.value = log(op1.value);
            break;
        case POW_OPER: // verify that this is correct
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
            result.value = exp2(op1.value);
            break;
        case CBRT_OPER:
            result.value = cbrt(op1.value);
            break;
        case HYPOT_OPER:
            result.value = hypot(op1.value, op2.value);
            break;
    }
    return result;
}

// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in.
    // when is it a double type, for testing purposes it always goes into the first if statement and prints Type: int
    if(val.type == INT_TYPE)
    {
        printf("Type: int\n");
        printf("%d",(int)val.value);
    }
    else
    {
        printf("Type: double\n");
        printf("%lf",val.value);
    }

}