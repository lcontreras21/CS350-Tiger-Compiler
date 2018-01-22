// Note: to learn how to invoke bison/flex in Eclipse, see https://www.eclipse.org/forums/index.php/t/1075215/
// And refer to the example at https://www.gnu.org/software/bison/manual/html_node/A-Complete-C_002b_002b-Example.html#A-Complete-C_002b_002b-Example


%language "C++"

%defines
%define parser_class_name {tigerParser}

/* next two from https://www.gnu.org/software/bison/manual/html_node/Complete-Symbols.html#Complete-Symbols */
%define api.value.type variant
%define api.token.constructor

%define api.token.prefix {TOK_}

// According to the Example, this turns on "location tracking"
%locations


%{
#include "util.h"
#include "errormsg.h"
#include "PT-types.h"
%}

%token <bool> BOOL
%token <int>  INT
%token <std::string> ID STRING

// NOTE that bison complains if you have the same symbol listed as %token (above) and %type (below)
//      so if you want to add attributes to a token, remove it from the list below

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN L_SQUARE_BRACKET R_SQUARE_BRACKET 
  L_CURLY_BRACE R_CURLY_BRACE
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
  BREAK NIL
  FUNCTION VAR TYPE DOT
  PLUS MINUS TIMES DIVIDE ASSIGN EQ NEQ LT LE GT GE OR AND
;

/* precedence (stickiness) ... put the stickiest stuff at the bottom of the list */

%left PLUS 
%left TIMES

/* Attributes types for nonterminals are next */
%type <A_exp>  exp


// The line below means our grammar must not have conflicts
//  (no conflicts means it must be unambiguous, and some other things too)
%expect 0



%start program

%%

program: exp	{ EM_debug("Got one expression.");
		  		  AST_root = new A_root_($1.AST);  }
	;

exp:  INT			{ $$ /* .type = Ty_Int();
		  			  $$.AST */ = A_IntExp(Position::current(), $1.ival); }
	| exp PLUS exp	{ $$  /* .type = Ty_Int();
					  $$.AST */ = A_OpExp(Position::range($1.AST->pos(), $3.AST->pos()),
					  				   A_plusOp,$1.AST,$3.AST); }
	| exp TIMES exp	{ $$ /* .type = Ty_Int();
					  $$.AST */ = A_OpExp(Position::range($1.AST->pos(), $3.AST->pos()),
					  				   A_timesOp,$1.AST,$3.AST); }

	;

