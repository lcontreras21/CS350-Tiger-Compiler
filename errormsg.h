#if ! defined ERRORMSG_H
#define ERRORMSG_H

#include "util.h"

class Position {
public:
	// these are for style/clarity, just names for the constructors
	static Position current()
	{ return Position(); }
	static Position range(const Position &start, const Position &end)
	{ return Position(start, end); }

	string __str__();

private: // actually, these are just 'discoraged' style; remove this line if you really want to call them rather than using the named static operations above
	Position();
	Position(const Position &start, const Position &end);
private:
	int s, e;   // use e==-1 for 'only one position, not a range'
};


// Print errors, warnings, or compiler debugging info
// position of -1 means use current scanner position
// for debug, message will only be printed if for_which_lab >= which_cs350_lab in tiger.cc
void EM_error(string message, Position position = Position::current(), bool fatal=false);
void EM_warning(string message, Position position = Position::current());
void EM_debug(string message, Position position = Position::current(), int for_which_lab = 1000);

// In the end, did we record any errors?
bool EM_recorded_any_errors();


// Reset to start parsing a new file, count lines from 1
//  max_errors is max # of errors to allow (or negative for never give up)
//  show_debug controls printing of EM_debug messages

 void EM_reset(string filename, int max_errors=-1, bool show_debug=false);

//
// These should ONLY be called from the lexical scanner, so we just declare them there
//  (this is old C code, modern code would have a class and make the scanner a friend or something
//
// Notify error message system when we see a newline in input
// void EM_newline(void);
// void EM_set_currentPos(int new_pos);  // CALL THIS ONLY FROM TIGER_LEX.L ... don't even declare it here
// int  EM_currentPos();

#endif
