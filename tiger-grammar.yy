// Note: to learn how to invoke bison/flex in Eclipse, see https://www.eclipse.org/forums/index.php/t/1075215/
// And refer to the example at https://www.gnu.org/software/bison/manual/html_node/A-Complete-C_002b_002b-Example.html#A-Complete-C_002b_002b-Example

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define api.parser.class {tigerParser}

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
// Thanks to Ian Fisher HC '19 for explaining the details of the parts I'd failed to get working


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
  PLUS MINUS TIMES DIVIDE ASSIGN EQ NEQ LT LE GT GE OR AND NOT
  UMINUS TRUE FALSE
;

/* precedence (stickiness) ... put the stickiest stuff at the bottom of the list */
/* https://stackoverflow.com/questions/12731922/reforming-the-grammar-to-remove-shift-reduce-conflict-in-if-then-else Precence for THEN ELSE */ 
%left OR
%left AND
%nonassoc GE LE EQ NEQ LT GT THEN
%nonassoc ELSE 
%left PLUS MINUS 
%left TIMES DIVIDE 
%left UMINUS

/* Attributes types for nonterminals are next, e.g. struct's from tigerParseDriver.h */
%type <expAttrs>  exp
%type <expListAttrs> expList


// The line below means our grammar must not have conflicts
//  (no conflicts means it is "an LALR(1) grammar",
//   meaning it must be unambiguous and have some other properties).
%expect 0


%%

%start program;
program: exp[main]	{ EM_debug("Got the main expression of our tiger program.", $main.AST->pos());
		 			  driver.AST = new A_root_($main.AST);
		 			}
	;

exp:  INT[i]					{ $$.AST = A_IntExp(Position::fromLex(@i), $i);
								  EM_debug("Got int " + str($i), $$.AST->pos());
								}
	| STRING[str1]				{ $$.AST = A_StringExp(Position::fromLex(@str1), $str1);
								  EM_debug("Got string: " + $str1, $$.AST->pos());
								}
	| exp[exp1] PLUS exp[exp2]	{ $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
												   A_plusOp,  $exp1.AST,$exp2.AST);
								  EM_debug("Got plus expression.", $$.AST->pos());
								}
	| exp[exp1] MINUS exp[exp2] { $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
												   A_minusOp, $exp1.AST, $exp2.AST);
								  EM_debug("Got minus expression.", $$.AST->pos());
								}
	| exp[exp1] TIMES exp[exp2]	{ $$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
												   A_timesOp, $exp1.AST, $exp2.AST);
								  EM_debug("Got times expression.", $$.AST->pos());
								}
	| exp[exp1] DIVIDE exp[exp2]{ $$.AST = A_CallExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()), to_Symbol("DIV"), A_ExpList($exp1.AST, A_ExpList($exp2.AST, 0)));

								  EM_debug("Got divide expression.", $$.AST->pos());
								}
	| MINUS exp[exp1] %prec UMINUS			{ $$.AST = A_OpExp($exp1.AST->pos(), A_timesOp, A_IntExp(Position::fromLex(@exp1), -1), $exp1.AST); 
								  EM_debug("Got Unary Negation expression.", $$.AST->pos());
								}
	| LPAREN exp[exp1] RPAREN	{ $$.AST = $exp1.AST; 
								}
	
	| ID[id1] LPAREN expList[list1] RPAREN[a]{ $$.AST = A_CallExp(Position::range(Position::fromLex(@id1), Position::fromLex(@a)), to_Symbol($id1), $list1.AST);
								  EM_debug("Got function call", $$.AST->pos()); 
								}
	| TRUE[t]					{ $$.AST = A_BoolExp(Position::fromLex(@t), true);
								  EM_debug("Got true boolean expression", $$.AST->pos());
								}
	| FALSE[f]					{ $$.AST = A_BoolExp(Position::fromLex(@f), false);
								  EM_debug("Got false boolean expression", $$.AST->pos());
								}
	| IF[if] exp[exp1] THEN exp[exp2] ELSE exp[exp3] {
								  $$.AST = A_IfExp(Position::range(Position::fromLex(@if), $exp3.AST->pos()), $exp1.AST, $exp2.AST, $exp3.AST);
								  EM_debug("Got if/then/else expression", $$.AST->pos());
								}
	| IF[if] exp[exp1] THEN exp[exp2] {
								  $$.AST = A_IfExp(Position::range(Position::fromLex(@if), $exp2.AST->pos()), $exp1.AST, $exp2.AST, 0);
								  EM_debug("Got if/then/ expression", $$.AST->pos());
//
// Note: In older compiler tools, instead of writing $exp1 and $exp2, we'd write $1 and $3,
//        to refer to the first and third elements on the right-hand-side of the production.
//        We can still use this notation (and note that Appel uses it in his book), e.g. the above is like
//					$$.AST = A_OpExp(Position::range($exp1.AST->pos(), $exp2.AST->pos()),
//									 A_timesOp, $1.AST,$3.AST);
// Also: Bison's location system produces information about nonterminals as well as terminals,
//        so we could use @exp1 to get it's information about the locations of exp1
//        writing, e.g., Position::fromLex(@exp1) or instead of $exp1.AST->pos()
//
			  					}
	;
expList: exp[exp1]						{ $$.AST = A_ExpList($exp1.AST, 0);
										  EM_debug("Got end of expList  expression.", $$.AST->pos());
										}
	| exp[exp1] COLON expList[list1]	{ $$.AST = A_ExpList($exp1.AST, $list1.AST); 
										  EM_debug("Got expList  expression.", $$.AST->pos());
										} 
	|									{ $$.AST = 0;
										  EM_debug("Got empty expList  expression");
										}
	;

%%

void
yy::tigerParser::error(const location_type& l,
          	       const std::string& m)
  {
	  EM_debug("In yy::tigerParser::error");
	  EM_error(m, true, Position::fromLex(l));
  }
