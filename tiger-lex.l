%{
/* In this first section of the lex file (between %{ and %}),
   we define C++ functions that will be used later in the actions of part 3 */

#include <stdlib.h>
/* The .cc file built from this .l will be in the "Debug" folder, so we need "../" to include our project's headers */
#include "../util.h"
#include "../AST.h"
#include "y.tab.h"

// These let the lexical scanner update the variables that are needed for the errormsg.c files
// Notify error message system when we see a newline in input
void EM_newline(void);
void EM_set_currentPos(int new_pos);


// The function below is somewhat overly verbose;
//  it is designed to serve as an example of
//  (a) calling a C function to process something from lex (see the "INT" pattern below), and
//  (b) processing a collection of characters one at a time, relying on their ASCII values

static int text_to_integer(String the_text)  // a C-style array of char will be converted to a String
{
	// here's a C-language way of doing this
	char zero = '0';  // the character 0
	char nine = '9';
	int result = 0;
	for (int i=0; i<the_text.length(); i++) {
		// the_text[i] is the i'th numeral, e.g. a '4' or a '2'
		// We need to convert this to a number, such as 4 or 2,
		//  and rely on the fact that the ASCII character set
		//  has '0', then '1', then '2', etc. in numeric order.
		// So '0'-'0' gives us 0, and '1'-'0' gives us 1, etc.
		// Start by saving the current character in a variable:
		char numeral = the_text[i];
		
		// We should get here *only* if we have only numerals,
		//  since we're called only for the pattern {digit}+ below.
		//
		assert (numeral >= zero and numeral <= nine);
		
		// now convert the numeral
		int numeral_value = numeral-zero;
		
		// and update the total value
		result = 10*result+numeral_value;
	}
	return result;
	
	// alternate implementation: use hc_string's to_int function:
	// return to_int(the_text);
}


/* The following have to do with error and input processing */

int charPos=1;

extern "C"
int yywrap(void)
{
 charPos=1;
 return 1;
}

void adjust(void)
{
 EM_set_currentPos(charPos);
 charPos+=yyleng;
}

%}

/* In this second section of the lex file (after the %}),
   we can define variables in terms of regular expressions.
   C-style comments (like this one) are also legal. */

integer	[0-9]+

/* real numbers don't occur in tiger, but if they did,
   and we always wanted a "." with at least one numeral preceding it,
   we could do this: */
real	[0-9]+\.[0-9]*(e-?[0-9]+)?




/* In the third section of the lex file (after the %%),
   we can define the patterns for each token
   in terms of regular expressions and the variables above,
   and give the action (as C++ code) for each token.
   Comments are legal only inside the actions. */

%%


[ \t]		{ adjust(); continue; }
[\n\r]		{ adjust(); EM_newline(); continue; }  /* allow MacOS \r as well as \n  -- may double-count in MS Windows */


"+"			{ adjust(); return PLUS; }
"*"			{ adjust(); return TIMES; }

{integer}		{
				  adjust();
				  yylval.int_attrs.ival = text_to_integer(yytext);  /* Function defined above */
				  return INT;
				}

"<"[Ee][Oo][Ff]">"		{ return 0; /* Fake end-of-input for debugging in Eclipse */ }

.			{ adjust(); String it = ""; appendchar(it, yytext[0]); EM_error("illegal token: " + it); }
