/*
 * errormsg.c - functions used in all phases of the compiler to give
 *              error messages about the Tiger program.
 *
 */

#include <cstdlib>
#include <iostream>
using namespace std;
#include "util.h"
#include "errormsg.h"

#include "which_cs350_lab.h"

static int EM_errCount;
static int EM_maxErrs;
static int EM_tokPos;
static bool EM_showingDebug;

static string fileName;
static int lineNum;

typedef struct intList {int i; struct intList *rest;} *IntList;
static IntList intList(int i, IntList rest) 
{
	IntList l= (IntList) checked_malloc(sizeof *l);
	l->i=i; l->rest=rest;
	return l;
}

static IntList linePos=NULL;


#if ! defined ERRORMSG_SKIP_LEX
static bool set_lex_input(bool use_stdin, const char *fname);  // set stuff for lex -- see below
#endif

void EM_reset(string fname, int max_errors, bool show_debug)
{
	EM_errCount = 0;
	EM_maxErrs  = max_errors;
	EM_showingDebug = show_debug;
	EM_tokPos = 1;
	fileName=fname;
	lineNum=1;
	linePos=intList(0,NULL);
#if ! defined ERRORMSG_SKIP_LEX
	if (!set_lex_input((fname == "-" || fname == ""), fname.c_str())) {
		EM_error("Cannot open file: " + fname, Position::undefined(), true);
	}
#endif
}

void EM_set_currentPos(int new_pos)
{
	EM_tokPos = new_pos;
}


void EM_newline(void)
{
	lineNum++;
	linePos = intList(EM_tokPos, linePos);
}

bool EM_recorded_any_errors()
{
	return EM_errCount > 0;
}

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

static void EM_core(string message, Position pos)
{
	cerr << fileName << " " << str(pos) << ": " << message << endl;
}

void EM_error(string message, Position position, bool fatal)
{
	//	if (position < 0)
	//		position = EM_tokPos;
	EM_errCount++;
	EM_core(message, position);
	if (fatal || (EM_maxErrs > 0 && EM_errCount >= EM_maxErrs)) {
		fprintf(stderr, "Giving up due to fatal error or too many errors\n");
		if (EM_showingDebug)
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

void EM_debug(string message, Position pos, int for_which_lab)
{
	//	if (position < 0)
	//		position = EM_tokPos;
	
	if (EM_showingDebug && for_which_lab >= which_cs350_lab)
		EM_core(message, pos);
}

Position::Position()
{
	s=EM_tokPos;
	e=-1;
}

Position::Position(const Position &start, const Position &end)
{
	s=start.s;
	e=max(max(start.s, end.s), end.e);
	assert("to be a valid position range, " && s>=0 && e>=0);
}

string Position::__str__()
{
	string sep = "."; // separate line number from position on line
	if (e < 0)
		if (s<0)
			return "--";
		else
			return EM_intpos_to_string(s, sep);
	else 
		return "[" + EM_intpos_to_string(s, sep) + ":" + EM_intpos_to_string(e, sep) + "]";
}

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
