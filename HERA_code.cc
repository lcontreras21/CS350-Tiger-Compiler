#include "AST.h"

// IfExp Counter for branching expressions
int if_counter = 0;
int comp_counter = 0;
int while_counter = 0;


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
	A_boolExp_
	A_ifExp_
	A_seqExp_
	A_whileExp_
	A_breakExp_
*/

string AST_node_::HERA_code()  // Default used during development; could be removed in final version
{
	string message = "HERA_code() requested for AST node type not yet having a HERA_code() method";
	EM_error(message);
	return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
}

string A_root_::HERA_code()
{
	return "\nCBON()\n\n" + main_expr->HERA_code();  // was SETCB for HERA 2.3
}



string A_intExp_::HERA_code()
{
	return indent_math + "SET(" + result_reg_s() + ", " + str(value) +")\n";
}

static string HERA_comp_op(A_oper op) {
	switch (op) {
	case A_eqOp:
		return "BZ";
	case A_neqOp:
		return "BNZ";
	case A_ltOp:
		return "BL";
	case A_leOp:
		return "BLE";
	case A_gtOp:
		return "BG";
	case A_geOp:
		return "BGE";
	default:
		EM_error("Unhandled case in HERA_comp_op");
		return "0";
	}
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
		if (op == A_eqOp || op == A_neqOp || op == A_ltOp || op == A_leOp || op == A_gtOp || op == A_geOp) {
			return "1";
		} else {
			EM_error("Unhandled case in HERA_math_op", false, p);
			return "0";
		}
	}
}
string A_opExp_::HERA_code()
{
	/* Modify to follow S-U algorithm
	   child with more registers should be first */
	int left_reg = _left->result_reg();
	string left_reg_s = _left->result_reg_s();
	int right_reg = _right->result_reg();
	string right_reg_s = _right->result_reg_s();
	
	string HERA_op = HERA_math_op(pos(), _oper);
	string output_reg;
	bool comp = false;
	if (HERA_op == "1") {
		comp = true;
		HERA_op = HERA_comp_op(_oper);
	}
	string output;
	// Handle Math operations
	if (left_reg == right_reg) {
		/* Handle case when they are equal */ 
		output = _left->HERA_code() + 
				 indent_math + "MOVE(R" + std::to_string(left_reg+1) + ", " + left_reg_s + ")\n" + 
				 _right->HERA_code();
		output_reg = "R" + std::to_string(left_reg+1);
		left_reg_s = output_reg;
		if (not comp) {
			output = output + indent_math + HERA_op + "(R" + std::to_string(left_reg+1) + ", " + "R" + std::to_string(left_reg+1) + ", " + right_reg_s + ")\n";
		}
	} else if (left_reg > right_reg) {
		/* Handle case when left is greater */ 
		output = _left->HERA_code() + _right->HERA_code();
		output_reg = left_reg_s;
		if (not comp) {
			output = output + indent_math + HERA_op + "(" + left_reg_s + ", " + left_reg_s + ", " + right_reg_s + ")\n";
		}
	} else if (left_reg < right_reg) {
		/* Handle case when right is greater */ 
		output = _right->HERA_code() + _left->HERA_code();
		output_reg = right_reg_s;
		if (not comp) {
			output = output + indent_math + HERA_op + "(" + right_reg_s + ", " + left_reg_s + ", " + right_reg_s + ")\n";
		}
	}
	if (comp) {
		// Handle Comparison Operations
		// A few string vars for label creation
		int this_comp_counter = comp_counter;
		comp_counter++;
		string label = "else_comp_" + std::to_string(this_comp_counter);
		string end_label = "end_of_comp_" + std::to_string(this_comp_counter);
		// Subtract both 
		// Sub by zero to check if there is a non-zero into for true or 0 for false
		output = output + indent_math + "CMP(" + left_reg_s + ", " + right_reg_s + ")\n" 
				+ indent_math + HERA_op + "(" + label + ")\n"
				+ indent_math + "SET(" + output_reg + ", 0)\n"  
				+ indent_math + "BR(" + end_label + ")\n"
				+ indent_math + "LABEL(" + label + ")\n"
				+ indent_math + "SET(" + output_reg + ", 1)\n"
				+ indent_math + "LABEL(" + end_label + ")\n";	
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
	string my_code;
	if (_args != 0) {
		my_code = _args->func_HERA_code(0);
	}
	my_code = my_code + indent_math + "CALL(FP_alt, " + Symbol_to_string(_func) + ")\n"
			+ indent_math + "MOVE(" + this->result_reg_s() + ", R1)\n"; 
	return my_code;
}

string A_stringExp_::HERA_code()
{
	/* Add preamble string memory allocation */
	string this_str_label = "string_" + std::to_string(count);
						 	 
	return indent_math + "SET(" + result_reg_s() + ", " + this_str_label + ")\n";
}

string A_boolExp_::HERA_code()
{
	if (value) {
		return indent_math + "SET(" + result_reg_s() + ", 1)\n";
	} else {
		return indent_math + "SET(" + result_reg_s() + ", 0)\n";
	}  
}

string A_ifExp_::HERA_code()
{	
	// A few string vars for label creation
	int this_if_counter = if_counter;
	if_counter = if_counter +1;
	string else_label = "else_label_" + std::to_string(this_if_counter);
	string end_label;
	if (_else_or_null != 0) {
		end_label = "end_of_if_then_else_" + std::to_string(this_if_counter);
	}
	// _test is either an int or 0. If int do _then, else do _else_or_null
	// First do check
	string my_code = _test->HERA_code();
	// Sub by zero to check if there is a non-zero into for true or 0 for false
	my_code = my_code + indent_math + "CMP(" + _test->result_reg_s() + ", R0)\n" 
			+ indent_math + "BZ(" + else_label + ")\n"
			+ _then->HERA_code();
	if (_then->result_reg() != this->result_reg()) {
		my_code = my_code + indent_math + "MOVE(" + this->result_reg_s() + ", " + _then->result_reg_s() + ")\n";
	}
	if (_else_or_null != 0) {
		my_code = my_code + indent_math + "BR(" + end_label + ")\n"
				+ indent_math + "LABEL(" + else_label + ")\n"
				+ _else_or_null->HERA_code();
		if (_else_or_null->result_reg() != this->result_reg()) {
			my_code = my_code + indent_math + "MOVE(" + this->result_reg_s() + ", " + _else_or_null->result_reg_s() + ")\n";
		}
		my_code = my_code + indent_math + "LABEL(" + end_label + ")\n";	
	} else {
		my_code = my_code + indent_math + "LABEL(" + else_label + ")\n";
	}
	return my_code;
}

string A_seqExp_::HERA_code() {
	// Iterate through A_expList _seq and add HERA_codes
	A_expList seq = _seq;
	string my_code = "";
	string reg = "";
	if (seq == 0) {
		return my_code;
	} else {
		while (seq != 0) {
			my_code = my_code + seq->_head->HERA_code();
			reg = seq->_head->result_reg_s();
			seq = seq->_tail;
		}
		// Move last exp to reg of seq
		my_code = my_code + indent_math + "MOVE(" + result_reg_s() + ", " + reg + ")\n";
		return my_code;

	}
}

string A_whileExp_::HERA_code() {
	// Evaluate _test
	// Check if zero
	// If it is, branch to end
	// Else evaluate _body and branch to beginning of while
	int this_while_counter = while_counter;
	my_num = this_while_counter;
	while_counter++;
	string start_label = "while_start_" + std::to_string(this_while_counter);
	string end_label = "while_end_" + std::to_string(this_while_counter);
	string my_code = indent_math + "LABEL(" + start_label + ")\n"
			+ _test->HERA_code()
			+ indent_math + "CMP(" + _test->result_reg_s() + ", R0)\n" 
			+ indent_math + "BZ(" + end_label + ")\n"
			+ _body->HERA_code()
			+ indent_math + "BR(" + start_label + ")\n"
			+ indent_math + "LABEL(" + end_label + ")\n";
	return my_code;
}

string A_breakExp_::HERA_code() {
	int earliest_while = am_i_in_while();
	return indent_math + "BR(while_end_" + std::to_string(earliest_while) + ")\n";
}
