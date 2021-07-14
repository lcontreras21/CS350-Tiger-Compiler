%{

/* In this first section of the lex file (between %{ and %}),
   we define C++ functions that will be used later in the actions of part 3 */
   
 // Dave's list of useful references for flex and bison for C++:
 //  http://www.jonathanbeard.io/tutorials/FlexBisonC++
 //  http://dinosaur.compilertools.net/flex/flex_19.html
 //  https://www.gnu.org/software/bison/manual/html_node/Split-Symbols.html

#include <stdlib.h>
#include "tigerParseDriver.h"
#include "tiger-grammar.tab.hh"

// next line from https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Scanner.html#Calc_002b_002b-Scanner
static yy::location loc;

// The function below is somewhat overly verbose;
//  it is designed to serve as an example of
//  (a) calling a C function to process something from lex (see the "INT" pattern below), and
//  (b) processing a collection of characters one at a time, relying on their ASCII values

static int textToInt(std::string the_text)  // a C-style array of char will be converted to a String
{
	// here's a C-language way of doing this
	char zero = '0';  // the character 0
	char nine = '9';
	int result = 0;

	// the length() method of class string returns an "unsigned int", so make i the same type
	for (unsigned int i=0; i<the_text.length(); i++) {
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

/*
static std::string parseString(std::string the_text)
{
	// Iterate through the string. If it reaches a \ then keep adding to it and convert to special char, then back to std::string
	// Possible escape sequences:
	// \n newline
	// \t tab
	// ^c control character c, for any appropriate c
	// \ddd single char with ascii code ddd (3 decimal digit)
	// \" escaped quote "
	// \\ backslash char
	// \f__f\ sequence is ignored. f__f can include any escape sequence

	// Tracking variables
	std::string string_to_return = "";	
	bool found_escape_char = false;
	std::string escape_string_so_far = "";

	for (int i=1; i<the_text.length()-1; i++)
	{	
		std::string escape_char = "";
		// First and last are quotes. Drop them in for loop bounds
		char letter = the_text[i];
		if (letter == "\\" and !found_escape_char) {
			found_escape_char = true;
		}
		if (found_escape_char) {
			if (letter == "n") {
				// Found Newline
				found_escape_char = false;
				escape_string_so_far = "";
				to_return++ "\n";
			} else if (letter = "t") {
				// Found tab
				found_escape_char = false;
				escape_string_so_far = "";
				to_return++ "\t";
			} else if (letter >= '0' and letter <= '9') {
				// Found ascii code
				if (escape_string_so_far.length() == 3) {
					found_escape_char = false;
					escape_string_so_far = "";
				} else {
					escape_string_so_far++ letter;
				}
		}	
	return string_to_return;

{string}			{ loc.step(); return yy::tigerParser::make_STRING(parseString(yytext), loc);	}
}*/

// This uses some stuff created by flex, so it's easiest to just put it here.
int tigerParseDriver::parse (const std::string &f)
{
	fileName = f;

	if (fileName == "" || fileName == "-") {
		yyin = stdin;
	} else if (!(yyin = fopen (fileName.c_str (), "r"))) {
		error ("cannot open " + fileName + ".");
		exit (EXIT_FAILURE);
	}

	yy::tigerParser parser (*this);
	int res = parser.parse ();  // sets this->AST_root

	fclose (yyin);
	return res;
}


%}

/* In this second section of the lex file (after the %}),
   we can define variables in terms of regular expressions.
   A few other things, like %option, can also be used here.
   C-style comments (like this one) are also legal. */

/* options from the example */
%option noyywrap nounput
/* Not using these: batch debug noinput */

integer	[0-9]+

/* real numbers don't occur in tiger, but if they did,
   and we always wanted a "." with at least one numeral preceding it,
   we could do this: */
real	[0-9]+\.[0-9]*(e-?[0-9]+)?

/*Identifiers: An identifier is a sequence of letters, digits, and underscores, starting with a letter. Uppercase letters are distinguished from lowercase. In this
appendix the symbol id stands for an identifier. FROM APPEL APPENDIX. */
identifier	[a-zA-Z]+[_a-zA-Z0-9]* 

string	\"[^"]*\" 
/* " */

c_comment \/\*([^*]|\*\**[^*\/])*\*\**\/


/* In the third section of the lex file (after the %%),
   we can define the patterns for each token
   in terms of regular expressions and the variables above,
   and give the action (as C++ code) for each token.
   Comments are legal only inside the actions. */

/* Inclusive start conditions */
/*%s*/ 
/* Exclusive start conditions */
%x COMMENT str
%%

%{
/* Surrounding four lines, and other things involving "loc", are from
      https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Scanner.html#Calc_002b_002b-Scanner */
  // Code run each time yylex is called.
  loc.step();
%}
	int num_comments = 0;
	std::string to_return = "";

[ \t]	{ loc.step(); }
[\n\r]	{ loc.lines(yyleng); loc.step(); }


\+					{ loc.step(); return yy::tigerParser::make_PLUS(loc);			}
\-					{ loc.step(); return yy::tigerParser::make_MINUS(loc);			}
\*					{ loc.step(); return yy::tigerParser::make_TIMES(loc);			}
"("					{ loc.step(); return yy::tigerParser::make_LPAREN(loc);			}
")"					{ loc.step(); return yy::tigerParser::make_RPAREN(loc);			} 
{identifier}		{ loc.step(); return yy::tigerParser::make_ID(yytext, loc);		}
{integer}			{ loc.step(); return yy::tigerParser::make_INT(textToInt(yytext), loc);
   /* textToInt is defined above */
   /* make_INT, make_END from example at https://www.gnu.org/software/bison/manual/html_node/Complete-Symbols.html#Complete-Symbols */	  
   }

"\""				{ /* Initial quote of string */ 
					  to_return = "";
					  BEGIN(str); }
<str>{
"\""				{ /* Closing quote of string */
					  BEGIN(INITIAL); 
					  loc.step(); 
					  return yy::tigerParser::make_STRING(repr(to_return), loc);	}
\\n					{ to_return = to_return + '\n'; }
\\t					{ to_return = to_return + '\t'; }
\\\"				{ to_return = to_return + '\"'; }
\\					{ to_return = to_return + '\\'; }
\\[ \t\n]*\\		{ /* Special thing defined by Appel */ }
[^\\\n\"]+			{ /* Any other part of the string */
					  to_return = to_return + yytext;
					}
}



"/*"				{BEGIN(COMMENT); ++num_comments;								}
<COMMENT>{
[^*/]*				{/* Match text that does not have * or / */						}
"*"+[^*/]*			{/* Match text that has * not followed by / */					}
"/"+[^*/]*			{/* Match text that has / not followed by * */					}
"/*"				{/* Encounter a start comment, may be nested */ ++num_comments;	}
"*"+"/"				{/* Encounter a closed comment, may be nested */
					--num_comments;
					if (num_comments < 0) { EM_error("Illegal nested comments");	}
					else if (num_comments == 0) {BEGIN(INITIAL);					}
					}
}


\<[Ee][Oo][Ff]\>		{ return yy::tigerParser::make_END(loc); /* this RE matches the literal five characters <EOF>, regardless of upper/lower case   */ }
<<EOF>>					{ return yy::tigerParser::make_END(loc); /* <<EOF>> is a flex built-in for an actual end of a file, when there is no more input */ }
.	{ string it = "?"; it[0] = yytext[0]; EM_error("illegal token: " + it); }
%%
