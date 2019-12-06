#include "ciLisp.h"
#include <limits.h>

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

RET_VAL read()
{
    RET_VAL result = {INT_TYPE, NAN};
//    double val = 0.0;
//    NUM_TYPE type = INT_TYPE;
    char buf[1024];
    printf("read :=");
    fgets(buf, 1024, stdin);

    if(NULL != strchr(buf, '.'))
    {
        result.type = DOUBLE_TYPE;
    }
    result.value = strtod(buf, NULL);

    return result;
}

double randomGenerator()
{
    //return a random double in [0.0, 1.0)
    double res = (rand() % RAND_MAX) / (double)RAND_MAX;
    return res;
}


RET_VAL evalRand() {
    return (RET_VAL) {DOUBLE_TYPE, randomGenerator()};
}


// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op_list)
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
    node->data.function.opList = op_list;

    while (op_list != NULL)
    {
        op_list->parent = node;
        op_list = op_list->next;
    }
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

AST_NODE *createConditionNode(AST_NODE *cond, AST_NODE *nonzero, AST_NODE *zero)
{
    AST_NODE *node;
    size_t nodeSize;

    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(1, nodeSize )) == NULL)
        yyerror("Memory allocation failed!");

    node->type = COND_NODE_TYPE;
    node->data.condition.cond = cond;
    node->data.condition.nonzero = nonzero;
    node->data.condition.zero = zero;

    if(node->data.condition.cond != NULL)
        node->data.condition.cond->parent = node;
    if(node->data.condition.nonzero != NULL)
        node->data.condition.nonzero->parent = node;
    if(node->data.condition.zero != NULL)
        node->data.condition.zero->parent = node;
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

AST_NODE *attachExprLists(AST_NODE *item, AST_NODE *list)
{
    item->next = list;
    return item;
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
    if(s_expr->type == FUNC_NODE_TYPE && (s_expr->data.function.oper == READ_OPER || s_expr->data.function.oper == RAND_OPER)) {
        if(s_expr->data.function.oper == READ_OPER)
        {
            RET_VAL result = read();
            AST_NODE *val = createNumberNode(result.value, result.type);
            node->val = val;
            node->val_type = NO_TYPE;
            freeNode(s_expr);
        }
       else if(s_expr->data.function.oper == RAND_OPER){
            RET_VAL result = evalRand();
            AST_NODE *val = createNumberNode(result.value, DOUBLE_TYPE);
            node->val = val;
            node->val_type = DOUBLE_TYPE;
            freeNode(s_expr);
       }
    } else {
        node->val = s_expr;
    }
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
        freeNode(node->data.function.opList);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

RET_VAL evalCondNode(AST_NODE *condNode)
{
    if(eval(condNode->data.condition.cond).value)
        return eval(condNode->data.condition.nonzero);

    return eval(condNode->data.condition.zero);
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
        case COND_NODE_TYPE:
            result = evalCondNode(node);
            break;
        default:
            yyerror("Invalid AST_NODE_TYPE, probably invalid writes somewhere!");
            break;
    }
    return result;
}

int maxNumOfParameters (OPER_TYPE oper)
{
    switch(oper)
    {
        case READ_OPER:
        case RAND_OPER:
            return 0;
        case NEG_OPER:
        case ABS_OPER:
        case EXP_OPER:
        case SQRT_OPER:
        case LOG_OPER:
        case CBRT_OPER:
        case EXP2_OPER:
            return 1;
        case SUB_OPER:
        case DIV_OPER:
        case REMAINDER_OPER:
        case POW_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case HYPOT_OPER:
        case GREATER_OPER:
        case LESS_OPER:
        case EQUAL_OPER:
            return 2;
        case ADD_OPER:
        case MULT_OPER:
        case PRINT_OPER:
            return INT_MAX;
        default:
        printf("Undefined");
    }
    return 0;
}

int minNumOfParameters (OPER_TYPE oper)
{
    switch(oper)
    {
        case READ_OPER:
        case RAND_OPER:
            return 0;
        case NEG_OPER:
        case ABS_OPER:
        case EXP_OPER:
        case SQRT_OPER:
        case LOG_OPER:
        case CBRT_OPER:
        case PRINT_OPER:
        case EXP2_OPER:
            return 1;
        case SUB_OPER:
        case DIV_OPER:
        case REMAINDER_OPER:
        case POW_OPER:
        case MAX_OPER:
        case MIN_OPER:
        case HYPOT_OPER:
        case ADD_OPER:
        case MULT_OPER:
        case GREATER_OPER:
        case LESS_OPER:
        case EQUAL_OPER:
            return 2;
        default:
            printf("Undefined");
    }
    return 0;
}

int sizeOfOplist (AST_NODE *opList)
{
    int count = 0;
    while(opList != NULL)
    {
        count++;
       opList = opList->next;
    }
    return count;
}

bool isCorrectParameters (OPER_TYPE oper, int count, int min, int max) {
    if (count < min) {
        printf("Error: Too few arguments: %s\n", funcNames[oper]);
        return false;
    }
    if (count > max) {
        printf("Warning: Too many arguments: %s\n", funcNames[oper]);
    }
    return true;
}


