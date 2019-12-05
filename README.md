# CI_LISP
## Task 1

### ciLisp.c
#### createNumberNode
Created the number node by setting the node type to NUM_NODE_TYPE and setting it's data type and value to the parameters.
#### createFunctionNode 
Here, this function was first implemented.
#### eval
Implemented a switch case for each node type
#### evalNumNode
Here, this function was first implemented.
#### evalFuncNode
Here, this function was first implemented: a switch case of each function operation
In task 2, I fixed it so it can return the correct type
#### printRetVal
Here, this function was first implemented: print statements of the values type and value.

### ciLisp.h
Removed union - NUM_AST_NODE only has a double type now.

### ciLisp.l
- implemented regex
- finished implementing "func" operations

### ciLisp.y
Left untouched

## Task 2
### ciLisp.c
#### createSymbolNode
Here, this function was first created and implemented: Setting the node to a symbol node
#### attachLetSection
Here, this function was first created and implemented to attach the let list
#### createLetList
Here, this function was first created and implemented to set the new element as the head
#### createdSymbolTableNode
Here, this function was first created and implemented to create the symbol table node
#### eval
SYMBOL_NODE_TYPE was added to the switch statement as a case
#### findSymbolTableNode
Here, this function was first created and implemented to recursively find the symbol table node
#### evalSymNode 
Here, this function was first created and implemented to set a symbol table node to the findSymbolTableNode method to evaluate the value at the node

### ciLisp.h
- added the symbol_table_node struct
- added the ast_node struct
- added the symbol_ast_node struct
- added functions: createSymbolNode, attachLetSection, createLetList, createSymbolTableNode, evalSymNode

### ciLisp.l
- added "let" token

### ciLisp.y
implemented the updated grammer in this file, also added ast_node and symbol_table_node to the union, added LET to the token, created two types(astNode and symAstNode)

## Task 3
### ciLisp.c
#### evalSymType
This method evaluates the symbol type when it's casted and the type adjusts accordingly
#### evalSymNode
Fixed this method to work with new implemented method (evalSymType)
### ciLisp.h
- added the evalSymType 
### ciLisp.l
- added double and int token
### ciLisp.y
changed grammar in y file to take in casting grammar(type)

## Task 4
### ciLisp.c
implemented print function to evalFuncNode
### ciLisp.l
- added "print" key word to lex

## Task 5 
### ciLisp.c
Created many helper functions. The helper functions essentially count the amount of parameters from the console and check if the function operater is able to take in the number by evaluating if it's too less or too many parameters. Add, mult, print, and div have their own functions that get called recursively.
### ciLisp.h
The h file was changed according to the changes that canvas said to change and changed the h file to match the c file
### ciLisp.y
fixed y file to match new grammar with s_expr list



