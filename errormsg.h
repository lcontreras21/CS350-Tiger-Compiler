#if ! defined ERRORMSG_H
#define ERRORMSG_H

#include "util.h"

#define USING_LOCATION_FROM_BISON 1

#if USING_LOCATION_FROM_BISON
#include "location.hh"  // auto-built by bison, when used with the location option turned on
typedef yy::location ScannerPosition;
#else
typedef int ScannerPosition;  // Appel's "pos" type was just one integer; corresponds to one *token*, not e.g. an *expression*
#endif

class Position {
public:
	// these are for style/clarity, just names for the constructors, in order of how likely they are to be used early
	static Position current();  // what is the current state of the lexical scanner (Appel calls this EM_currentPos())
	static Position range(const Position &start, const Position &end); // range from one positino to another
	static Position fromLex(ScannerPosition posAttributeInLex);        // convert from a "lex" token position
	static Position undefined();                                       // what if there isn't one, e.g. AST_example()

	string __repr__();
	string __str__();

private: // actually, these are just 'discouraged' style; remove this line if you really want to call them rather than using the named static operations above
	Position();
	Position(const Position &start, const Position &end);
private:
#if USING_LOCATION_FROM_BISON
	ScannerPosition l;
	bool undef;
#else
	ScannerPosition s, e;   // use e==-1 for 'only one position, not a range'
#endif
};


// Print errors, warnings, or compiler debugging info
// for debug, message will only be printed if for_which_lab >= which_cs350_lab in tiger.cc
void EM_error  (string message, bool fatal=false, Position position = Position::undefined());
void EM_warning(string message, Position position = Position::undefined());
void EM_debug  (string message, Position position = Position::undefined());

// In the end, did we record any errors?
bool EM_recorded_any_errors();


// Reset to start parsing a new file, count lines from 1
//  max_errors is max # of errors to allow (or negative for never give up)
//  show_debug controls printing of EM_debug messages
//  crash_compiler_on_fatal_error can be turned on to cause fatal errors to call "abort", hopefully triggering debugger at that point

void EM_reset(string filename, int max_errors=-1, bool show_debug=false, bool crash_compiler_on_fatal_error=false);

//
// These should ONLY be called from the lexical scanner, so we just declare them there
//  (this is old C code, modern code would have a class and make the scanner a friend or something
//
// Notify error message system when we see a newline in input
// void EM_newline(void);
// void EM_set_currentPos(ScannerPosition new_pos);  // CALL THIS ONLY FROM TIGER_LEX.L ... don't even declare it here
// ScannerPosition  EM_currentPos();
// ScannerPosition  EM_currentScannerPosition(); // same as above

#endif