bool evalParameters (OPER_TYPE func, AST_NODE *oper)
{
    return isCorrectParameters(func, sizeOfOplist(oper), minNumOfParameters(func), maxNumOfParameters(func));
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

NUM_TYPE evalFuncType (OPER_TYPE oper, NUM_TYPE op1, NUM_TYPE op2)
{
    if(op1 == INT_TYPE)
    {
        if(oper == NEG_OPER)
        {
            return INT_TYPE;
        }
        if(oper == ADD_OPER || oper == SUB_OPER || oper == MULT_OPER || oper == DIV_OPER)
        {
            if(op2 == INT_TYPE)
            {
                return INT_TYPE;
            }
        }
    }
    return DOUBLE_TYPE;
}

RET_VAL divide(AST_NODE *funcNode)
{

    if(funcNode == NULL)
    {
        return (RET_VAL){INT_TYPE, 1.0};
    }

    RET_VAL result = divide(funcNode->next);
    RET_VAL temp = eval(funcNode);

    if(result.value == 0)
    {
        printf("WARNING: Cannot divide by 0");
        return (RET_VAL){INT_TYPE, NAN};
    }

    return (RET_VAL)
            {
                    evalFuncType(DIV_OPER, temp.type, result.type), temp.value / result.value
            };
}



double evalFuncNodeVal (OPER_TYPE oper, double op1, double op2)
{
    switch (oper)
    {
        case NEG_OPER:
            return op1*-1;
        case ABS_OPER:
            return fabs(op1);
        case EXP_OPER:
            return exp(op1);
        case SQRT_OPER:
            return sqrt(op1);
        case SUB_OPER:
            return op1 - op2;

        case REMAINDER_OPER:
            return remainder(op1,op2);
        case LOG_OPER:
            if(op1 == 0)
            {
                printf("WARNING: log 0 is undefined");
                return NAN;
            }
            return log10(op1);
        case POW_OPER: // verify that this is correct
            return pow(op1, op2);
        case MAX_OPER:
            return fmax(op1,op2);
        case MIN_OPER:
            return fmin(op1, op2);
        case EXP2_OPER:
            return  exp2(op1);
        case CBRT_OPER:
            return cbrt(op1);
        case HYPOT_OPER:
            return hypot(op1,op2);
        case GREATER_OPER:
            if(op1 > op2)
            {
                return 1.0;
            }
            else
            {
                return 0.0;
            }
        case LESS_OPER:
            if(op1 < op2)
            {
                return 1.0;
            }
            else
            {
                return 0.0;
            }
        case EQUAL_OPER:
            if(op1 == op2)
            {
                return 1.0;
            }
            else
            {
                return 0.0;
            }
    }
}



RET_VAL add(AST_NODE *funcNode)
{
    if(funcNode == NULL)
    {
        return (RET_VAL){INT_TYPE, 0};
    }

    if(funcNode->next == NULL)
    {
        return eval(funcNode);
    }

    RET_VAL result = add(funcNode->next);
    RET_VAL temp = eval(funcNode);

    return (RET_VAL)
            {
        evalFuncType(ADD_OPER, temp.type, result.type), temp.value + result.value
            };
}

RET_VAL mult(AST_NODE *funcNode)
{
    if(funcNode == NULL)
    {
        return (RET_VAL){INT_TYPE, 1.0};
    }

    RET_VAL result = mult(funcNode->next);
    RET_VAL temp = eval(funcNode);

    return (RET_VAL)
            {
                    evalFuncType(MULT_OPER, temp.type, result.type), temp.value * result.value
            };
}


RET_VAL print (AST_NODE *func)
{
    RET_VAL var = {INT_TYPE, 0};
    if(func != NULL)
    {
        printf("=>");
        while(func != NULL)
        {
            var = eval(func);
        if(var.type == INT_TYPE)
            printf(" %.0lf", var.value);
        else
        {
            printf(" %.2f", var.value);
        }
          func = func->next;
        }
        printf("\n");
    }
    return var;
}



RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode)
{
//    if (!funcNode)
//        return (RET_VAL){INT_TYPE, NAN};

  //  RET_VAL result = {INT_TYPE, 0};

    // TODO populate result with the result of running the function on its operands.  Might need to further adjust
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE

    if(funcNode->oper == READ_OPER) {
        return read();
    }
    if(funcNode->oper == RAND_OPER) {
        return evalRand();
    }
    if(!evalParameters(funcNode->oper, funcNode->opList))
    {
        return (RET_VAL){INT_TYPE, NAN};
    }

    if(funcNode->oper == ADD_OPER)
    {
        return add(funcNode->opList);
    }
    if(funcNode->oper == MULT_OPER)
    {
       return mult(funcNode->opList);
    }
    if(funcNode->oper == PRINT_OPER)
    {
       return print(funcNode->opList);
    }
    if(funcNode->oper == DIV_OPER)
    {
        return divide(funcNode->opList);
    }
    RET_VAL op1 = eval(funcNode->opList);
    RET_VAL op2 = eval(funcNode->opList->next);

    return (RET_VAL)
            {
                    evalFuncType(funcNode->oper, op1.type, op2.type),
                    evalFuncNodeVal(funcNode->oper, op1.value, op2.value)
            };
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



RET_VAL evalSymType (SYMBOL_TABLE_NODE * node, RET_VAL val)
{
   // RET_VAL symbol = eval(node->val);
    if(node->val_type == INT_TYPE && val.type == DOUBLE_TYPE)
    {
        printf("WARNING: precision loss in the assignment for variable %s\n", node->ident);
        freeNode((node->val));
        node->val = createNumberNode((val.value), INT_TYPE);
        return (RET_VAL){INT_TYPE, floor(val.value)};
    }
    if(node->val_type == DOUBLE_TYPE && val.type == INT_TYPE)
    {
        //  printf("No precision loss, add .0 to make it a double\n");
        return (RET_VAL){DOUBLE_TYPE, (val.value)};
    }
    return val;
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
        return evalSymType(node, eval(node->val));

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
        // remove this at the end
        printf("Type: double\n");
        printf("%lf",val.value);
    }
}