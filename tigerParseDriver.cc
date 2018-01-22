// The tigerParseDriver class is closely based on the example from the documentation (i.e., copied and edited)
// https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parsing-Driver.html#Calc_002b_002b-Parsing-Driver

#include "tigerParseDriver.h"

tigerParseDriver::tigerParseDriver()
{
}

#include <stdio.h>

// Parse method is in the .ll file to get access to YYIN

void
tigerParseDriver::error (const yy::location& l, const std::string& m)
{
	std::cerr << l << ": " << m << std::endl;
}

void
tigerParseDriver::error (const std::string& m)
{
	std::cerr << m << std::endl;
}
