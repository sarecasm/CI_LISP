%{
    #include "ciLisp.h"
%}

%union {
    double dval;
    char *sval;
    struct ast_node *astNode;
    struct symbol_table_node *symbolNode;
};

%token <sval> FUNC SYMBOL
%token <dval> INT DOUBLE
%token LPAREN RPAREN LET EOL QUIT TYPE_INT TYPE_DOUBLE COND

%type <astNode> s_expr f_expr number s_expr_list
%type <symbolNode> let_elem let_section let_list

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
    	$$ = createSymbolTableNode($2, $3);
    }
    | LPAREN COND s_expr s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN COND s_expr s_expr s_expr RPAREN\n");
        $$ = createCondNode($3, $4, $5);
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
    | TYPE_INT INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($2, INT_TYPE);
    }
    | TYPE_DOUBLE INT {
        fprintf(stderr, "yacc: number ::= INT\n");
        $$ = createNumberNode($2, DOUBLE_TYPE);
    }
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    }
    | TYPE_INT DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($2, INT_TYPE);
    }
    | TYPE_DOUBLE DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($2, DOUBLE_TYPE);
     };

f_expr:
    LPAREN FUNC s_expr_list RPAREN {
    	fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC s_expr_list RPAREN\n");
    	$$ = createFunctionNode($2, $3);
    }
    | LPAREN FUNC RPAREN {
	fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC RPAREN\n");
	$$ = createFunctionNode($2, NULL);
    };

	// TASK 5:
	// s_expr_list ::= s_expr s_expr_list | s_expr | <empty>
s_expr_list:
    s_expr {
    	fprintf(stderr, "yacc: s_expr_list ::= s_expr\n");
    	$$ = $1;
    }
    | s_expr s_expr_list {
    	fprintf(stderr, "yacc: s_expr_list ::= s_expr s_expr_list\n");
    	$$ = s_expr_list($1, $2);
    };

	// TASK 2:
	// let_section ::= <empty> | ( let_list )
let_section:
    /*EMPTY*/ {
    	$$ = NULL;
    }
    | LPAREN let_list RPAREN {
        fprintf(stderr, "yacc: let_section ::= LPAREN let_list RPAREN\n");
    	$$ = $2;
    };

	// let_list ::= let let_elem | let_list let_elem
let_list:
    LET let_elem {
        fprintf(stderr, "yacc: let_list ::= LET let_elem\n");
        $$ = $2;
    }
    | let_list let_elem {
        fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
    	$$ = addSymbolToList($1, $2);
    };

	// let_elem ::= ( symbol s_expr )
	// TASK 3: let_elem ::= ( [type] symbol s_expr )
let_elem:
    LPAREN SYMBOL s_expr RPAREN {
    	fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL s_expr RPAREN \n");
    	$$ = createSymbol(NO_TYPE, $2, $3);
    }
    | LPAREN TYPE_INT SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE_INT SYMBOL s_expr RPAREN \n");
        $$ = createSymbol(INT_TYPE, $3, $4);
    }
    | LPAREN TYPE_DOUBLE SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN TYPE_DOUBLE SYMBOL s_expr RPAREN \n");
        $$ = createSymbol(DOUBLE_TYPE, $3, $4);
    };
%%