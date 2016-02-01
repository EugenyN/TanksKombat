#ifndef  _MULTIFUNCTION_HPP_
#define  _MULTIFUNCTION_HPP_

// A multicast function type for C++
// https://github.com/klmr/multifunction

#include <functional>
#include <vector>

namespace detail {

	template <typename R, typename... Args>
	struct multifunction_traits 
	{
		typedef R return_type;
	};

	template <typename R, typename Arg>
	struct multifunction_traits<R, Arg> 
	{
		typedef R return_type;
		typedef Arg argument_type;
	};

	template <typename R, typename Arg1, typename Arg2>
	struct multifunction_traits<R, Arg1, Arg2> 
	{
		typedef R return_type;
		typedef Arg1 first_argument_type;
		typedef Arg2 second_argument_type;
	};

	template <typename R, typename... Args>
	struct call_helper 
	{
		static R call(std::vector<std::function<R(Args...)>> const& listeners, Args... args) 
		{
			R ret;
			for (auto listener : listeners)
				ret = listener(args...);
			return ret;
		}
	};

	template <typename... Args>
	struct call_helper<void, Args...> 
	{
		static void call(std::vector<std::function<void(Args...)>> const& listeners, Args... args)
		{
			for (auto listener : listeners)
				listener(args...);
		}
	};

} // namespace detail

template <typename R, typename... Args>
class multifunction;

template <typename R, typename... Args>
class multifunction<R(Args...)> : public detail::multifunction_traits<R, Args...> 
{
public:

	// Uniquely identifies a listener in this multifunction.
	// This is an opaque type, meaning it can only be created and accessed
	// from within this class. The outside world can only store them.
	class function_token
	{
		friend class multifunction;

		function_token(size_t id) noexcept 
			: id(static_cast<unsigned>(id)) 
		{ }

		// We assume that numeric_limits<unsigned>::max() tokens are enough.
		unsigned id;
	};

	multifunction() = default;
	multifunction(multifunction const&) = default;
	multifunction(multifunction&&) = default;
	multifunction& operator =(multifunction const&) = default;
	multifunction& operator =(multifunction&&) = default;
	~multifunction() = default;

	template <typename F>
	function_token operator +=(F listener) 
	{
		listeners.push_back(listener);
		token_lookup.push_back(listeners.size() - 1);
		return function_token{ token_lookup.size() - 1 };
	}

	void operator -=(function_token token) 
	{
		auto i = token_lookup[token.id];

		if (i == NIL)
			return;

		// Adjust token_lookup positions which have shifted.
		for (auto& token_index : token_lookup)
			if (token_index > i and token_index != NIL)
				--token_index;

		listeners.erase(listeners.begin() + i);
		token_lookup[token.id] = NIL;
	}

	R operator ()(Args... args) const 
	{
		return detail::call_helper<R, Args...>::call(listeners, args...);
	}

private:

	std::vector<std::function<R(Args...)>> listeners;
	std::vector<std::size_t> token_lookup;
	static constexpr std::size_t NIL = -1;
};

#endif // _MULTIFUNCTION_HPP_
