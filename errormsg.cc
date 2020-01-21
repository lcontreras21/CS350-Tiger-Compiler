/*
 * errormsg.c - functions used in all phases of the compiler to give
 *              error messages about the Tiger program.
 */

#include <cstdlib>
#include <iostream>
using namespace std;
#include "util.h"
#include "errormsg.h"

static int EM_errCount;
static int EM_maxErrs;
// static ScannerPosition EM_tokPos; not needed with location.hh, I hope...
static bool EM_showingDebug;
static bool EM_crashOnFatal;

static string fileName;
static int lineNum;

typedef struct intList_ {int i; struct intList_ *rest;} *IntList;
static IntList intList(int i, IntList rest) 
{
	IntList l= new intList_;
	l->i=i; l->rest=rest;
	return l;
}

static IntList linePos=NULL;

#if 0  /* cutting this out since it's now in tigerParseDriver */
#if ! defined ERRORMSG_SKIP_LEX
static bool set_lex_input(bool use_stdin, const char *fname);  // set stuff for lex -- see below
#endif
#endif

void EM_reset(string fname, int max_errors, bool show_debug, bool crash_compiler_on_fatal_error)
{
	EM_errCount = 0;
	EM_maxErrs  = max_errors;
	EM_showingDebug = show_debug;
	EM_crashOnFatal = crash_compiler_on_fatal_error;
	//	EM_tokPos = 1;  not needed with location.hh, I hope...
	fileName=fname;
	lineNum=1;
	linePos=intList(0,NULL);
#if 0  /* cutting this out since it's now in tigerParseDriver */
#if ! defined ERRORMSG_SKIP_LEX
	if (!set_lex_input((fname == "-" || fname == ""), fname.c_str())) {
		EM_error("Cannot open file: " + fname, Position::undefined(), true);
	}
#endif
#endif
}

#if ! USING_LOCATION_FROM_BISON
void EM_set_currentPos(ScannerPosition new_pos)
{
	EM_tokPos = new_pos;
}

ScannerPosition  EM_currentScannerPosition()
{
	return EM_tokPos;
}
ScannerPosition  EM_currentPos()
{
	return EM_currentScannerPosition();
}

void EM_newline(void)
{
	lineNum++;
	linePos = intList(EM_tokPos, linePos);
}
#endif

bool EM_recorded_any_errors()
{
	return EM_errCount > 0;
}

#if USING_LOCATION_FROM_BISON
#else
static string EM_intpos_to_string(int position, const string &divider)
{
	if (linePos == 0) {
		return "*** errormsg.cc not initialized; should call EM_reset first ***";
	} else {
		IntList lines = linePos; 
		int num=lineNum;

		while (lines->rest && lines->i >= position) {
			lines=lines->rest;
			num--;
		}
		return std::to_string(num) + divider + std::to_string(position-lines->i);
	}
}
#endif

static void EM_core(string message, Position pos)
{
#if USING_LOCATION_FROM_BISON
	cerr << str(pos) << ": " << message << endl;
#else
	cerr << fileName << " " << str(pos) << ": " << message << endl;
#endif
}

void EM_error(string message, bool fatal, Position position)
{
	//	if (position < 0)
	//		position = EM_tokPos;
	EM_errCount++;
	EM_core(message, position);
	if (fatal || (EM_maxErrs > 0 && EM_errCount >= EM_maxErrs)) {
		fprintf(stderr, "Giving up due to fatal error or too many errors\n");
		if (fatal && EM_crashOnFatal)
			abort(); // get into the debugger, I hope
		else
			exit(2);
	}
}

void EM_warning(string message, Position pos)
{
	//	if (position < 0)
	//		position = EM_tokPos;
	EM_core(message, pos);
}

void EM_debug(string message, Position pos)
{
	if (EM_showingDebug)
		EM_core(message, pos);
}

Position Position::current() {
	EM_warning("Warning: Position::current no longer works, using undefined instead");
	return Position(); // returns "undefined" for now
}
Position Position::range(const Position &start, const Position &end) {
	return Position(start, end);
}
Position Position::fromLex(ScannerPosition posAttributeInLex) {
	Position it;
	it.undef=false;
	it.l = posAttributeInLex;
	if (it.l.begin.filename == 0 && it.l.end.filename == 0) {
		it.l.begin.filename = &fileName; // use the one from EM_reset ...
		it.l.end.filename   = &fileName; // @TODO: figure out why flex doesn't give this
		static bool whinedAlready = false;
		if (!whinedAlready) {
			EM_debug("Huh, had to build Position from flex info that lacked file name, by using hack", it);
			whinedAlready=true;
		}
	} else if (it.l.end.filename == 0) {
		it.l.end.filename   = it.l.begin.filename;
		EM_debug("Curiouser and curiouser ...  had to build Position from flex info that lacked file name BUT ONLY IN THE END", it);
	} else if (it.l.begin.filename == 0) {
		it.l.begin.filename = it.l.end.filename; 
		EM_debug("Curiouser and curiouser! ... had to build Position from flex info that lacked file name BUT ONLY IN THE BEGIN", it);
	}
	return it;
}
Position Position::undefined() {
	return Position();
}

Position::Position()
{
	undef = true;
}

Position::Position(const Position &start, const Position &end)
{
#if USING_LOCATION_FROM_BISON
	undef = false;
	l = start.l;
	l.end = end.l.end;
#else
	s=start.s;
	e=max(max(start.s, end.s), end.e);
	//	assert("to be a valid position range, " && s>=0 && e>=0);
#endif
}

#if USING_LOCATION_FROM_BISON
#include <sstream>
#endif

string Position::__str__()
{
#if USING_LOCATION_FROM_BISON
	if (!undef) {
		std::stringstream result;
		result << l;
		return result.str();
	} else {
		return "-.-:";
	}
#else
	string sep = "."; // separate line number from position on line
	if (e < 0)
		if (s<0)
			return "--";
		else
			return EM_intpos_to_string(s, sep);
	else 
		return "[" + EM_intpos_to_string(s, sep) + ":" + EM_intpos_to_string(e, sep) + "]";
#endif
}

string Position::__repr__()
{
#if USING_LOCATION_FROM_BISON
	return "<Position corresponding to " + str(*this) + ">";
#else
	return "Position::range(Position::fromLex(" + str(s) + "), Position::fromLex(" + str(e) + "))";
#endif
}

#if 0  /* cutting this out since it's now in tigerParseDriver */
// we may want to skip this, e.g. in AST_examples
#if ! defined ERRORMSG_SKIP_LEX
// lex interface stuff, uses STDIO library rather than iostreams:

#include <stdio.h>
extern FILE *yyin;
static bool set_lex_input(bool use_stdin, const char *fname)
{
	if (use_stdin) {
 		yyin = stdin;
 		return true;
	} else {
    		yyin = fopen(fname, "r");
		return yyin;
	}
}
#endif
#endif
