#include "AST.h"
#include "ST.h"

// IfExp Counter for branching expressions
int if_counter = 0;
int comp_counter = 0;
int loop_counter = 0;
int SP_counter = 0;

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
	A_forExp_
	A_varExp_
	A_simpleVar_
	A_letExp_
	A_decList_
	A_varDec_
	A_assignExp_
	A_functionDec_
	A_fundecList_
	A_fundec_
*/

string AST_node_::HERA_code()  // Default used during development; could be removed in final version
{
	string message = "HERA_code() requested for AST node type not yet having a HERA_code() method";
	EM_error(message);
	return "#error " + message;  //if somehow we try to HERA-C-Run this, it will fail
}

string func_HERA_code = "";

string A_root_::HERA_code()
{
	string output = "\nCBON()\n\n" + main_expr->HERA_code()  // was SETCB for HERA 2.3
		+ "\nHALT()\n" + func_HERA_code;

	return output;
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
		EM_error("ERROR: Unhandled case in HERA_comp_op");
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
			EM_error("ERROR: Unhandled case in HERA_math_op", false, p);
			return "0";
		}
	}
}

string A_opExp_::HERA_code()
{
	/* Modify to follow S-U algorithm child with more registers should be first */
	int left_reg = _left->result_reg();
	string left_reg_s = _left->result_reg_s();
	int right_reg = _right->result_reg();
	string right_reg_s = _right->result_reg_s();
	
	string HERA_op = HERA_math_op(pos(), _oper);
	string output_reg_s;
	bool comp = false;
	if (HERA_op == "1") {
		comp = true;
		HERA_op = HERA_comp_op(_oper);
	}
	string output;
	// Handle which operation happens first according to SU algorithm
	if (left_reg == right_reg) {
		/* Handle case when they are equal */ 
		output_reg_s = "R" + std::to_string(left_reg+1);
		output = _left->HERA_code() + 
				 indent_math + "MOVE(" + output_reg_s + ", " + left_reg_s + ") // in opExp\n" +
				 _right->HERA_code();
		left_reg_s = output_reg_s;
	}  else {
		output = (left_reg > right_reg) ? (_left->HERA_code() + _right->HERA_code()) : (_right->HERA_code() + _left->HERA_code());
		output_reg_s = (left_reg > right_reg) ? (left_reg_s) : (right_reg_s);
	}
	
	if (not comp) {
		// Arithmetic Operation
		output = output + indent_math + HERA_op + "("+ output_reg_s + ", " + left_reg_s + ", " + right_reg_s + ")\n";
	} else  {
		// A few string vars for label creation
		int this_comp_counter = comp_counter;
		comp_counter++;
		string label = "else_comp_" + std::to_string(this_comp_counter);
		string end_label = "end_of_comp_" + std::to_string(this_comp_counter);

		// Int Comparisons
		if (_left->typecheck() == Ty_Int()) {
			// Handle Comparison Operations
			output = output + indent_math + "CMP(" + left_reg_s + ", " + right_reg_s + ")\n"; 
		} else if (_left->typecheck() == Ty_String()) {
			// String comparison. Function call to tstrcmp
			output = output 
				+ "// Start of Function Call for function tstrcmp in opExp. Current SP at: " + std::to_string(SP_counter) 
				+ indent_math + "MOVE(Rt, FP_alt)\n"
				+ indent_math + "MOVE(FP_alt, SP)\n" 
				+ indent_math + "INC(SP, 5)\n";
			SP_counter = SP_counter + 5;
			output = output + indent_math + "STORE(Rt, 2, FP_alt)\n"
				+ indent_math + "STORE(" + left_reg_s + ", 3, FP_alt)\n"
				+ indent_math + "STORE(" + right_reg_s + ", 4, FP_alt)\n"
				+ indent_math + "CALL(FP_alt, tstrcmp)\n"
				+ indent_math + "LOAD(" + output_reg_s + ", 3, FP_alt)\n"
				+ indent_math + "LOAD(FP_alt, 2, FP_alt)\n"
				+ indent_math + "DEC(SP, 5)\n"
				+ indent_math + "CMP(" + output_reg_s + ", R0)\n"; 
			SP_counter = SP_counter - 5;
		}
		// Comparison Operation and Branching. Generic to all comparisons
		output = output + indent_math + HERA_op + "(" + label + ")\n"
				+ indent_math + "SET(" + output_reg_s + ", 0)\n"  
				+ indent_math + "BR(" + end_label + ")\n"
				+ indent_math + "LABEL(" + label + ")\n"
				+ indent_math + "SET(" + output_reg_s + ", 1)\n"
				+ indent_math + "LABEL(" + end_label + ")\n";	
	}
	return output;
}

