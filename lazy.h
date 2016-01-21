#if ! defined LAZY_H
#define LAZY_H

/*
 * Template for a "lazy variable" (like in Scala)  that can be in 3 states:
 *  when created, it is in the state "empty"
 *  it can then be set to "running" as we start to compute its value,
 *  and then made "ready" via "set".
 *
 * Started April 2014 by Dave W (davew@cs.haverford.edu), updated January 2016, after Richard and Dylan's theses in 2015
 *
 * WARNING: the "cyclic_definition" exception contains a reference to the lazy variable that wasn't ready,
 *	    which could be a problem if lazy variables are used as local variables and the exception
 *	    propogates past the scope of the local. This should not be a problem for data fields
 *	    of AST nodes in CS340
 */

#include <functional>

// Note that T must have a constructor that does not require arguments
template <class T> class lazy {
public:
	lazy<T>(std::function<T ()> init_function);

	const T &get() const;	// get the value if it has been set, or throw a lazy::cyclic_definition exception if not yet set

	class cyclic_definition { // exception type ... while defining object a, we needed b, which needed ... which needed a
	public:
		const lazy<T> &what_wasnt_ready;
	private:
		friend class lazy<T>;
		cyclic_definition(const lazy<T> &what);
	};

private:
	// what state is it currently in?
	bool is_empty() const;
	bool is_running() const;
	bool is_ready() const;

	std::function<T ()> my_initializer;
	mutable enum { empty, running, ready } current_state;
	mutable T *my_ptr;
};


/* #include the template instantiation here, since most C++ compliers require that we do so */
#include "lazy.t"

#endif
