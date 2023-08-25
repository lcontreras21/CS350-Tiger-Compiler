#include "AST.h"
#include "ST.h"

/*
	Functions used by parents to find out the function library
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
				A_forExp_
				A_breakExp_
				A_seqExp_
		A_var_
			A_simpleVar_
			A_fieldVar_
			A_subscriptVar_
		A_dec_
			A_varDec_**
			A_functionDec_**
			A_typeDec_
			A_decList_**
		A_fundec_**
		A_expList_
		A_efield_
		A_efieldList_
		A_fundecList_
		A_namety_
		A_nametyList_
		A_field_
		A_fieldList_
		A_ty_
			A_nameTy_
			A_arrayty_
			A_recordty_
*/

//--------------------------------------------------------------------------------

ST<function_info> AST_node_::set_my_function_library(AST_node_ *child) {
    EM_debug("Functionalizing AST_node_");
    if (child == stored_parent) {
        EM_warning("Functionalizing AST_node_: got stored_parent as child param");
    }
    return stored_parent->get_my_function_library(this);
}

ST<function_info> A_root_::set_my_function_library(AST_node_ *child) {
    EM_debug("Functionalizing root, returning Tiger Library");
    return tiger_library;  // Return the full set of Tiger standard functions
}

ST<function_info> A_letExp_::get_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _decs || _parent_or_child == _body) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _decs ? "decs": "body");
        EM_debug("Functionalizing letExp #" + get_my_let_number_s() + ": getting library for " + who);

        ST<function_info>* my_function_library_ptr;
        if (_parent_or_child == stored_parent) {
            my_function_library_ptr = &this->my_function_library_asked_by_parent;
        } else if (_parent_or_child == _decs) {
            my_function_library_ptr = &this->my_function_library_asked_by_decs;
        } else {
            my_function_library_ptr = &this->my_function_library_asked_by_body;
        }
        if (is_name_there(to_Symbol("Empty"), *my_function_library_ptr)) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing letExp error: _parent_or_child param does not match stored_parent, _decs, _body");
        return ST<function_info>();
    }
}

ST<function_info> A_letExp_::set_my_function_library(AST_node_ *child) {
    if (child == stored_parent) {
        EM_debug("Functionalizing letExp #" + get_my_let_number_s() + " when asked by parent to be empty");
        return ST<function_info>();
    } else if (child == _decs || child == _body) {
        string who = child == _decs ? "decs" : "body";
        EM_debug("Functionalizing letExp #" + get_my_let_number_s() + " when asked by " + who);

        EM_debug("Functionalizing letExp #" + get_my_let_number_s() + " getting parent library");
        ST<function_info> parent_function_library = stored_parent->get_my_function_library(this);

        if (child == _decs) {
            return parent_function_library;
        } else {
            ST<function_info> dec_function_library = _decs ? _decs->get_my_function_library(this) : ST<function_info>();
            return MergeAndShadow(dec_function_library, parent_function_library);
        }
    } else {
        EM_error("Functionalizing letExp #" + get_my_let_number_s() + " error: child does not match _decs, or _body");
        return ST<function_info>();
    }
}

ST<function_info> A_decList_::get_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _head || _parent_or_child == _tail) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _head ? "head": "tail");

        ST<function_info>* my_function_library_ptr = _parent_or_child == stored_parent ? &this->my_function_library_asked_by_parent : (_parent_or_child == _head ? &this->my_function_library_asked_by_head : &this->my_function_library_asked_by_tail);
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_function_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Functionalizing decList: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing decList error: _parent_or_child param does not match _head or _tail");
        return ST<function_info>();
    }
}

ST<function_info> A_decList_::set_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Functionalizing decList from parent");
        // A letExp is asking about its declarations
        ST<function_info> dec_function_library  = _head->get_my_function_library(this);
        ST<function_info> tail_function_library = _tail ? _tail->get_my_function_library(this) : ST<function_info>();
        return MergeAndShadow(tail_function_library, dec_function_library);
    } else if (_parent_or_child == _head || _parent_or_child == _tail) {
        // A dec is asking about previous declarations
        ST<function_info> parent_function_library = stored_parent->get_my_function_library(this);
        if (_parent_or_child == _head) {
            EM_debug("Functionalizing decList when asked by head");
            return parent_function_library;
        } else {
            EM_debug("Functionalizing decList when asked by tail");
            ST<function_info> dec_function_library  = _head->get_my_function_library(this);
            return MergeAndShadow(dec_function_library, parent_function_library);
        }
    } else {
        EM_error("Functionalizing decList error: _parent_or_child param does not match _head or _tail");
        return ST<function_info>();
    }
}

