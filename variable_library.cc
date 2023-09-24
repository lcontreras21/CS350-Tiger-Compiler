#include "AST.h"
#include "ST.h"

/*
	Functions used by parents to find out the variable library
*/

//--------------------------------------------------------------------------------

/* Methods with getter method have ** on their name and any field nested below that will assume to use it if it doesn't
have **
	AST_node_**
		A_exp_
			A_literalExp_
				A_leafExp_
					A_nilExp_
					A_intExp_
					A_boolExp_
					A_stringExp_
				A_recordExp_
				A_arrayExp_
			A_varExp_
			A_opExp_
			A_assignExp_
			A_letExp_**
			A_callExp_
			A_controlExp_
				A_ifExp_
				A_whileExp_
				A_forExp_**
				A_breakExp_
				A_seqExp_
		A_var_
			A_simpleVar_
			A_fieldVar_
			A_subscriptVar_
		A_dec_**
			A_varDec_**
			A_functionDec_** TODO
			A_typeDec_
			A_decList_**
		A_fundec_**
		A_expList_
		A_efield_
		A_efieldList_
		A_fundecList_
		A_namety_
		A_nametyList_
		A_field_**
		A_fieldList_**
		A_ty_
			A_nameTy_
			A_arrayty_
			A_recordty_
*/

//--------------------------------------------------------------------------------

ST<var_info> AST_node_::set_my_variable_library(AST_node_ *child) {
    EM_debug("Variablizing AST_node_");
    if (child == stored_parent) {
        EM_debug("Variabalizing AST_node_: got stored_parent as child param");
    }
    return stored_parent->get_my_variable_library(this);
}

ST<var_info> A_root_::set_my_variable_library(AST_node_ *child) {
    EM_debug("Variablizing root");
    return ST<var_info>();
}

ST<var_info> A_letExp_::get_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _decs || _parent_or_child == _body) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _decs ? "decs": "body");
        EM_debug("Variablizing letExp #" + get_my_let_number_s() + ": getting library for " + who);

        ST<var_info>* my_variable_library_ptr;
        if (_parent_or_child == stored_parent) {
            my_variable_library_ptr = &this->my_variable_library_asked_by_parent;
        } else if (_parent_or_child == _decs) {
            my_variable_library_ptr = &this->my_variable_library_asked_by_decs;
        } else {
            my_variable_library_ptr = &this->my_variable_library_asked_by_body;
        }
        if (is_name_there(to_Symbol("Empty"), *my_variable_library_ptr)) {
            *my_variable_library_ptr = set_my_variable_library(_parent_or_child);
        }
        return *my_variable_library_ptr;
    } else {
        EM_error("Variablizing letExp error: _parent_or_child param does not match stored_parent, _decs, _body");
        return ST<var_info>();
    }
}

ST<var_info> A_letExp_::set_my_variable_library(AST_node_ *child) {
    if (child == stored_parent) {
        EM_debug("Variablizing letExp #" + get_my_let_number_s() + " when asked by parent to be empty");
        return ST<var_info>();
    } else if (child == _decs || child == _body) {
        string who = child == _decs ? " decs" : " body";
        EM_debug("Variablizing letExp #" + get_my_let_number_s() + " when asked by " + who);

        EM_debug("Variablizing letExp #" + get_my_let_number_s() + " getting parent library");
        ST<var_info> parent_variable_library = stored_parent->get_my_variable_library(this);

        if (child == _decs) {
            return parent_variable_library;
        } else {
            ST<var_info> dec_variable_library = _decs ? _decs->get_my_variable_library(this) : ST<var_info>();
            return MergeAndShadow(dec_variable_library, parent_variable_library);
        }
    } else {
        EM_error("Variablizing letExp #" + get_my_let_number_s() + " error: child does not match _decs, or _body");
        return ST<var_info>();
    }
}

ST<var_info> A_forExp_::set_my_variable_library(AST_node_ *child) {
    ST<var_info> parent_variable_library = stored_parent->get_my_variable_library(this);

    int this_SP_counter = calculate_my_SP(this);
    ST<var_info> for_var = ST<var_info>(_var, var_info(Ty_Int(), this_SP_counter, false));

    // Add variable to variable library
    return MergeAndShadow(for_var, parent_variable_library);
}

ST<var_info> A_dec_::set_my_variable_library(AST_node_ *child) {
    EM_debug("Variablizing dec error: currently not set up for");
    return ST<var_info>();
}

