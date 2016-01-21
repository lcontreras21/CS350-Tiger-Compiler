%{
/* The .cc file generated from this .y file ends up in the "Debug" folder, so we need to #include from this folder: */

#include "../util.h"
#include "../errormsg.h"
#include "../AST.h"


#include <malloc.h>  // needed by auto-generated stuff on some systems

int yylex(void); /* function declaration, C-style with no parameters */

void yyerror(char *s)
{
 EM_error(s);
}


%}


%union {
	/* The first four of these sets of attributes are for terminals, and are defined in the lexer */
	struct {
		int pos;
	} just_a_pos;
	struct {
		int pos;
		int ival;
	} int_attrs;
	struct {
		int pos;
		int bval;
	} bool_attrs;
	struct {
		int pos;
		Symbol sval; /* no objects w/ constructors in union, so can't use String here */
	} str_attrs;

	/* The remaining sets of attributes are for non-terminals, defined in the grammar below */
	struct {
		Ty_ty type;
		A_exp AST;
	} exp_attrs;
	}

%token <bool_attrs> BOOL
%token <int_attrs> INT
%token <str_attrs> ID STRING

// NOTE that bison complains if you have the same symbol listed as %token (above) and %type (below)
//      so if you want to add attributes to a token, remove it from the list below

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN L_SQUARE_BRACKET R_SQUARE_BRACKET 
  L_CURLY_BRACE R_CURLY_BRACE
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE DOT
  PLUS MINUS TIMES DIVIDE ASSIGN EQ NEQ LT LE GT GE OR AND


/* precedence (stickiness) ... put the stickiest stuff at the bottom of the list */

%left PLUS 
%left TIMES

%type <exp_attrs>  exp

// The line below means our grammar must not have conflicts
//  (no conflicts means it must be unambigous, and some other things too)
%expect 0



%start program

%%


program: exp	{ EM_debug("Got one expression.");
		  AST_root = new A_root_($1.AST);
		}
	;

exp:
/* literals */
	  INT			{ $$.type = Ty_Int();
		  			  $$.AST = A_IntExp(Position::current(), $1.ival); }
	| exp PLUS exp	{ $$.type = Ty_Int();
					  $$.AST = A_OpExp(Position::range($1.AST->pos(), $3.AST->pos()),
					  				   A_plusOp,$1.AST,$3.AST); }
	| exp TIMES exp	{ $$.type = Ty_Int();
					  $$.AST = A_OpExp(Position::range($1.AST->pos(), $3.AST->pos()),
					  				   A_timesOp,$1.AST,$3.AST); }

	;

