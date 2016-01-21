// This file is meant to be included only in pinherit.cc, with some_type defined various ways

/*
 * The basic idea here is quite similar to "synthesize", in that we call a function
 *  on each child, and that function is expected to call pinherit on its node to recurse
 * The main differences are:
 *  The attribute is a parameter as we call _down_ the tree rather than a returned
 *    value as we go back up.
 *  We don't need to have a function to combine the results of the children
 *  We don't need a default for leaves (though the first call to the attribute
 *    defining function used for "heres_yer_attribute", at the root, must be
 *    given the value at the root of the AST.
 *  At a leaf node we do nothing unless overridden
 */
 

void AST_node_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const  some_type &attribute_fer_you)
{
	// This version should only be called for AST leaf nodes;
	//  it is supposed to be overridden below for every other kind of node
}


void A_recordExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const  some_type &attribute_fer_you)
{
	if (_fields)	(_fields->*heres_yer_attribute)(attribute_fer_you);
}

void A_arrayExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	assert(_size != 0);
	assert(_init != 0);
	(_size->*heres_yer_attribute)(attribute_fer_you);
	(_init->*heres_yer_attribute)(attribute_fer_you);
}

void A_varExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_var->*heres_yer_attribute)(attribute_fer_you);
}

void A_opExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_left->*heres_yer_attribute)(attribute_fer_you);
	(_right->*heres_yer_attribute)(attribute_fer_you);
}

void A_assignExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_var->*heres_yer_attribute)(attribute_fer_you);
	(_exp->*heres_yer_attribute)(attribute_fer_you);
}

void A_letExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	if (_decs)	(_decs->*heres_yer_attribute)(attribute_fer_you);
	if (_body)	(_body->*heres_yer_attribute)(attribute_fer_you);
}

void A_callExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	if (_args)	(_args->*heres_yer_attribute)(attribute_fer_you);
}

void A_ifExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_test->*heres_yer_attribute)(attribute_fer_you);
	(_then->*heres_yer_attribute)(attribute_fer_you);
	if (_elsee)	(_elsee->*heres_yer_attribute)(attribute_fer_you);
}


void A_forExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_lo->*heres_yer_attribute)(attribute_fer_you);
	(_hi->*heres_yer_attribute)(attribute_fer_you);
	(_body->*heres_yer_attribute)(attribute_fer_you);
}

void A_seqExp_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	if (_seq)	(_seq->*heres_yer_attribute)(attribute_fer_you);
}

void A_fieldVar_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_var->*heres_yer_attribute)(attribute_fer_you);
}

void A_subscriptVar_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_var->*heres_yer_attribute)(attribute_fer_you);
	(_exp->*heres_yer_attribute)(attribute_fer_you);
}


void A_expList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}

void A_efield_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_exp->*heres_yer_attribute)(attribute_fer_you);
}

void A_efieldList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}

void A_decList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}

void A_varDec_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_init->*heres_yer_attribute)(attribute_fer_you);
}

void A_fundecList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}

void A_fundec_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	if (_params)	(_params->*heres_yer_attribute)(attribute_fer_you);
	(_body->*heres_yer_attribute)(attribute_fer_you);
}

void A_nametyList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}

void A_namety_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_ty->*heres_yer_attribute)(attribute_fer_you);
}

void A_recordty_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	if (_record)	(_record->*heres_yer_attribute)(attribute_fer_you);
}

void A_fieldList_::pinherit(void (AST_node_::*heres_yer_attribute)(const some_type &), const some_type &attribute_fer_you)
{
	(_head->*heres_yer_attribute)(attribute_fer_you);
	if (_tail)	(_tail->*heres_yer_attribute)(attribute_fer_you);
}
