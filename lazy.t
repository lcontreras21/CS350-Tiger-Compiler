/*
 * Implementation file for templated lazy variable class.
 *
 *  Since most C++ compilers reqire that the implementation of templated code be #inlude'd rather than compiled separately,
 *  I've put it in this ".t" file, which I #include from the ".h" file to avoid visual clutter there,
 *  rather than putting it in a separate ".cc" file that includes lazy.h.
 */

template <class T> lazy<T>::lazy(std::function<T ()> init_function)
{
	current_state = empty;
	my_initializer = init_function;
}

template <class T> const T &lazy<T>::get() const
{
	if (is_ready()) {
		return *my_ptr;
	} else if (is_running()) {
		throw cyclic_definition(*this);
	} else {
		current_state = running;
		my_ptr = new T(my_initializer());
		current_state = ready;
		return *my_ptr;
	}
}


template <class T> bool lazy<T>::is_empty() const	 { return current_state == empty; }
template <class T> bool lazy<T>::is_running() const	 { return current_state == running; }
template <class T> bool lazy<T>::is_ready() const	 { return current_state == ready; }



/* Implementation stuff for exception class */

template<class T> lazy<T>::cyclic_definition::cyclic_definition(const lazy<T> &what) : what_wasnt_ready(what)
{
}
