#include <stdio.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "errormsg.h"
#include "AST.h"
#include "ST.h"  /* to run ST_test */
#include "tigerParseDriver.h"

/* Turned this off while having trouble switching to C++ approach; this used to work in C version */
#if defined COMPILE_LEX_TEST
// array of token number / token name pairs
static struct {
	int number;
	const char *name;
} toknames[] = {
	{ID, "ID"},
	{STRING, "STRING"},
	{INT, "INT"},
	{COMMA, "COMMA"},
	{COLON, "COLON"},
	{SEMICOLON, "SEMICOLON"},
	{LPAREN, "LPAREN"},
	{RPAREN, "RPAREN"},
	{L_SQUARE_BRACKET, "L_SQUARE_BRACKET"},
	{R_SQUARE_BRACKET, "R_SQUARE_BRACKET"},
	{L_CURLY_BRACE, "L_CURLY_BRACE"},
	{R_CURLY_BRACE, "R_CURLY_BRACE"},
	{DOT, "DOT"},
	{PLUS, "PLUS"},
	{MINUS, "MINUS"},
	{TIMES, "TIMES"},
	{DIVIDE, "DIVIDE"},
	{EQ, "EQ"},
	{NEQ, "NEQ"},
	{LT, "LT"},
	{LE, "LE"},
	{GT, "GT"},
	{GE, "GE"},
	{AND, "AND"},
	{OR, "OR"},
	{ASSIGN, "ASSIGN"},
	{ARRAY, "ARRAY"},
	{IF, "IF"},
	{THEN, "THEN"},
	{ELSE, "ELSE"},
	{WHILE, "WHILE"},
	{FOR, "FOR"},
	{TO, "TO"},
	{DO, "DO"},
	{LET, "LET"},
	{IN, "IN"},
	{END, "END"},
	{OF, "OF"},
	{BREAK, "BREAK"},
	{NIL, "NIL"},
	{FUNCTION, "FUNCTION"},
	{VAR, "VAR"},
	{TYPE, "TYPE"}
};

// function to convert token numbers to printable names
String tokname(int tok) {
	for (unsigned int i = 0; i < sizeof(toknames)/sizeof(*toknames); i++)
	{
		if (toknames[i].number == tok)
			return toknames[i].name;
	}
	return "BAD_TOKEN";
}


extern YYSTYPE yylval;  // global variable set by lexical analyzer

void lex_test()
{
	extern int yylex(void); /* prototype for the lexing function, C style with no parameters */

	int tok;	// token number

	while( (tok=yylex()) != 0)
	{
		cout << tokname(tok) << " (token # " << tok << ")";
		switch(tok)
		{
		case ID:
			cout << " \tvalue is: " << *(yylval.str_attrs.sval) << endl;
			break;
		case STRING:
			cout << " \tvalue in printable form is: " << repr(*(yylval.str_attrs.sval)) <<
				" and in raw form is: " << *(yylval.str_attrs.sval) << endl;
			break;
		case INT:
			cout << " \tvalue is: " << yylval.int_attrs.ival << endl;
			break;
		default:
			cout << endl;
		}
	}
}
#endif

int main(int argc, char **argv)
{
  try {
	bool debug = false, show_ast = false, crash_on_fatal;
#if defined COMPILE_LEX_TEST
	bool just_do_lex_and_then_stop = false;
#endif
	String filename;
	int arg_consumed = 0;
  
	if (argc>arg_consumed+1 && string(argv[1]).length()>= 2 && (argv[1][0] == '-' && argv[1][1] == 'd')) { // Debug option
		arg_consumed++;
		debug = true;
		if (string(argv[1]).length()>= 3 && argv[1][2] == 'a')
			show_ast = true;
		else if (string(argv[1]).length()>= 3 && argv[1][2] == 'A')
			print_ASTs_with_attributes = show_ast = true;
		else if (string(argv[1]).length()>= 3 && argv[1][2] == 'c')
			crash_on_fatal = true;
#if defined COMPILE_LEX_TEST
		else if (string(argv[1]).length()>= 3 && argv[1][2] == 'l')
			just_do_lex_and_then_stop = true;
#endif
	}


	if (argc>arg_consumed+1)
	{
		arg_consumed++;
		filename = argv[arg_consumed];
	}
	else
	{
		cout << "Enter name of file to be compiled (or - for standard input) ";
		// read from stdin to avoid confusing stdin/cin
		char buf[1024];
		scanf("%s", buf);
		filename = buf;
	}

	// open "filename" for reading by lexical scanner,
	// give up after 8 errors,
	// with compiler debugging ON if the "-d" flag was used when we started
	EM_reset(filename, 8, debug, crash_on_fatal);

	ST_test();  // internal consistency check

#if defined COMPILE_LEX_TEST
	if (just_do_lex_and_then_stop) {
		lex_test();
		return 0;
	} else
#endif
	{
		tigerParseDriver driver;
		int result = driver.parse(filename);
		if (!EM_recorded_any_errors()) {
			if (result != 0) {
				EM_error("Strange result in tiger.cc: parser failed but EM module reported no errors",
					 true, Position::undefined()); // true = fatal error
			}

			EM_debug("Parsing Successful", driver.AST->pos());


			// Could do static checks, e.g. type checking, here if we want to do them all before any code generation

			if (show_ast) cerr << "Printing AST due to -da or -dA flag:" << endl << repr(driver.AST) << endl;

			if (! EM_recorded_any_errors()) {
				String code = driver.AST->HERA_code();
				if (! EM_recorded_any_errors()) {
					cout << code;
					return 0; // no errors
				}
			}
		}
		EM_warning("Not generating HERA code due to above errors.");
		return EM_recorded_any_errors(); // got errors somewhere, or would have returned 0 above
	}

  } catch (const char *message) {
	  cerr << "Compiler exception (this should not happen): " << message << endl;
	  return 4;
  } catch (std::string message) {
	  cerr << "Compiler exception (this should not happen): " << message << endl;
	  return 4;
  } catch (...) {
	  cerr << "Yikes! Uncaught compiler exception (this REALLY should not happen)" << endl;
	  return 66;
  }
}
