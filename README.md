# CI_LISP

## I finished at Task 6, it works fine to me, but I am not sure if there are any other errors.
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

### Sample Runs
(neg (abs (sub 5 3.2)))
Type: double
-1.800000


(sqrt (remainder 15.7 3.7))
Type: double
0.948683

(pow 5.5 4)
Type: double
915.062500

(max (min 3 4) (sub 8 9))
Type: double
3.000000

(log (exp2 (div 10 5)))
Type: double
0.602060

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

### Sample Runs
(add ((let (abcd 1)) (sub 3 abcd)) 4)
Type: int
6

(mult ((let (a 1) (b 2)) (add a b)) (sqrt 2))
Type: double
4.242641

(add ((let (a ((let (b 2)) (mult b (sqrt 10))))) (div a 2)) ((let (c 5)) (sqrt c)))
Type: double
5.398346

((let (first (sub 5 1)) (second 2)) (add (pow 2 first) (sqrt second)))
Type: double
17.414214


((let (a ((let (c 3) (d 4)) (mult c d)))) (sqrt a))
Type: double
3.464102

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
### Sample Runs
WARNING: precision loss in the assignment for variable a
Type: int
2

 ((let (double a 2))(add a 2))
Type: double
4.000000

((let (double a 3))(add a 5.75))
Type: double
8.750000

## Task 4
### ciLisp.c
implemented print function to evalFuncNode
### ciLisp.l
- added "print" key word to lex
### Sample Runs
(mult ((let (int a 3.54)) (print (sub 7 a))) 2)
=>WARNING: precision loss in the assignment for variable a
 4
Type: int
8

(mult ((let (a ((let (b 7.9)) (print (add b (log 12)))))) (div a 3)) ((let (c 34)) (sqrt c)))

> => 8.98
Type: double
17.452391

## Task 5 
### ciLisp.c
Created many helper functions. The helper functions essentially count the amount of parameters from the console and check if the function operater is able to take in the number by evaluating if it's too less or too many parameters. Add, mult, print, and div have their own functions that get called recursively.
### ciLisp.h
The h file was changed according to the changes that canvas said to change and changed the h file to match the c file
### ciLisp.y
fixed y file to match new grammar with s_expr list
### Sample Runs
> (add 1)
Error: Too few arguments: add
Type: int
-9223372036854775808

> (add 1 2 3 4 5)
Type: int
15

> ((let (int a 1)(double b 2))(print a b 3))
=> 1 2.00 3
Type: int
3


## Task 6
Created rand, read, and condition functions

### Sample Runs
> ((let (int a (read)) (double b (read)) (c (read)) (d (read))) (print a b c d))
read :=3
read :=5.0
read :=10
read :=5.175
=> 3 5.00 10 5.17
Type: double
5.175000


> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read :=6
=> 0.00
Type: double
0.000008
> ((let (myA (read))(myB (rand)))(cond (less myA myB) (print myA) (print myB)))
read :=7
=> 0.13
Type: double
0.131538