string A_callExp_::_args_HERA_code(int counter) {
	// Using parameter on the stack version
	// Go through expList and move SP counter
	// Call HERA_code and store(reg, SP_counter, FP_alt)
	string output = "";
	A_expList args = _args;
	while(args != 0) {
		output = output + args->_head->HERA_code() 
						+ indent_math + "STORE(" + args->_head->result_reg_s() + ", " + std::to_string(counter) + ", FP_alt)\n";
		args = args->_tail;
		counter++;
	}
	return output;
	
	//---------------------------------------

	/* Code for parater as registers
	// Go through _head->code and move to R counter
	//   Then repeat with _tail 
	counter++;
	string output = _head->HERA_code() + 
					indent_math + "MOVE(R" + std::to_string(counter) + ", " + _head->result_reg_s() + ")\n";
	if (_tail == 0) {
		return output;
	} else {
		return output + _tail->func_HERA_code(counter);
	}
	*/
}

string A_callExp_::HERA_code()
{
	// Using parameter on the stack version
	
	// Check if func has return
	string return_val = this->func_returnq(stored_parent->get_my_function_library(this));
	int stack_size = 3;
	if (_args != 0) {
		stack_size = stack_size + _args->length();
	}
	string my_code = "// Start of Function Call for function " + Symbol_to_string(_func) + ". Current SP at: " + std::to_string(SP_counter) + "\n";
	SP_counter = SP_counter + stack_size;
	my_code = my_code 
			+ indent_math + "MOVE(Rt, FP_alt)\n"                                //
			+ indent_math + "MOVE(FP_alt, SP)\n"                                //
			+ indent_math + "INC(SP, " + std::to_string(stack_size) + ")\n"     //
//			+ indent_math + "STORE(Rt, 2, FP_alt)\n"                            //
			+ _args_HERA_code(3) 
			+ indent_math + "CALL(FP_alt, " + Symbol_to_string(_func) + ")\n"
			+ return_val
//			+ indent_math + "LOAD(FP_alt, 2, FP_alt)\n"                         //
			+ indent_math + "DEC(SP, " + std::to_string(stack_size) + ")\n";	//
	SP_counter = SP_counter - stack_size;
	my_code = my_code + "// End of Function Call for function " + Symbol_to_string(_func) + ". Current SP at: " + std::to_string(SP_counter) + "\n";
	return my_code;

	//---------------------------------------
	

	/*
	// Code for parameters as registers
	string my_code;
	if (_args != 0) {
		my_code = _args->func_HERA_code(0);
	}
	my_code = my_code + indent_math + "CALL(FP_alt, " + Symbol_to_string(_func) + ")\n"
			+ indent_math + "MOVE(" + this->result_reg_s() + ", R1)\n"; 
	return my_code;
	*/
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
	string reg_str = "";
	int reg_int = 4;  // TODO Make this set to default min register
	if (seq == 0) {
		return my_code;
	} else {
		while (seq != 0) {
			my_code = my_code + seq->_head->HERA_code();
			reg_int = seq->_head->result_reg();
			reg_str = seq->_head->result_reg_s();
			seq = seq->_tail;
		}
		// Move last exp to reg of seq if not already there
		string move_statement = result_reg() == reg_int ? "" : indent_math + "MOVE(" + result_reg_s() + ", " + reg_str + ")\n";
		my_code = my_code + move_statement;
		return my_code;

	}
}

string A_whileExp_::HERA_code() {
    EM_debug("Compiling whileExp");

	// Evaluate _test
	// Check if zero
	// If it is, branch to end
	// Else evaluate _body and branch to beginning of while
	int this_loop_counter = loop_counter;
	my_num = this_loop_counter;
	loop_counter++;
	string start_label = "loop_start_" + std::to_string(this_loop_counter);
	string end_label = "loop_end_" + std::to_string(this_loop_counter);
	string my_code = "// Start of While loop: " + std::to_string(my_num) + "\n"
			+ indent_math + "LABEL(" + start_label + ")\n"
			+ _test->HERA_code()
			+ indent_math + "CMP(" + _test->result_reg_s() + ", R0)\n" 
			+ indent_math + "BZ(" + end_label + ")\n"
			+ _body->HERA_code()
			+ indent_math + "BR(" + start_label + ")\n"
			+ indent_math + "LABEL(" + end_label + ")\n"
			+ "// End of While Loop: " + std::to_string(my_num) + "\n";
	return my_code;
}

string A_breakExp_::HERA_code() {
	int earliest_while = am_i_in_loop(this);
	return indent_math + "BR(loop_end_" + std::to_string(earliest_while) + ")  // Break in LOOP\n";
}

