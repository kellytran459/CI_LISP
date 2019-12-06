%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symAstNode;
};

%token <sval> FUNC SYMBOL
%token <dval> INT DOUBLE
%token LPAREN RPAREN EOL QUIT LET TYPE_INT TYPE_DOUBLE COND

%type <astNode> s_expr f_expr number s_expr_list
%type <symAstNode> let_list let_elem let_section
%%


program:
    s_expr EOL {
        fprintf(stderr, "yacc: program ::= s_expr EOL\n");
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
    };

s_expr:
    number {
        fprintf(stderr, "yacc: s_expr ::= number\n");
        $$ = $1;
    }
    | SYMBOL {
       fprintf(stderr, "yacc: s_expr ::= SYMBOL\n");
       $$ = createSymbolNode($1);
    }
    | f_expr {
        fprintf(stderr, "yacc: s_expr ::= f_expr\n");
        $$ = $1;
    }
    | LPAREN let_section s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN let_section s_expr RPAREN\n");
        $$ = attachLetSection($2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN
    {
    fprintf(stderr, "yacc: s_expr ::= LPAREN let_section s_expr RPAREN\n");
            $$ = createConditionNode($3, $4, $5);
    }
    | QUIT {
        fprintf(stderr, "yacc: s_expr ::= QUIT\n");
        exit(EXIT_SUCCESS);
    }
    | error {
        fprintf(stderr, "yacc: s_expr ::= error\n");
        yyerror("unexpected token");
        $$ = NULL;
    };

number:
    INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($1, INT_TYPE);
    }
    | TYPE_INT INT
    {
    	fprintf(stderr, "yacc: number ::= TYPE_INT INT\n");
        $$ = createNumberNode($2, INT_TYPE);
    }
    | TYPE_DOUBLE INT
    {
    	fprintf(stderr, "yacc: number ::= TYPE_DOUBLE INT\n");
        $$ = createNumberNode($2, DOUBLE_TYPE);
    }
    | TYPE_INT DOUBLE
    {
       	fprintf(stderr, "yacc: number ::= TYPE_INT DOUBLE\n");
         $$ = createNumberNode($2, INT_TYPE);
    }
    | TYPE_DOUBLE DOUBLE
    {
        fprintf(stderr, "yacc: number ::= TYPE_DOUBLE DOUBLE\n");
        $$ = createNumberNode($2, DOUBLE_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

let_section:
    LPAREN LET let_list RPAREN{
        fprintf(stderr, "yacc: let_section ::= LPAREN LET let_list RPAREN\n");
        $$ = $3;
	}
	| {
		$$ = NULL;
	};

let_list:
    let_elem {
        fprintf(stderr, "yacc: let_list ::= let_elem\n");
        $$ = $1;
    }
    | let_list let_elem {
        fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
        $$ = createLetList($1, $2);
    };

let_elem:
    LPAREN SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($2, $3, NULL);
    }
    | LPAREN TYPE_INT SYMBOL s_expr RPAREN {
    	fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($3, $4, INT_TYPE);
    }
    | LPAREN TYPE_DOUBLE SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE SYMBOL s_expr RPAREN\n");
        $$ = createSymbolTableNode($3, $4, DOUBLE_TYPE);
     };

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
        fprintf(stderr, "yacc: f_expr ::= LPAREN FUNC s_expr_list RPAREN\n");
        $$ = createFunctionNode($2, $3);
    };

s_expr_list:
	s_expr s_expr_list {
	 fprintf(stderr, "yacc: s_expr_list ::= s_expr s_expr_list\n");
                $$ = attachExprLists($1, $2);
	}
	| s_expr {
	 fprintf(stderr, "yacc: s_expr_list ::= s_expr\n");
                        $$ = $1;
	}
	| /*empty*/ {
		$$ = NULL;
	};


%%