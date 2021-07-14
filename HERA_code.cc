#include "AST.h"

/*
 * HERA_code methods
 */

const string indent_math = "    ";  // might want to use something different for, e.g., branches


string AST_node_::HERA_code()  // Default used during development; could be removed in final version
{
	string message = "HERA_code() requested for AST node type not yet having a HERA_code() method";
	EM_error(message);
	return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
}


string A_root_::HERA_code()
{
	return main_expr->HERA_data() + "\nCBON()\n\n" + main_expr->HERA_code();  // was SETCB for HERA 2.3
}



string A_intExp_::HERA_code()
{
	return indent_math + "SET(" + result_reg_s() + ", " + str(value) +")\n";
}



static string HERA_math_op(Position p, A_oper op) // needed for opExp
{
	switch (op) {
	case A_plusOp:
		return "ADD";
	case A_minusOp:
		return "SUB";
	case A_timesOp:
		return "MUL";	// was MULT for HERA 2.3
	default:
		EM_error("Unhandled case in HERA_math_op", false, p);
		return "Oops_unhandled_hera_math_op";
	}
}
string A_opExp_::HERA_code()
{
	string my_code = indent_math + (HERA_math_op(pos(), _oper) + "(" +
					this->result_reg_s() + ", " +
					_left->result_reg_s() + ", " +
					_right->result_reg_s() + ")\n");
	return _left->HERA_code() + _right->HERA_code() + my_code;
}

string A_callExp_::HERA_code()
{
	string my_code = indent_math + "MOVE(R1, R3)\n" + 
					 indent_math + "CALL(FP_alt, " + 
					 Symbol_to_string(_func) + ")\n";	
	return _args->HERA_code() + my_code;
}

string A_expList_::HERA_code()
{
	string output =_head->HERA_code(); 
	if (_tail != 0)
	{
		output = output + _tail->HERA_code();
	}
	return output;
}

string A_stringExp_::HERA_code()
{
	/* Add preamble string memory allocation */
	string this_str_label = "string_" + std::to_string(count);
						 	 
	return indent_math + "SET(" + result_reg_s() + ", " + this_str_label + ")\n";
}
