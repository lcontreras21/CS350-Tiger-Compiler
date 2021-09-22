#if ! defined TIGER_PARSE_DRIVER_H
#define TIGER_PARSE_DRIVER_H

#include "AST.h"
// Define the types of the attributes of various kinds of nodes in the parse tree
struct expAttrs {
		Ty_ty type;
		A_exp AST;
};

struct lvalueAttrs {
		A_exp AST;
};

struct expListAttrs {
		Ty_ty type;
		A_expList AST;
};

struct seqExpAttrs {
		Ty_ty type;
		A_expList AST;
};

struct decListAttrs {
		A_decList AST;
};

struct decAttrs {
		A_dec AST;
};

#include "tiger-grammar.tab.hh"


// The tigerParseDriver class is closely based on the example from the documentation (i.e., copied and edited)
// https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parsing-Driver.html#Calc_002b_002b-Parsing-Driver
class tigerParseDriver {
public:
	tigerParseDriver();

	A_root_ *AST;  // parsing will set this to the root of the AST, if it succeeds

	// Run the parser on file f, return 0 on success.
	int parse (const std::string& f);
	// The name of the file being parsed.
	// Used later to pass the file name to the location tracker.
	std::string fileName;

	// Error handling.
	void error (const yy::location& l, const std::string& m);
	void error (const std::string& m);
};

// Tell Flex the lexer's prototype ...
# define YY_DECL \
	yy::tigerParser::symbol_type yylex (tigerParseDriver &)
// ... and declare it for the parser's sake.
YY_DECL;



#endif
