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
%token LPAREN RPAREN LET EOL QUIT

%type <astNode> s_expr f_expr number
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
         fprintf(stderr, "yacc: s_expr ::= symbol\n");
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
    | DOUBLE {
        fprintf(stderr, "yacc: number ::= DOUBLE\n");
        $$ = createNumberNode($1, DOUBLE_TYPE);
    };

f_expr:
    LPAREN FUNC s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr RPAREN\n");
        $$ = createFunctionNode($2, $3, NULL);
    }
    | LPAREN FUNC s_expr s_expr RPAREN {
        fprintf(stderr, "yacc: s_expr ::= LPAREN FUNC expr expr RPAREN\n");
        $$ = createFunctionNode($2, $3, $4);
    };

let_section:
    {
    	$$ = NULL;
    }
    | LPAREN let_list RPAREN {
        fprintf(stderr, "yacc: let_section ::= LPAREN let_list RPAREN\n");
//    	fprintf(stderr, "yacc: s_expr ::= LPAREN let_list RPAREN\n");
    	$$ = $2;
    };

let_list:
    LET let_elem {
        fprintf(stderr, "yacc: let_list ::= LET let_elem\n");
//    	fprintf(stderr, "yacc: s_expr ::= LET let_elem\n");
	$$ = $2;
    }
    | let_list let_elem {
        fprintf(stderr, "yacc: let_list ::= let_list let_elem\n");
//    	fprintf(stderr, "yacc: s_expr ::= let_list let_elem\n");
    	$$ = addSymbolToList($1, $2);
    };

let_elem:
    LPAREN SYMBOL s_expr RPAREN {
        fprintf(stderr, "yacc: let_elem ::= LPAREN SYMBOL s_expr RPAREN \n");
//    	fprintf(stderr, "yacc: s_expr ::= LPAREN LET let_list RPAREN\n");
    	$$ = createSymbol($2, $3);
    };
%%