ST<function_info> A_varDec_::get_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _init) {
        string who = _parent_or_child == stored_parent ? "parent" : "init";

        ST<function_info>* my_function_library_ptr = _parent_or_child == stored_parent ? &this->my_function_library_asked_by_parent : &this->my_function_library_asked_by_init;
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_function_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Functionalizing varDec: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing varDec error: _parent_or_child param does not match _init");
        return ST<function_info>();
    }
}

ST<function_info> A_varDec_::set_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Functionalizing varDec from parent");
        ST<function_info> my_function_library  = ST<function_info>();
        return my_function_library;
    } else if (_parent_or_child == _init) {
        // _init is asking about functions in its scope
        ST<function_info> parent_function_library = stored_parent->get_my_function_library(this);

        EM_debug("Functionalizing varDec from _init");
        return parent_function_library;
    } else {
        EM_error("Functionalizing varDec error: _parent_or_child param does not match _init");
        return ST<function_info>();
    }
}

ST<function_info> A_functionDec_::get_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == theFunctions) {
        ST<function_info>* my_function_library_ptr = _parent_or_child == stored_parent ? &this->my_function_library_asked_by_parent : &this->my_function_library_asked_by_functions;
        if (is_name_there(to_Symbol("Empty"), *my_function_library_ptr)) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing functionDec error: _parent_or_child does not match parent or theFunctions");
        return ST<function_info>();
    }
}

ST<function_info> A_functionDec_::set_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Functionalizing functionDec when asked by parent");
        return theFunctions->get_my_function_library(this);
    } else if (_parent_or_child == theFunctions) {
        EM_debug("Functionalizing functionDec getting parent library");
        ST<function_info> parent_function_library = stored_parent->get_my_function_library(this);
        return parent_function_library;
    } else {
        EM_error("Functionalizing functionDec error: _parent_or_child does not match parent or theFunctions");
        return ST<function_info>();
    }
}

ST<function_info> A_fundecList_::get_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent || _parent_or_child == _head || _parent_or_child == _tail) {
        string who = _parent_or_child == stored_parent ? "parent" : (_parent_or_child == _head ? "head": "tail");

        ST<function_info>* my_function_library_ptr = _parent_or_child == stored_parent ? &this->my_function_library_asked_by_parent : (_parent_or_child == _head ? &this->my_function_library_asked_by_head : &this->my_function_library_asked_by_tail);
        bool is_empty = is_name_there(to_Symbol("Empty"), *my_function_library_ptr);
        string action = is_empty ? "setting" : "getting";
        EM_debug("Functionalizing fundecList: " + action + " library when asked by " + who);
        if (is_empty) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing fundecList error: _parent_or_child param does not match _head or _tail");
        return ST<function_info>();
    }
}

ST<function_info> A_fundecList_::set_my_function_library(AST_node_ *_parent_or_child) {
    if (_parent_or_child == stored_parent) {
        EM_debug("Functionalizing fundecList from parent");
        // A letExp is asking about its declarations
        ST<function_info> dec_function_library  = _head->get_my_function_library(this);
        ST<function_info> tail_function_library = _tail ? _tail->get_my_function_library(this) : ST<function_info>();
        return MergeAndShadow(tail_function_library, dec_function_library);
    } else if (_parent_or_child == _head || _parent_or_child == _tail) {
        // A fundec is asking about previous fundecs or about fundecs in current fundeclist (functionDec group)
        ST<function_info> parent_function_library = stored_parent->get_my_function_library(this);
        if (_parent_or_child == _head) {
            EM_debug("Functionalizing fundecList when asked by head");
            ST<function_info> tail_function_library = _tail ? _tail->get_my_function_library(this) : ST<function_info>();
            ST<function_info> dec_function_library  = _head->get_my_function_library(this);
            return MergeAndShadow(tail_function_library, MergeAndShadow(dec_function_library, parent_function_library));
        } else {
            EM_debug("Functionalizing fundecList when asked by tail");
            ST<function_info> dec_function_library  = _head->get_my_function_library(this);
            return MergeAndShadow(dec_function_library, parent_function_library);
        }
    } else {
        EM_error("Functionalizing fundecList error: _parent_or_child param does not match _head or _tail");
        return ST<function_info>();
    }
}

