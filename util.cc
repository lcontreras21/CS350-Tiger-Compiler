/*
 * util.c - commonly used utility functions.
 */

#include <stdlib.h>
#include <string.h>
#include "util.h"
#include <iostream>
using namespace std;


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
			rep.append(1, hexdigits[s[i] % 16]);
			rep.append(1, hexdigits[s[i] / 16]);
		}
	}
	rep.append(1, '\"');
	return rep;
}
