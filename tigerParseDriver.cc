
// The tigerParseDriver class is closely based on the example from the documentation (i.e., copied and edited)
// https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parsing-Driver.html#Calc_002b_002b-Parsing-Driver

tigerParseDriver::tigerParseDriver()
{
}

#include <stdio.h>

int
tigerParseDriver::parse (const std::string &f)
{
	fileName = f;
	extern FILE *yyin;

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
