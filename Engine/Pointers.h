#pragma once

#pragma warning(disable: 4150)

#include "Assert.h"
#include "BasicTypes.h"
#include "FastOperator.h"
#include "framework.h"

template<typename T>
class Weak;

template<typename T>
class Shared;

template<typename T>
class EXPORT EnableSharedFromThis
{
    friend Shared<T>;

public:
    Shared<T> shared_from_this() const;

    Weak<T> weak_from_this() const;

    uint64* last_valid_counter = nullptr;
};

template<typename T>
concept CanEnableShared = requires
{
    { T::last_valid_counter } -> std::same_as<uint64*&>;
};

template<typename T>
class EXPORT Shared
{
    friend Weak<T>;
    friend EnableSharedFromThis<T>;
    
public:
    Shared()
        : ptr_(nullptr)
        , counter_(nullptr)
    {}

    Shared(T* ptr) requires CanEnableShared<T>
            : ptr_(ptr)
            , counter_(new uint64(1))
    {
        ptr->last_valid_counter = counter_;
    }

    Shared(T* ptr)
            : ptr_(ptr)
            , counter_(new uint64(1))
    {
    }

    Shared(T* ptr, uint64* counter)
        : ptr_(ptr)
        , counter_(counter)
    {
        if (ptr_) (*counter_)++;
    }

    Shared(const Shared& rhs)
        : ptr_(rhs.ptr_)
        , counter_(rhs.counter_)
    {
        if (ptr_) (*counter_)++;
    }

    ~Shared()
    {
        if (ptr_)
        {
            decref();
        }
    }

    Shared& operator=(const Shared& rhs)
    {
        if (&rhs == this) return *this;

        if (ptr_)
        {
            decref();
        }

        ptr_ = rhs.ptr_;
        counter_ = rhs.counter_;

        (*counter_)++;
        
        return *this;
    }

    template<typename T1>
    FORCEINLINE operator Shared<T1>() const
    {
        return Shared<T1>(ptr_, counter_);
    }

    uint64* get_counter() const
    {
        return counter_;
    }

    Shared& operator=(nullptr_t null)
    {
        if (ptr_)
        {
            decref();
        }

        ptr_ = nullptr;
        counter_ = nullptr;
        
        return *this;
    }

    void reset()
    {
        if (ptr_)
        {
            decref();
        }

        ptr_ = nullptr;
        counter_ = nullptr;
    }

    FORCEINLINE T* get() const { return ptr_; }

    FORCEINLINE T& operator*() { return *ptr_; }
    FORCEINLINE const T& operator*() const { return *ptr_; }

    FORCEINLINE T* operator->() { return ptr_; }
    FORCEINLINE T* operator->() const { return ptr_; }

    FORCEINLINE bool operator==(nullptr_t null) const { return ptr_ == nullptr; }
    FORCEINLINE bool operator!=(nullptr_t null) const { return ptr_ != nullptr; }
    FORCEINLINE operator bool() const { return ptr_; }

    template<typename T1>
    FORCEINLINE static Shared<T1> cast_to(const Shared& from)
    {
        if (T1* casted = dynamic_cast<T1*>(from.get()))
        {
            return Shared<T1>(casted, from.get_counter());
        }

        return nullptr;
    }

private:
    FORCEINLINE void decref() const
    {
        if (--(*counter_) == 0)
        {
            delete ptr_;
            delete counter_;
        }
    }
    
    T* ptr_;
    uint64* counter_;
};

template<class T, class... ArgTypes>
FORCEINLINE Shared<T> MakeShared(ArgTypes&&... Args)
{
    return Shared<T>(new T(std::forward<ArgTypes>(Args)...));
}

#define MakeSharedInternal(T, ...) (Shared<T>(new T(__VA_ARGS__)))

template<typename T>
class EXPORT Weak
{
    friend EnableSharedFromThis<T>;

public:
    Weak()
        : ptr_(nullptr)
        , counter_(nullptr)
    {}

    Weak(T* ptr, uint64* counter)
        : ptr_(ptr)
        , counter_(counter)
    {}

    Weak(const Shared<T>& rhs)
        : ptr_(rhs.ptr_)
        , counter_(rhs.counter_)
    {
    }