ST<function_info> A_fundec_::get_my_function_library(AST_node_ *_parent_or_child) {
    EM_debug("Functionalizing fundec '" + Symbol_to_string(_name) + "': getting library");
    if (_parent_or_child == stored_parent || _parent_or_child == _body) {
        ST<function_info>* my_function_library_ptr = _parent_or_child == stored_parent ? &this->my_function_library_asked_by_parent : &this->my_function_library_asked_by_body;
        if (is_name_there(to_Symbol("Empty"), *my_function_library_ptr)) {
            *my_function_library_ptr = set_my_function_library(_parent_or_child);
        }
        return *my_function_library_ptr;
    } else {
        EM_error("Functionalizing fundec '" + Symbol_to_string(_name) + "' error: _parent_or_child param does not match stored_parent or _body");
        return ST<function_info>();
    }
}


ST<function_info> A_fundec_::set_my_function_library(AST_node_ *_parent_or_child) {
    // Get return type from type library, if there
    Ty_ty my_return_type = 0;
    if (is_name_there(_result, type_library)) {
        type_info type_struct = lookup(_result, type_library);
        my_return_type = type_struct.my_type();
    } else {
        EM_error("Functionalizing fundec error: '" + Symbol_to_string(_name) + "' does not have a valid return type");
        return ST<function_info>();
    }

    if (_parent_or_child == stored_parent) {
        Ty_fieldList param_types = 0;
        if (_params != 0) {
            param_types = _params->init_Ty_fieldList();
        }
        EM_debug("Functionalizing fundec '" + Symbol_to_string(_name) + "' when asked by parent");
        function_count++;
        bool is_tiger_function = false;
        ST<function_info> declared_function_library = ST<function_info>(_name, function_info(Ty_Function(my_return_type, param_types), function_count, is_tiger_function));
        return declared_function_library;
    } else if(_parent_or_child == _body) {
        EM_debug("Functionalizing fundec '" + Symbol_to_string(_name) + "' when asked by init");
	    return stored_parent->get_my_function_library(this);
    } else {
        EM_error("Functionalizing fundec '" + Symbol_to_string(_name) + "' error: _parent_or_child param does not match stored_parent or _body");
        return ST<function_info>();
    }
}


/*
From Appel 'Modern Compiler Implementation in C' p122
Mutually recursive functions are handled similarly. The first pass gathers information about the header of each function
 (function name, formal parameter list, return type) but leaves the bodies of the functions untouched.
In this pass, the types of the formal parameters are needed, but not their names (which cannot be seen from outside
the function). [handled by function_library pass through]

The second pass processes the bodies of all functions in the mutually recursive declaration, taking advantage of the
environment augmented with all the function headers. For each body, the formal parameter list is processed again,
this time entering the parameters as varDecs in the value environment.
*/

Ty_fieldList A_fieldList_::init_Ty_fieldList() {
	if (_tail == 0) {
		return Ty_FieldList(_head->init_Ty_field(), 0);
	} else {
		return Ty_FieldList(_head->init_Ty_field(), _tail->init_Ty_fieldList());	
	}
}

Ty_field A_field_::init_Ty_field() {
	// Check if type _typ is in allowed or declared types
	if (is_name_there(_typ, type_library)) { 
		type_info type_struct = lookup(_typ, type_library);
		Ty_ty this_type = type_struct.my_type();
		return Ty_Field(_name, this_type);
	} else {
		EM_error("Init_Ty_field: Var " + Symbol_to_string(_name) + " in function declaration does not have type in" +
		         "type library.");
		return Ty_Field(_name, Ty_Error());
	}
}