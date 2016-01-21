/*
 * util.c - commonly used utility functions.
 */

#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <iostream>
using namespace std;

void *checked_malloc(int len)
{
	void *p = malloc(len);
	if (!p)	error("Ran out of memory!");
 
	return p;
}


U_boolList U_BoolList(bool head, U_boolList tail)
{
	U_boolList list = (U_boolList) checked_malloc(sizeof(*list));
	list->head = head;
	list->tail = tail;
	return list;
}

void error(const char *s)
{
	cerr << endl << "Error: " << s << endl;
	if (getenv("CS350_ABORT_ON_ERROR"))
		abort();
	else
		exit(1);
}

// Some string-processing functions follow.
//
// Remember that in C/C++, unlike Python, an element of a string is a
//   *character*, not a one-character string.
// In C/C++, apostrophes (single-quotes) indicate characters,
//   while standard double-quotes indicate strings.
// So 'c' is the character c,
//    "c" is a string of length 1
//    "c++" is a string of length 3, and
//    'c++' is something you probably should not use.
//
// A C-style string of length n is an n+1 element array of non-zero characters
//  followed by a character number 0 (NUL), so
//  "c" is a two-character array with 'c' and character 0 in it.
// Note that a character variable can be used as or set to an integer value,
//  which just treats each character as its number in the character set of
//  the hardware.
// So, for example, the appendchar function below builds a C-style string
//  from a character value and then adds it to a String.
//
// C-style strings can be automatically converted to String from hc_string.h
// 

void appendchar(String &s, char c)
{
  char byte_me[2];
  byte_me[0] = c;
  byte_me[1] = 0;
  s+=String(byte_me);
}


// Use "appendchar" above and "isprint" from ctype.h
String string_as_tiger_source_code(const String &s)
{
  String result;
  for (unsigned int i = 0; i<s.length(); i++) {
    char c = s[i];
    if (isprint(c) and c != '\\' and c != '\"') {
      appendchar(result, c);
    } else if (c <= 127) {
      appendchar(result, '\\');
      switch(c) {
      case '\t':
	appendchar(result, 't');
	break;
      case '\n':
	appendchar(result, 'n');
	break;
      case '\\':
	appendchar(result, '\\');  // note '\\' is a single backslash character
	break;
      case '\"':
	appendchar(result, '"');
	break;
      default:
	char tiger_char_base = 10;
	char d2 = c/(tiger_char_base*tiger_char_base);
	char d1 = (c/tiger_char_base)%tiger_char_base;
	char d0 = c%tiger_char_base;
	assert(d2 >= 0 && d2 < tiger_char_base);
	assert(d1 >= 0 && d1 < tiger_char_base);
	assert(d0 >= 0 && d0 < tiger_char_base);

	appendchar(result, d2+'0');  // e.g. '7' = 7+'0'
	appendchar(result, d1+'0');
	appendchar(result, d0+'0');
	break;
      }
    } else {
      // for now, just ignore this non-ASCII character
    }
  }
  return result;
}

static int test_string_as_tiger_source_code_once(const String &q, const String &a)
{
  if (string_as_tiger_source_code(q) != a) {
    cerr << "WARNING: internal compiler inconsistency: "
	 << "string_as_tiger_source_code(" << q << ") produces "
	 << string_as_tiger_source_code(q) << " rather than the desired "
	 << a << " :-(" << endl;
    return 1;
  } else {
    return 0;
  }
}

