#pragma once

#include <functional>
#include <map>
#include <stdexcept>

template<typename... InTypes>
class Delegate
{
public:
	// Add member function binding to execution list
	template<typename T>
	void Bind(T* obj, void(T::* func)(InTypes...))
	{
		bindings[{obj, func}] = std::bind(func, obj);
	}

	// Add static or non-member function to execution list
	void Bind(void(* func)(InTypes...))
	{
		bindings[{nullptr, func}] = std::bind(func);
	}

	// Execute all bindings
	void Call(InTypes... args)
	{
		auto bindingsCopy = bindings;

		for (auto& binding : bindingsCopy)
		{
			binding.second(std::forward<InTypes>(args)...);
		}
	}

	// Execute all bindings
	void operator()(InTypes... args)
	{
		Call(std::forward<InTypes>(args)...);
	}

	template<typename T>
	void Unbind(T* obj, void(T::* func)(InTypes...))
	{
		bindings.erase({ obj, func });
	}

	void Unbind(void(* func)(InTypes...))
	{
		bindings.erase({ nullptr, func });
	}

private:
	struct ObjFunc
	{
		void* obj = nullptr;
		void* func = nullptr;

		bool operator==(const ObjFunc& rhs) const
		{
			return obj == rhs.obj && func == rhs.func;
		}

		bool operator<(const ObjFunc& rhs) const
		{
			return obj < rhs.obj || func < rhs.func;
		}
	};

	std::map<ObjFunc, std::function<void(InTypes...)>> bindings;
};

template<typename... ArgTypes>
class Action
{
public:
	template<typename T>
	void Bind(T* obj, void(T::* func)(ArgTypes...))
	{
		binding = std::bind(func, obj);
		isBound = true;
	}

	void Bind(void(* func)(ArgTypes...))
	{
		binding = std::bind(func);
		isBound = true;
	}

	void Unbind()
	{
		binding.obj = binding.func = nullptr;
		isBound = false;
	}

	void Call(ArgTypes... args)
	{
		if (isBound)
		{
			binding(std::forward<ArgTypes>(args)...);
		}
	}

	void operator()(ArgTypes... args)
	{
		Call(std::forward<ArgTypes>(args)...);
	}

    [[nodiscard]] bool IsBound() const
	{
		return isBound;
	}

private:
	std::function<void(ArgTypes...)> binding;
	bool isBound = false;
};

template<typename RetType, typename... ArgTypes>
class Function
{
public:
	template<typename T>
	void Bind(T* obj, void(T::* func)(ArgTypes...))
	{
		binding = std::bind(func, obj);
		isBound = true;
	}

	void Bind(void(* func)(ArgTypes...))
	{
		binding = std::bind(func);
		isBound = true;
	}

	void Unbind()
	{
		binding.obj = binding.func = nullptr;
	}

	RetType Call(ArgTypes... args)
	{
		if (isBound)
		{
			return binding(std::forward<ArgTypes>(args)...);
		}
		else
		{
			throw new std::runtime_error("Function is not bound!");
		}
	}

	RetType operator()(ArgTypes... args)
	{
		return Call(std::forward<ArgTypes>(args)...);
	}

    [[nodiscard]] bool IsBound() const
	{
		return isBound;
	}

private:
	std::function<RetType(ArgTypes...)> binding;
	bool isBound = false;
};
