// this file should only be included in synthesize.cc,
//  possibly repeatedly, each time after some_type has been #define'd

/*
   HERE is the introduction to pointers to functions from lecture 3/15/06:
   
   untested code showing functions returning pointers vs pointers to functions
   
   char *getstring();
   char first_letter_typed = * (getstring());
   
   add(4,5) // 9
   sub(5,4) // 1
   int (*which_op)(); // creates which_op pointer to a function
   which_op = &add;
   cout << (* which_op) (5,4);
 */
 
 
 /*
  * The general approach to "synthesize" is this:
  * 
  * Every non-leaf node will call "get_what_from_children" on all non-trivial
  *  children (i.e. things of unknown size or interesting structure, but not
  *  just a symbol or an integer value).
  * 
  * Every leaf node will return default_value_for_leaves if synthesize is
  *  called on it; to avoid getting the default, override the
  *  get_what_from_children for that kind of leaf so that it doesn't call
  *  back to synthesize, but rather just returns something else.
  *
  * The "get_what_from_children" functions should then either
  *  + call "synthesize" again to traverse _their_ children, if not a leaf
  *    (this is typically done in a superclass version of the function), or
  *  + just compute and return the result _without_ calling synthesize,
  *    if a leaf or something with only trivial data fields (e.g. a symbol)
  * AND also do anything else important, such as store a memo-ized copy of the value
  *  for later use.
  */


some_type AST_node_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	// This version should only be called for AST leaf nodes;
	//  it is supposed to be overridden below for every other kind of node
	return default_value_for_leaves;
}


some_type A_recordExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_fields)
		return (_fields->*get_what_from_children)();
	else
		return default_value_for_leaves;
}

some_type A_arrayExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	assert(_size != 0);
	assert(_init != 0);
	return (*how_to_combine)((_size->*get_what_from_children)(),
							(_init->*get_what_from_children)());
}

some_type A_varExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (_var->*get_what_from_children)();
}

some_type A_opExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	some_type l_result = (_left->*get_what_from_children)();
	some_type r_result = (_right->*get_what_from_children)();
	return (*how_to_combine)(l_result, r_result);
}

some_type A_assignExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (*how_to_combine)((_var->*get_what_from_children)(),
							(_exp->*get_what_from_children)());
}

some_type A_letExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_decs and _body)
		return (*how_to_combine)((_decs->*get_what_from_children)(),
								(_body->*get_what_from_children)());
	else if (_body)
		return (_body->*get_what_from_children)();
	else if (_decs)
		return (_decs->*get_what_from_children)();
	else
		assert (_decs == 0 and _body == 0);  // nothin' to do...
		return default_value_for_leaves; // shut up the C++ compiler's warnings
}

some_type A_callExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_args)
		return (_args->*get_what_from_children)();
	else
		return default_value_for_leaves;
}

some_type A_ifExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_elsee)
		return (*how_to_combine)((_test->*get_what_from_children)(),
							(*how_to_combine)((_then->*get_what_from_children)(),
										      (_elsee->*get_what_from_children)()));
	else
		return (*how_to_combine)((_test->*get_what_from_children)(),
							((_then->*get_what_from_children)()));
}


some_type A_forExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (*how_to_combine)((_lo->*get_what_from_children)(),
							(*how_to_combine)((_hi->*get_what_from_children)(),
										      (_body->*get_what_from_children)()));
}
some_type A_seqExp_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_seq)
		return (_seq->*get_what_from_children)();
	else
		return default_value_for_leaves;
}

some_type A_fieldVar_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (_var->*get_what_from_children)();
}

some_type A_subscriptVar_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (*how_to_combine)((_var->*get_what_from_children)(),
							  (_exp->*get_what_from_children)());
}


some_type A_expList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
							(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}

some_type A_efield_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (_exp->*get_what_from_children)();
}

some_type A_efieldList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
							(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}

some_type A_decList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
								(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}

some_type A_varDec_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (_init->*get_what_from_children)();
}

some_type A_fundecList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
							(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}

some_type A_fundec_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_params)
		return (*how_to_combine)((_params->*get_what_from_children)(),
								(_body->*get_what_from_children)());
	else
		return (_body->*get_what_from_children)();
}

some_type A_nametyList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
								(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}

some_type A_namety_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	return (_ty->*get_what_from_children)();
}

some_type A_recordty_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_record)
		return (_record->*get_what_from_children)();
	else
		return default_value_for_leaves;
}

some_type A_fieldList_::synthesize(some_type (AST_node_::*get_what_from_children)(),
					          some_type (*how_to_combine)(const some_type &, const some_type &),
					          const some_type &default_value_for_leaves)
{
	if (_tail)
		return (*how_to_combine)((_head->*get_what_from_children)(),
							(_tail->*get_what_from_children)());
	else
		return (_head->*get_what_from_children)();
}