bool test_string_as_tiger_source_code()
{
  String char_seven;
  String char_seventeen;
  appendchar(char_seven, 7);  // whatever character 7 happens to be
  appendchar(char_seventeen, 17);  // whatever character 17 happens to be
  int errors =
    test_string_as_tiger_source_code_once("a simple test", "a simple test") +
    // be careful thinking about backslashes below, e.g.
    //  "\n" in C++ is a length-one string containing a newline;
    //  "\\n" is a length-two string with one backslash and an 'n'.
    test_string_as_tiger_source_code_once("\n", "\\n") +
    test_string_as_tiger_source_code_once("\t", "\\t") +
    test_string_as_tiger_source_code_once("\\", "\\\\") +  // whoa
    test_string_as_tiger_source_code_once("\"", "\\\"") +
    test_string_as_tiger_source_code_once(char_seven, "\\007") +
    test_string_as_tiger_source_code_once(char_seventeen, "\\017") +
    test_string_as_tiger_source_code_once("a bigger\ttest\n  on \\ \"two\"\tlines.\n",
					  "a bigger\\ttest\\n  on \\\\ \\\"two\\\"\\tlines.\\n");

  /* remove this and the matching star-slash below to have it print an example

  // Build an example from 04-lex-strings-advanced.tig
  // Initially, most of this is the same as it would be in C++,
  //  e.g. \t is tab in C++ as well as in tiger,
  //  except that tiger uses the backslash-return (in the .tig file)
  //  to let a long string be stretched across multiple lines,
  //  but in C++ I've used string concatenation with "+"
  //  (there is also a C=language mechanism for this).
  // Some of the later things like \^i for control-i, are not
  //  easily reproduced in C++, so I'll put those on with appendchar.
  String example_from_04_lex_strings_advanced =
	  // The call to "String" below makes sure we have a C++ String,
	  //  rather than a C-style character array, which can't be concatenated with +
	  ( String("Finally, here is a third string that has _everything_ in it, ") +
	    "including multiple lines,  " +
	    "embedded\ttab\tcharacters, " +
	    "many weird characters such as !#$%&'()*+,-./:;<=>?@[]^_`{|}~ and " +
	    "both kinds of quote \" characters \" \\ in this \\ part");


  // the next part is:
  //   " as well as\^i control i (tab) \^i "
  // Getting a "control-i" (a "\^i" in tiger) is a bit tricky;
  //  the ctype.h header file has "toupper" and "tolower" to make upper or lower-case letters,
  //  but not corresponding tocontrol.
  // Traditionally this is done by turning off certain binary digits in the binary
  //  representation of the character code, using C++'s "bitwise and" operator, "&".
  //  Turning off everything at/above the 32's place (leaving the 16's place, 8's place, etc.) does this.
  
  char bits_for_control_character = 16+8+4+2+1;
  char control_i = 'i' & bits_for_control_character;

  example_from_04_lex_strings_advanced += " as well as ";
  appendchar(example_from_04_lex_strings_advanced, control_i);
  example_from_04_lex_strings_advanced += " control i (tab) ";
  appendchar(example_from_04_lex_strings_advanced, control_i);
  appendchar(example_from_04_lex_strings_advanced, ' ');

  // and now "ASCII code nine (\009) (a tab?) and 103 (\103) (a lower case g)	"
  // turning numbers into characters is easy, especially if we're sending character values to appendchar:
  example_from_04_lex_strings_advanced += "ASCII code nine (";
  appendchar(example_from_04_lex_strings_advanced, 9);  // be careful NOT to put 009 here
  example_from_04_lex_strings_advanced += ") (a tab?) and 103 (";
  appendchar(example_from_04_lex_strings_advanced,103);
  example_from_04_lex_strings_advanced += ") (a lower case g)	";

  // and now the last bit, in C++ just as it was in tiger:
  example_from_04_lex_strings_advanced += " that should test just about all of it\n";

  cout << "Example, as a tiger string, is: \"" << 
	  string_as_tiger_source_code(example_from_04_lex_strings_advanced) << "\"" << endl;
  cout << "The original prints as: " << example_from_04_lex_strings_advanced;
  */

  return errors > 0 ? false : true;
}

// stdio contains the function "sprintf".
#include <stdio.h>

String to_String(double d)
{
	char buffer[128];
	sprintf(buffer, "%g", d);
	buffer[127] = '\0'; // just be sure...
	return buffer;
}


String to_String(bool b)
{
	if (b) return "true"; else return "false";
}

String to_String(const char *c_string)
{
	return String(c_string);
}

String char_to_String(char c)
{
  char byte_me[2];
  byte_me[0] = c;
  byte_me[1] = 0;
  return String(byte_me);
}