ST<var_info> A_varDec_::get_my_variable_library(AST_node_ *_parent_or_child) {
    EM_debug("Variablizing varDec " + Symbol_to_string(_var) + ": getting library");
    if (_parent_or_child == stored_parent || _parent_or_child == _init) {
        ST<var_info>* my_variable_library_ptr = _parent_or_child == stored_parent ? &this->my_variable_library_asked_by_parent : &this->my_variable_library_asked_by_child;
        if (is_name_there(to_Symbol("Empty"), *my_variable_library_ptr)) {
            *my_variable_library_ptr = set_my_variable_library(_parent_or_child);
        }
        return *my_variable_library_ptr;
    } else {
        EM_error("Variablizing decList error: _parent_or_child param does not match _head or _tail");
        return ST<var_info>();
    }
}

ST<var_info> A_varDec_::set_my_variable_library(AST_node_ *_parent_or_child) {
    ST<var_info> declared_variable_library;

	if (_parent_or_child == stored_parent) {
        int my_SP = calculate_my_SP(this);
        if (Symbols_are_equal(_typ, to_Symbol("NA"))) {  // No explicit type declared
            // Use type of _init to initialize type
            // Initialize type to Ty_Placeholder to not cause a race condition during typechecking
            declared_variable_library = ST<var_info>(_var, var_info(_init->typecheck(), my_SP, true));
        } else {
            // Lookup the type for _typ in ST type_library
            if (is_name_there(_typ, type_library)) {
                type_info type_struct = lookup(_typ, type_library);
                Ty_ty type = type_struct.my_type();
                declared_variable_library = ST<var_info>(_var, var_info(type, my_SP, true));
            } else {
                EM_error("Variablizing varDec error: Type not found for VarDec" + Symbol_to_string(_var) + "with type " + Symbol_to_string(_typ));
                return ST<var_info>();
            }
        }
	    EM_debug("Variablizing varDec " + Symbol_to_string(_var) + " when asked by letExp");
	    return declared_variable_library;
	} else if (_parent_or_child == _init) {
	    EM_debug("Variablizing varDec " + Symbol_to_string(_var) + " when asked by init");
	    return stored_parent->get_my_variable_library(this);
	} else {
	    // Unexpected error
	    EM_error("Variablizing varDec error: _parent_or_child does not match stored parent or init");
        return ST<var_info>();
	}
}

ST<var_info> A_decList_::get_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _head || _parent_or_child == _tail) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _head ? "head": "tail");

        ST<var_info>* my_variable_library_ptr = _parent_or_child == stored_parent ? &this->my_variable_library_asked_by_parent : (_parent_or_child == _head ? &this->my_variable_library_asked_by_head : &this->my_variable_library_asked_by_tail);
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_variable_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Variablizing decList: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_variable_library_ptr = set_my_variable_library(_parent_or_child);
        }
        return *my_variable_library_ptr;
    } else {
        EM_error("Variablizing decList error: _parent_or_child param does not match _head or _tail");
        return ST<var_info>();
    }
}

ST<var_info> A_decList_::set_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Variablizing decList from parent");
        // A letExp is asking about its declarations
        ST<var_info> dec_variable_library  = _head->get_my_variable_library(this);
        ST<var_info> tail_variable_library = _tail ? _tail->get_my_variable_library(this) : ST<var_info>();
        return MergeAndShadow(tail_variable_library, dec_variable_library);
    } else if (_parent_or_child == _head || _parent_or_child == _tail) {
        // A dec is asking about previous declarations or variables available to let
        ST<var_info> parent_variable_library = stored_parent->get_my_variable_library(this);
        if (_parent_or_child == _head) {
            EM_debug("Variablizing decList from head");
            return parent_variable_library;
        } else {
            EM_debug("Variablizing decList from tail");
            ST<var_info> dec_variable_library  = _head->get_my_variable_library(this);
            return MergeAndShadow(dec_variable_library, parent_variable_library);
        }
    } else {
        EM_error("Variablizing decList error: _parent_or_child param does not match _head or _tail");
        return ST<var_info>();
    }
}

ST<var_info> A_functionDec_::get_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == theFunctions) {
        string who = _parent_or_child == stored_parent ? "parent" : "theFunctions";

        ST<var_info>* my_variable_library_ptr = _parent_or_child == stored_parent ? &this->my_variable_library_asked_by_parent : &this->my_variable_library_asked_by_functions;
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_variable_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Variablizing functionDec: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_variable_library_ptr = set_my_variable_library(_parent_or_child);
        }
        return *my_variable_library_ptr;
    } else {
        EM_error("Variablizing functionDec error: _parent_or_child param does not match parent or theFunctions");
        return ST<var_info>();
    }
}

