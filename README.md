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

### Sample runs:
