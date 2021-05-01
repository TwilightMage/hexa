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
	void bind(T* obj, void(T::* func)(InTypes...))
	{
		bindings_[obj_func(obj, func_id::construct(func))] = [obj, func](InTypes... args) -> void
		{
			(obj->*func)(args...);
		};
	}

	// Add static or non-member function to execution list
	void bind(void(* func)(InTypes...))
	{
		bindings_[obj_func(nullptr, func_id::construct(func))] = [func](InTypes... args) -> void
		{
			(*func)(args...);
		};
	}

	// Execute all bindings
	void call(InTypes... args)
	{
		auto bindings_copy = bindings_;

		for (auto& binding : bindings_copy)
		{
			binding.second(std::forward<InTypes>(args)...);
		}
	}

	// Execute all bindings
	void operator()(InTypes... args)
	{
		call(std::forward<InTypes>(args)...);
	}

	template<typename T>
	void unbind(T* obj, void(T::* func)(InTypes...))
	{
		bindings_.erase(obj_func(obj, func_id::construct(func)));
	}

	template<typename T>
	void unbind_object(T* obj)
	{
		for(typename std::map<obj_func, std::function<void(InTypes...)>>::const_iterator iter = bindings_.begin(); iter != bindings_.end(); )
		{
			if (iter->first.obj == obj)
			{
				iter = bindings_.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void unbind(void(* func)(InTypes...))
	{
		bindings_.erase(obj_func(nullptr, func_id::construct(func)));
	}

private:
	struct func_id
	{
		byte* data = nullptr;
		uint size = 0;

		func_id(byte* data, uint size)
			: data(data)
			, size(size)
		{
		}

		func_id(const func_id& rhs)
			: data(new byte[rhs.size])
			, size(rhs.size)
		{
			memcpy(data, rhs.data, size);
		}

		~func_id()
		{
			delete[] data;
		}

		func_id& operator=(const func_id& rhs)
		{
			if (this == &rhs) return *this;
			
			delete[] data;

			data = new byte[rhs.size];
			memcpy(data, rhs,data, rhs.size);
			size = rhs.size;

			return *this;
		}

		bool operator==(const func_id& rhs) const
		{
			return size == rhs.size && memcmp(data, rhs.data, size) == 0;
		}
		
		bool operator<(const func_id& rhs) const
		{
			if (size < rhs.size) return true;
			for (uint i = 0; i < size; i++)
			{
				if (data[i] < rhs.data[i]) return true;
			}
			return false;
		}

		template<typename FSIG>
		static func_id construct(FSIG func)
		{
			uint size = sizeof(FSIG);
			byte* data = new byte[size];
			memcpy(data, &func, size);
			return func_id(data, size);
		}
	};
	
	struct obj_func
	{
		void* obj;
		func_id func;

		obj_func(void* obj, const func_id& func)
			: obj(obj)
			, func(func)
		{
		}
		
		bool operator==(const obj_func& rhs) const
		{
			return obj == rhs.obj && func == rhs.func;
		}

		bool operator<(const obj_func& rhs) const
		{
			return obj < rhs.obj || func < rhs.func;
		}
	};

	std::map<obj_func, std::function<void(InTypes...)>> bindings_;
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
