#pragma once

#include <functional>
#include <stdexcept>
#include <vector>

#include "Array.h"
#include "BasicTypes.h"

template<typename T>
class List : public Array<T>
{
public:
    using Array<T>::begin;
    using Array<T>::end;

    using Array<T>::get_data;
    using Array<T>::operator==;
    using Array<T>::operator!=;

    List()
    {
    }

    List(const List& rhs)
        : List(rhs.inner_, rhs.length())
    {
    }

    List(T* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = std::move(inner[i]);
            }
        }
    }

    List(const T* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = std::move(inner[i]);
            }
        }
    }

    List(const std::vector<T>& vec)
        : List(vec.data(), static_cast<uint>(vec.size()))
    {
    }

    List(const std::initializer_list<T>& il)
        : List(static_cast<const T*>(il.begin()), static_cast<uint>(il.size()))
    {
    }

    List(uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            this->inner_ = new T[allocated_length_];
            for (uint i = 0; i < length; i++)
            {
                this->inner_[i] = T();
            }
        }
    }

    template<typename ...Items>
    static List of(Items... items)
    {
        return {{items...}};
    }

    static List generate(uint size, std::function<T(uint index)> generator)
    {
        auto result = List(size);
        for (uint i = 0; i < size; i++)
        {
            result[i] = generator(i);
        }
        return result;
    }

    List& operator=(const List& rhs)
    {
        if (this == &rhs) return *this;

        delete[] inner_;

        length_ = rhs.length_;
        allocated_length_ = rhs.allocated_length_;

        if (length_ > 0)
        {
            inner_ = new T[allocated_length_];
            for (uint i = 0; i < length_; i++)
            {
                inner_[i] = std::move(rhs.inner_[i]);
            }
        }
        else
        {
            inner_ = nullptr;
        }

        return *this;
    }

    void Add(const T& item)
    {
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        inner_[length_++] = std::move(item);
    }

    template<typename... Items>
    void AddMany(const T& first, const Items&... rest)
    {
        Add(first);

        if constexpr (sizeof...(rest) > 0)
        {
            AddMany(rest...);
        }
    }

    void AddMany(const List<T>& items)
    {
        if (length_ + items.length() > allocated_length_)
        {
            reallocate(get_allocate_size(length_ + items.length()));
        }
        
        for (const auto& item : items)
        {
            inner_[length_++] = std::move(item);
        }
    }

    void add_unique(const T& item)
    {
        if (length_ > 0)
        {
            for (uint i = 0; i < length_; i++)
            {
                if (inner_[i] == item) return;;
            }
        }

        Add(item);
    }

    bool Contains(const T& item) const
    {
        if (length_ == 0) return false;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return true;
        }
        return false;
    }

    int IndexOf(const T& item) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return i;
        }
        return -1;
    }

    int IndexOf(std::function<bool(const T& item)> predicate) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return i;
        }
        return -1;
    }

    void RemoveAt(uint index)
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        if (index < length_ - 1)
        {
            for (uint i = index; i < length_ - 1; i++)
            {
                inner_[i] = std::move(inner_[i + 1]);
            }
        }

        inner_[length_ - 1] = T();
        
        --length_;
    }

    void Remove(const T& item)
    {
        uint offset = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (i + offset < length_)
            {
                while (inner_[i + offset] == item && i + offset < length_) ++offset;
                if (offset > 0)
                {
                    if (i + offset < length_)
                    {
                        inner_[i] = std::move(inner_[i + offset]);
                    }
                    else
                    {
                        inner_[i] = T();
                    }
                }
            }
            else
            {
                inner_[i] = T();
            }
        }

        length_ -= offset;
    }

    void Remove(bool(* predicate)(const T&))
    {
        uint offset = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (i + offset < length_)
            {
                while (predicate(inner_[i + offset]) && i + offset < length_) ++offset;
                if (offset > 0)
                {
                    if (i + offset < length_)
                    {
                        inner_[i] = std::move(inner_[i + offset]);
                    }
                    else
                    {
                        inner_[i] = T();
                    }
                }
            }
            else
            {
                inner_[i] = T();
            }
        }

        length_ -= offset;
    }

    T& operator[](uint index)
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    const T& operator[](uint index) const
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    T& first()
    {
        return operator[](0);
    }

    const T& first() const
    {
        return operator[](0);
    }

    T& first_or_default()
    {
        return length_ > 0 ? operator[](0) : T();
    }

    const T& first_or_default() const
    {
        return length_ > 0 ? operator[](0) : T();
    }
    
    T& last()
    {
        return operator[](length_ - 1);
    }

    const T& last() const
    {
        return operator[](length_ - 1);
    }

    T& last_or_default()
    {
        return length_ > 0 ? operator[](length_ - 1) : T();
    }

    const T& last_or_default() const
    {
        return length_ > 0 ? operator[](length_ - 1) : T();
    }

    bool all(bool(* predicate)(const T&)) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool any(bool(* predicate)(const T&)) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return true;
        }

        return  false;
    }
    
    bool none(bool(* predicate)(const T&)) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool count(bool(* predicate)(const T&)) const
    {
        uint counter = 0;
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i]))
            {
                counter++;
            }
        }

        return  counter;
    }

    List<T> where(std::function<bool(const T& item)> predicate) const
    {
        List<T> result;
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i]))
            {
                result.Add(inner_[i]);
            }
        }
        
        return result;
    }

    template<typename ResultType>
    List<ResultType> select(std::function<ResultType(const T& item)> fetcher)
    {
        List<ResultType> result(length_);

        for (uint i = 0; i < length_; i++)
        {
            result[i] = fetcher(operator[](i));
        }

        return result;
    }

    void Insert(const T& item, uint indexAt)
    {
        if (indexAt > length_)
        {
            throw new std::out_of_range("Parameter \"indexAt\" is greater than last available index");
        }

        T item_copy = item;
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        ++length_;
        
        if (indexAt < length_)
        {
            for (uint i = 0; i < length_ - indexAt - 1; i++)
            {
                inner_[length_ - i - 1] = std::move(inner_[length_ - i - 2]);
            }
        }

        inner_[indexAt] = std::move(item_copy);
    }

    void Clear()
    {
        reallocate(0);
        length_ = 0;
    }

    void sort()
    {
        if (inner_ == nullptr) return;
        
        for (uint i = 0; i < length_ - 1; i++)
        {
            for (uint j = i + 1; j < length_; j++)
            {
                if (inner_[i] < inner_[j])
                {
                    std::swap(inner_[i], inner_[j]);
                }
            }
        }
    }

    void sort_reverse()
    {
        if (inner_ == nullptr) return;
        
        for (uint i = 0; i < length_ - 1; i++)
        {
            for (uint j = i + 1; j < length_; j++)
            {
                if (inner_[i] > inner_[j])
                {
                    std::swap(inner_[i], inner_[j]);
                }
            }
        }
    }

    template<class Predicate>
    void sort()
    {
        if (inner_ == nullptr) return;
        
        for (uint i = 0; i < length_ - 1; i++)
        {
            for (uint j = i + 1; j < length_; j++)
            {
                if (Predicate(inner_[i], inner_[j]))
                {
                    std::swap(inner_[i], inner_[j]);
                }
            }
        }
    }

    void sort(const std::function<bool(T a, T b)> predicate)
    {
        if (inner_ == nullptr) return;
        
        for (uint i = 0; i < length_ - 1; i++)
        {
            for (uint j = i + 1; j < length_; j++)
            {
                if (predicate(inner_[i], inner_[j]))
                {
                    std::swap(inner_[i], inner_[j]);
                }
            }
        }
    }

    List operator+(const T& rhs)
    {
        List result = *this;
        result.Add(rhs);

        return result;
    }

    List& operator+=(const T& rhs)
    {
        Add(rhs);

        return *this;
    }

    List operator+(const List& rhs)
    {
        List result = *this;
        result.AddMany(rhs);

        return result;
    }

    List& operator+=(const List& rhs)
    {
        AddMany(rhs);

        return *this;
    }

    List operator*(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (rhs.Contains(entry))
            {
                result.Add(entry);
            }
        }

        return result;
    }

    List& operator*=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!rhs.Contains(inner_[i]))
            {
                RemoveAt(i--);
            }
        }

        return *this;
    }

    List operator-(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (!rhs.Contains(entry))
            {
                result.Add(entry);
            }
        }

        return result;
    }

    List& operator-=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (rhs.Contains(inner_[i]))
            {
                RemoveAt(i--);
            }
        }

        return *this;
    }

private:
    using Array<T>::get_allocate_size;
    using Array<T>::reallocate;
    using Array<T>::slack;

    using Array<T>::inner_;
    using Array<T>::length_;
    using Array<T>::allocated_length_;
};