string A_forExp_::HERA_code() {
	// Strings used for loop management
	int this_loop_counter = loop_counter;
	int this_SP_counter = calculate_my_SP(this);
	my_num = this_loop_counter;
	loop_counter++;
	string start_label = "loop_start_" + std::to_string(this_loop_counter);
	string end_label = "loop_end_" + std::to_string(this_loop_counter);
	// SP location strings for loop bounds
	string _lo_sp_loc = std::to_string(this_SP_counter);
	string _hi_sp_loc = std::to_string(this_SP_counter+1);
	SP_counter = SP_counter + 2;
	// Store the _var in Stack with _lo, and store _hi one above that
	string output = "// Start of For Loop: " + std::to_string(my_num) + ". Current SP at: " + std::to_string(SP_counter) + "\n"
				  + indent_math + "INC(SP, 2)\n"
				  + _lo->HERA_code() 
				  + indent_math + "STORE(" + _lo->result_reg_s() + ", " + _lo_sp_loc + ", FP)\n"
				  + _hi->HERA_code()
				  + indent_math + "STORE(" + _hi->result_reg_s() + ", " + _hi_sp_loc + ", FP)\n";

	// Start of Loop
	output = output + indent_math + "LABEL(" + start_label + ")\n";
		// Load _var from Stack (_lo and _hi
	output = output + indent_math + "LOAD(R1, " + _lo_sp_loc + ", FP)\n" 
					+ indent_math + "LOAD(R2, " + _hi_sp_loc + ", FP)\n"; 
		// Compare _lo to _hi, if <= 0 go to end of loop, Otherwise go through loop
	output = output	+ indent_math + "CMP(R2, R1)\n"
					+ indent_math + "BL(" + end_label + ")\n";
		// Run _body HERA_code
	output = output + _body->HERA_code();
		// Increment _hi and store in _var in Stack
	output = output + indent_math + "LOAD(R1, " + _lo_sp_loc + ", FP)\n"
					+ indent_math + "INC(R1, 1)\n"
					+ indent_math + "STORE(R1, " + _lo_sp_loc + ", FP)\n";
		// Branch back to beginning of loop
	output = output + indent_math + "BR(" + start_label + ")\n";
	// End of Loop. Decrement the SP
	SP_counter = SP_counter - 2;
	output = output + indent_math + "LABEL(" + end_label + ")\n"
					+ indent_math + "DEC(SP, 2)\n"
					+ "// End of For Loop: " + std::to_string(my_num) + ". Current SP at: " + std::to_string(SP_counter) + "\n";
	return output;
}

string A_varExp_::HERA_code() {
	return _var->HERA_code();
}

string A_simpleVar_::HERA_code() {
    EM_debug("Compiling simpleVar " + Symbol_to_string(_sym));

    ST<var_info> my_variable_library = stored_parent->get_my_variable_library(this);
	// Two cases: In A_varDec_ or A_assignExp_
	int inAssignExp = am_i_in_assignExp_(this);
	// Returns register of new assignment value if in assignexp, otherwise < 0

	string output = "";
	if (is_name_there(_sym, my_variable_library)) {
		var_info var_struct = lookup(_sym, my_variable_library);
        string variable_comment = Symbol_to_string(_sym) + "' at SP: " + std::to_string(var_struct.my_SP()) + "\n";
		if (inAssignExp > 0) {
			// Check if var is writable, otherwise produce error
			bool writable = var_struct.am_i_writable();
			if (writable) {
				output = indent_math + "STORE(R" + std::to_string(inAssignExp) + ", " + std::to_string(var_struct.my_SP()) + ", FP)" +
				         indent_math + "// Reassigning Variable '" + variable_comment;
			} else {
				EM_error("ERROR: Tried to write to a variable that is not writable. This happens most often when trying to"
				         " write to the loop variable in an IF statement");
			}
		} else {
			// In varDec_
			// Access sp number from declaration
			// Load it into R4
			output = indent_math + "LOAD(R4, " + std::to_string(var_struct.my_SP()) + ", FP)" +
			         indent_math + "// Accessing Variable '" + variable_comment;
		}
		return output;
	} else {
	    EM_error("ERROR: A_simpleVar: Could not find " + Symbol_to_string(_sym));
		return "";
	}
}