    template<typename T1>
    FORCEINLINE operator Weak<T1>() const
    {
        return Weak<T1>(ptr_, counter_);
    }

    Weak& operator=(const Weak& rhs)
    {
        if (&rhs == this) return *this;

        ptr_ = rhs.ptr_;
        counter_ = rhs.counter_;
        
        return *this;
    }

    Weak& operator=(nullptr_t null)
    {
        ptr_ = nullptr;
        counter_ = nullptr;
        
        return *this;
    }

    FORCEINLINE Shared<T> lock() const
    {
        return Shared<T>(ptr_, counter_);
    }

    FORCEINLINE void reset()
    {
        ptr_ = nullptr;
        counter_ = nullptr;
    }

    FORCEINLINE T* get() const { return ptr_; }

    FORCEINLINE T& operator*() { return *ptr_; }
    FORCEINLINE const T& operator*() const { return *ptr_; }

    FORCEINLINE T& operator->() { return *ptr_; }
    FORCEINLINE const T& operator->() const { return *ptr_; }

    FAST_OPERATOR(Weak, ==, ptr_);
    FAST_OPERATOR(Weak, <=, ptr_);
    FAST_OPERATOR(Weak, <, ptr_);
    FAST_OPERATOR(Weak, !=, ptr_);
    FAST_OPERATOR(Weak, >, ptr_);
    FAST_OPERATOR(Weak, >=, ptr_);

    FORCEINLINE bool operator==(nullptr_t null) const { return ptr_ == nullptr; }
    FORCEINLINE bool operator!=(nullptr_t null) const { return ptr_ != nullptr; }
    FORCEINLINE operator bool() const { return ptr_; }

private:
    T* ptr_;
    uint64* counter_;
};

template<typename T>
class EXPORT Reference
{
public:
    Reference()
        : ptr_(nullptr)
    {}

    Reference(T* ptr)
        : ptr_(ptr)
    {}

    ~Reference()
    {
        delete ptr_;
    }

    void reset(T* ptr)
    {
        delete ptr_;
        ptr_ = ptr;
    }

    FORCEINLINE void free()
    {
        ptr_ = nullptr;
    }

    FORCEINLINE T* get() const { return ptr_; }

    FORCEINLINE T& operator*() { return *ptr_; }
    FORCEINLINE const T& operator*() const { return *ptr_; }

    FORCEINLINE T* operator->() { return ptr_; }
    FORCEINLINE T* operator->() const { return ptr_; }

private:
    T* ptr_;
};

template <typename T>
Shared<T> EnableSharedFromThis<T>::shared_from_this() const
{
    AssertMsg(last_valid_counter != nullptr, "Memory", "Object was never constructed as shared");
    return Shared<T>((T*)this, last_valid_counter);
}

template <typename T>
Weak<T> EnableSharedFromThis<T>::weak_from_this() const
{
    AssertMsg(last_valid_counter != nullptr, "Memory", "Object was never constructed as shared");
    return Weak<T>((T*)this, last_valid_counter);
}

template<typename To, typename From>
FORCEINLINE To* cast(From* obj)
{
    return dynamic_cast<To*>(obj);
}

template<typename To, typename From>
FORCEINLINE Shared<To> cast(const Shared<From>& obj)
{
    return Shared<From>::template cast_to<To>(obj);
}

template<typename To, typename From>
FORCEINLINE Shared<const To> cast(const Shared<const From>& obj)
{
    return Shared<const From>::template cast_to<const To>(obj);
}

template<typename To, typename From>
FORCEINLINE Shared<To> cast(const Weak<From>& obj)
{
    return Shared<From>::template cast_to<To>(obj.lock());
}

template<typename T1, typename T2>
FORCEINLINE bool operator==(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() == rhs.get(); };

template<typename T1, typename T2>
FORCEINLINE bool operator<=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() <= rhs.get(); };

template<typename T1, typename T2>
FORCEINLINE bool operator<(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() < rhs.get(); };

template<typename T1, typename T2>
FORCEINLINE bool operator!=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() != rhs.get(); };

template<typename T1, typename T2>
FORCEINLINE bool operator>(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() > rhs.get(); };

template<typename T1, typename T2>
FORCEINLINE bool operator>=(const Shared<T1>& lhs, const Shared<T2>& rhs) { return lhs.get() >= rhs.get(); };
