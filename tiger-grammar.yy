// Note: to learn how to invoke bison/flex in Eclipse, see https://www.eclipse.org/forums/index.php/t/1075215/
// And refer to the example at https://www.gnu.org/software/bison/manual/html_node/A-Complete-C_002b_002b-Example.html#A-Complete-C_002b_002b-Example

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {tigerParser}

/* next two from https://www.gnu.org/software/bison/manual/html_node/Complete-Symbols.html#Complete-Symbols */
%define api.value.type variant
%define api.token.constructor

%code requires
{
class tigerParseDriver;
}
%param { tigerParseDriver& driver }

// According to the Example, this turns on "location tracking"
%locations


%{
#include "tigerParseDriver.h"
%}

%token END  0
%token <bool> BOOL
%token <int>  INT
%token <std::string> ID STRING

// NOTE that bison complains if you have the same symbol listed as %token (above) and %type (below)
//      so if you want to add attributes to a token, remove it from the list below

%token 
  COMMA COLON SEMICOLON LPAREN RPAREN L_SQUARE_BRACKET R_SQUARE_BRACKET 
  L_CURLY_BRACE R_CURLY_BRACE
  ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END_LET OF 
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


%%

%start program;
program: exp	{ EM_debug("Got one expression.");
		  		  driver.AST = new A_root_($1); }
	;

exp:  INT			{ EM_debug("Got int " + str($1));
					  $$ = A_IntExp(Position::current(), $1); }
	| exp PLUS exp	{ EM_debug("Got plus expression.");
					  $$ = A_OpExp(/* */ Position::range($1->pos(), $3->pos()),
					  			   A_plusOp,$1,$3); }
	| exp TIMES exp	{ EM_debug("Got times expression.");
					  $$ = A_OpExp(Position::range($1->pos(), $3->pos()),
					  			   A_timesOp,$1,$3); }

	;

%%

void
yy::tigerParser::error(const location_type& l,
          	       const std::string& m)
  {
	  EM_error(m, Position::current());  // ToDo: convert flex locations into tiger Positions
  }