string A_letExp_::HERA_code() {
    EM_debug("Compiling letExp");

	// INC SP by number of things being declared
	int my_SP = _decs ? _decs->calculate_my_SP(this) : 0;
    string this_counter = get_my_let_number_s();
	string output = "// Start of Let Expression " + this_counter + ". Stack starting at SP: " + std::to_string(SP_counter) +  ". Initializing " + std::to_string(my_SP) + " variable(s).\n";
	if (my_SP > 0) {
		output = output + indent_math + "INC(SP, " + std::to_string(my_SP) + ")\n";	
	}
	// Increment the SP_counter
	SP_counter = SP_counter + my_SP;
	// Save current type_library, function_library to restore later
	ST<type_info> copy_type_lib = type_library;
	ST<function_info> copy_tiger_lib = tiger_library;
	// Make changes to STs and store decs into Stack
		// Should happen in A_decList_::HERA_code and in A_decs_
	if (_decs != 0) {
		output = output + _decs->HERA_code()
			   + indent_math + "// Finished declaring variables in Let Expression " + this_counter + ". Stack now at SP: " + std::to_string(SP_counter) + "\n";
	}
	// Do _body HERA_code
	if (_body != 0) {
		A_expList body = _body;
		while (body != 0) {
			output = output + body->_head->HERA_code();
			body = body->_tail;
		}
	}
	// Move result to final reg if necessary
\	if (this->result_reg() != _body->result_reg()) {
        output = output + indent_math + "MOVE(" + this->result_reg_s() + ", " + _body->result_reg_s() + ")\n";
	}

	// DEC SP by same amount as was increased
	if (my_SP > 0) {
		output = output + indent_math + "DEC(SP, " + std::to_string(my_SP) + ")\n";
	}
	SP_counter = SP_counter - my_SP;
	// Restore all scopes to original
	type_library = copy_type_lib;
	tiger_library = copy_tiger_lib;
	output = output + "// END of Let Expression " + this_counter + ". Stack back at SP: " + std::to_string(SP_counter) + "\n";
	return output;
}

string A_decList_::HERA_code() {
    EM_debug("Compiling decList");
    string output = _head->HERA_code();
	if (_tail == 0) {
		return output;
	} else {
		return output + _tail->HERA_code();
	}
}

string A_varDec_::HERA_code() {
    EM_debug("Compiling varDec: " + Symbol_to_string(_var));

    ST<var_info> my_variable_library = stored_parent->get_my_variable_library(this);
    EM_debug("Compiling varDec: " + Symbol_to_string(_var) + " ST: " + my_variable_library.__repr__());
    string my_sp_number = std::to_string(calculate_my_SP(this));
    string variable_comment = Symbol_to_string(_var) + " at SP: " + my_sp_number + "\n";

	// Add variable to stack
	string output = _init->HERA_code()
				  + indent_math + "STORE(" + _init->result_reg_s() + ", " + my_sp_number + ", FP)"
				  + indent_math + "// Declaring variable " + variable_comment;
	return output;
}

string A_assignExp_::HERA_code() {
	// Run code for _exp
	// Have _var store that in the ST
	return _exp->HERA_code() + _var->HERA_code(); 
}

string A_functionDec_::HERA_code() {
	string output; 
	// Need to do two passes, just like with typechecking, to set up functions in function library for recursive functions
	// First pass:
	theFunctions->HERA_code();
	// Second pass
	string func_code = theFunctions->HERA_code();
	output = "// Start of Function Declarations\n" + func_code + "// End of Function Declarations\n";
	// Have to add Function Definitions to end of HERA_code, not with all the other code
	func_HERA_code = func_HERA_code + output;
	return "";
}

string A_fundecList_::HERA_code() {
	if (_tail == 0) {
		return _head->HERA_code();
	} else {
		return _head->HERA_code() + _tail->HERA_code();
	}
}

string A_fundec_::HERA_code() {
	/* To define a function to be called with these conventions, we use these steps, as needed:
		• Increment SP to make space for local storage
		• Save registers, including PC_ret (return address) and FP_alt (dynamic link)
		• Give the function body (in which parameters come from the stack frame, e.g. FP + 3)
		• Store the return value at FP + 3
		• Restore saved registers, including FP_alt and PC_ret, and decrement SP
		• RETURN from the function
	*/
	// Make copy of function and variable library
	if (not firstPass) {
		// Add function to tiger_library
		tiger_library = FuseOneScope(tiger_library, this_func_ST);
		firstPass = true;
		return "";
	} else {
		// Add params to ST and make available in body, make copy of vars
		string output;
		output  = "// Start of Function Definition: " + Symbol_to_string(_name) + "\n"
				+ "LABEL(" + Symbol_to_string(_name) + ")\n"
				+ indent_math + "// Saving PC_ret, FP_alt\n"
				+ indent_math + "STORE(PC_ret, 0, FP) // Return Address\n"
				+ indent_math + "STORE(FP_alt, 1, FP) // Control Link\n"
				+ indent_math + "// Body of Function\n"
				+ indent_math + _body->HERA_code()
				+ indent_math + "STORE(" + _body->result_reg_s() + ", 3, FP)\n"
				+ indent_math + "LOAD(PC_ret, 0, FP)\n"
				+ indent_math + "LOAD(FP_alt, 1, FP)\n"
				+ indent_math + "RETURN(FP_alt, PC_ret)\n";
		return output;
	}
}