ST<var_info> A_functionDec_::set_my_variable_library(AST_node_ *_parent_or_child) {
    EM_debug("Variablizing functionDec");
    
    if (_parent_or_child == stored_parent) {
        return ST<var_info>();
    } else if (_parent_or_child == theFunctions) {
        return stored_parent->get_my_variable_library(this);
    } else {
        EM_error("functionDec: child param does not match stored_parent or theFunctions");
        return ST<var_info>();
    }
}

ST<var_info> A_fundecList_::get_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _head || _parent_or_child == _tail) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _head ? "head": "tail");

        ST<var_info>* my_variable_library_ptr = _parent_or_child == stored_parent ? &this->my_variable_library_asked_by_parent : (_parent_or_child == _head ? &this->my_variable_library_asked_by_head : &this->my_variable_library_asked_by_tail);
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_variable_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Variablizing fundecList: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_variable_library_ptr = set_my_variable_library(_parent_or_child);
        }
        return *my_variable_library_ptr;
    } else {
        EM_error("Variablizing fundecList error: _parent_or_child param does not match _head or _tail");
        return ST<var_info>();
    }
}

ST<var_info> A_fundecList_::set_my_variable_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Variablizing fundecList from parent");
        // A letExp is asking about its declarations
        ST<var_info> dec_variable_library  = _head->get_my_variable_library(this);
        ST<var_info> tail_variable_library = _tail ? _tail->get_my_variable_library(this) : ST<var_info>();
        return MergeAndShadow(tail_variable_library, dec_variable_library);
    } else if (_parent_or_child == _head || _parent_or_child == _tail) {
        // A dec is asking about previous declarations or variables available to let
        ST<var_info> parent_variable_library = stored_parent->get_my_variable_library(this);
        if (_parent_or_child == _head) {
            EM_debug("Variablizing fundecList from head");
            return parent_variable_library;
        } else {
            EM_debug("Variablizing fundecList from tail");
            ST<var_info> dec_variable_library  = _head->get_my_variable_library(this);
            return MergeAndShadow(dec_variable_library, parent_variable_library);
        }
    } else {
        EM_error("Variablizing fundecList error: _parent_or_child param does not match _head or _tail");
        return ST<var_info>();
    }
}

ST<var_info> A_fundec_::set_my_variable_library(AST_node_ *child) {
    EM_debug("Variablizing fundec");

    ST<var_info> declared_variable_library = ST<var_info>();
    if (child == stored_parent) {
        // When the decList is asking about this fundec, nothing should be available
        return declared_variable_library;
    } else if (child == _body) {
        // Body of function definition is asking about any variables available such as function params and scope variables
        ST<var_info> parent_variable_library = stored_parent->get_my_variable_library(this);
        ST<var_info> param_variable_library = _params->get_my_variable_library(this);
        return MergeAndShadow(param_variable_library, parent_variable_library);
    } else {
        EM_error("fundec: child param does not match stored_parent or _body");
        return ST<var_info>();
    }
}

// Variable to track the number of fields in a function declaration
// three is base case for FP_alt, SP, ?
int fieldList_count = 3;

ST<var_info> A_fieldList_::set_my_variable_library(AST_node_ *parent) {
    EM_debug("Variablizing fieldList");

    if (parent != stored_parent) {
        EM_error("fieldList: should only be called by its parent");
    }

    ST<var_info> head_variable_library = _head->get_my_variable_library(this);
    ST<var_info> tail_variable_library = _tail ? _tail->get_my_variable_library(this) : ST<var_info>();

    // Reset the fieldList counter
    fieldList_count = 3;
    return FuseOneScope(head_variable_library, tail_variable_library);
}

ST<var_info> A_field_::set_my_variable_library(AST_node_ *parent) {
    EM_debug("Variablizing field");

    if (parent != stored_parent) {
        EM_error("field: should only be called by its parent");
    }
    if (is_name_there(_typ, type_library)) {
        type_info type_struct = lookup(_typ, type_library);
        Ty_ty this_type = type_struct.my_type();
        ST<var_info> param_variable_library = ST<var_info>(_name, var_info(this_type, fieldList_count, true));

        // Increment the fieldList counter for the next params if any
        fieldList_count++;
        return param_variable_library;
    } else {
        EM_error("field: Var " + Symbol_to_string(_name) + " in function declaration does not have type in type library.");
        return ST<var_info>();
    }
}