
// The tigerParser class is closely based on the example from the documentation (i.e., copied and edited)
// https://www.gnu.org/software/bison/manual/html_node/Calc_002b_002b-Parsing-Driver.html#Calc_002b_002b-Parsing-Driver

tigerParser::tigerParser()
{
}

int
tigerParser::parse (const std::string &f)
{
	fileName = f;

	if (fileName == "" || file == "-") {
		yyin = stdin;
	} else if (!(yyin = fopen (file.c_str (), "r"))) {
		error ("cannot open " + file + ": " + strerror(errno));
		exit (EXIT_FAILURE);
	}

	yy::tigerParser parser (*this);
	int res = parser.parse ();  // sets this->AST_root

	fclose (yyin);
	return res;
}

void
tigerParser::error (const yy::location& l, const std::string& m)
{
	std::cerr << l << ": " << m << std::endl;
}

void
tigerParser::error (const std::string& m)
{
	std::cerr << m << std::endl;
}
