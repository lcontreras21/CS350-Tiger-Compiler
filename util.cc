/*
 * util.c - commonly used utility functions.

 To run the example/test of string printing, use the command
	g++ -std=c++0x  -D UTIL_EXAMPLES_IS_MAIN=1 util.cc -o util_examples && ./util_examples

*/

#include <string>
#include "util.h"


// Given a string s, what could we enter into a C++ (or HERA-C) file to put s into a program?
//
// Remember that in C/C++, unlike Python, an element of a string is a
//   *character*, not a one-character string.
// In C/C++, apostrophes (single-quotes) indicate characters,
//   while standard double-quotes indicate strings.
// So 'c' is the character c,
//    "c" is a string of length 1
//    "c++" is a string of length 3, and
//    'c++' is something you probably should not use.

string repr(string   s)
{
	string rep = "\"";
	string hexdigits = "0123456789ABCDEF";
	for (unsigned int i=0; i<s.length(); i++) {
		if (0 > s[i] || s[i] > 0x7F) { // non-ASCII
			// for now, do nothing
		} else if (32 <= s[i] && s[i] <= 0x7E) { // print these as they are
			rep.append(1, s[i]);
		} else {
			rep.append(1, '\\');  // that's one backslash character, of course
			rep.append(1, hexdigits[s[i] / 16]);
			rep.append(1, hexdigits[s[i] % 16]);
		}
	}
	rep.append(1, '\"');
	return rep;
}


// useful for testing/demonstrating repr vs. plain strings
string util_example_string_chars_1_through_7f()
{
	string example = "";
	example.append(0x7f, 'x');
	for (char c=1; c<=0x7e; c++) { // stop at 7e to avoid infinite loop
		example[c-1] = c;
	}
	example[0x7e] = 0x7f;
	return example;
}


#if defined UTIL_EXAMPLES_IS_MAIN && UTIL_EXAMPLES_IS_MAIN
#include <iostream>
using namespace std;
int main(int argc, char *argv[])
{
	string example = util_example_string_chars_1_through_7f();
	cout << "example.length() and repr(example) are: " << example.length() << " " << repr(example) << endl;
	cout << "what happens if we just cout example?   " << example << endl;
}
#endif
