#include "AST.h"

/*
 * HERA_code methods
 */

const string indent_math = "    ";  // might want to use something different for, e.g., branches


string AST_node_::HERA_code()  // Default used during development; could be removed in final version
{
	string message = "HERA_code() requested for AST node type not yet having a HERA_code() method";
	EM_error(message, pos());
	return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
}


string A_root_::HERA_code()
{
	return main_expr->HERA_code();
}



string A_intExp_::HERA_code()
{
	return indent_math + "SET(" + my_reg_s() + ", " + str(value) +")\n";
}



static string HERA_math_op(Position p, A_oper op) // needed for opExp
{
	switch (op) {
	case A_plusOp:
		return "ADD";
	case A_timesOp:
		return "MULT";
	default:
		EM_error("Unhandled case in HERA_math_op", p);
		return "Oops";
	}
}
string A_opExp_::HERA_code()
{
	string my_code = indent_math + (HERA_math_op(pos(), _oper) + "(" +
					this->my_reg_s() + ", " +
					_left->my_reg_s() + ", " +
					_right->my_reg_s() + ")\n");
	return _left->HERA_code() + _right->HERA_code() + my_code;
}
