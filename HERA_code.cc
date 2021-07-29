#include "AST.h"

/*
 * HERA_code methods
 */

const string indent_math = "    ";  // might want to use something different for, e.g., branches
/* Methods with HERA_code:
	AST_node_
	A_root_
	A_opExp_
	A_callExp_
	A_expList_
	A_stringExp_

*/

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
	case A_divideOp:
		return "DIV";
	default:
		EM_error("Unhandled case in HERA_math_op", false, p);
		return "Oops_unhandled_hera_math_op";
	}
}
string A_opExp_::HERA_code()
{
	/* Modify to follow S-U algorithm
	   child with more registers should be first */
	int left_reg = _left->result_reg();
	int right_reg = _right->result_reg();
	string output;
	if (left_reg == right_reg) {
		/* Handle case when they are equal */ 
		output = _left->HERA_code() + 
				 indent_math + "MOVE(R" + std::to_string(left_reg+1) + ", " + _left->result_reg_s() + ")\n" + 
				 _right->HERA_code() +
				 indent_math + HERA_math_op(pos(), _oper) + "(R" + std::to_string(left_reg+1) + ", " + "R" + std::to_string(left_reg+1) + ", " + _right->result_reg_s() + ")\n";
	} else if (left_reg > right_reg) {
		/* Handle case when left is greater */ 
		output = _left->HERA_code() + _right->HERA_code() +
				 indent_math + HERA_math_op(pos(), _oper) + "(" + _left->result_reg_s() + ", " + _left->result_reg_s() + ", " + _right->result_reg_s() + ")\n";
	} else if (left_reg < right_reg) {
		/* Handle case when right is greater */ 
		output = _right->HERA_code() + _left->HERA_code() +
				 indent_math + HERA_math_op(pos(), _oper) + "(" + _right->result_reg_s() + ", " + _left->result_reg_s() + ", " + _right->result_reg_s() + ")\n";
	}
	return output;
}

string A_expList_::func_HERA_code(int counter) {
	/* Go through _head->code and move to R counter
	   Then repeat with _tail */
	counter++;
	string output = _head->HERA_code() + 
					indent_math + "MOVE(R" + std::to_string(counter) + ", " + _head->result_reg_s() + ")\n";
	if (_tail == 0) {
		return output;
	} else {
		return output + _tail->func_HERA_code(counter);
	}
}

string A_callExp_::HERA_code()
{
	/* Need custom function to do _expList_ iteration and hera code
	   generation. expList used here and in seqExp so have to
	   differentiate */
	string my_code = _args->func_HERA_code(0) + 
					 indent_math + "CALL(FP_alt, " + 
					 Symbol_to_string(_func) + ")\n"; 
	return my_code;
}

string A_stringExp_::HERA_code()
{
	/* Add preamble string memory allocation */
	string this_str_label = "string_" + std::to_string(this->count);
						 	 
	return indent_math + "SET(" + result_reg_s() + ", " + this_str_label + ")\n";
}
