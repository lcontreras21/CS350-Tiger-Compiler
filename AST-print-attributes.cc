#include "util.h"
#include "AST.h"
#include "errormsg.h"
#include <logic.h>

/*
 * Some stuff for printing attributes
 */


String AST_node_::attributes_for_printing()
{
	return (string("")
		+ ("pos = "            + str(stored_pos)      + "; ")
		// concatenate any other attributes here for printing
		);
}


string A_exp_::attributes_for_printing()
{
	return (AST_node_::attributes_for_printing()
		+ "my_reg() = " + str(result_reg()) + "; "
		// concatenate any other EXP attributes here for printing
		);
}



