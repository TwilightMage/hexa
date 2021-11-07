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
    using ItemType = std::conditional_t<(sizeof(T) > 8), const T&, T>;
    
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
        length_ = length;
        if (length > 0)
        {
            inner_ = new T[allocated_length_];
            if constexpr (std::is_convertible<T, std::size_t>::value)
            {
                memcpy(inner_, inner, sizeof(T) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = inner[i];
                }
            }
        }
    }

    List(const T* inner, uint length)
    {
        allocated_length_ = get_allocate_size(length);
        this->length_ = length;
        if (length > 0)
        {
            inner_ = new T[allocated_length_];
            if constexpr (std::is_convertible<T, std::size_t>::value)
            {
                memcpy(inner_, inner, sizeof(T) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = inner[i];
                }
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
        length_ = length;
        if (length > 0)
        {
            inner_ = new T[allocated_length_];
            if constexpr (std::is_convertible<T, std::size_t>::value)
            {
                ZeroMemory(inner_, sizeof(T) * length_);
            }
            else
            {
                for (uint i = 0; i < length; i++)
                {
                    inner_[i] = T();
                }
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

    static List generate(uint size, ItemType placeholder)
    {
        auto result = List(size);
        for (uint i = 0; i < size; i++)
        {
            result[i] = placeholder;
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
            if constexpr (std::is_convertible<T, std::size_t>::value)
            {
                memcpy(inner_, rhs.inner_, sizeof(T) * length_);
            }
            else
            {
                for (uint i = 0; i < length_; i++)
                {
                    inner_[i] = std::move(rhs.inner_[i]);
                }
            }
        }
        else
        {
            inner_ = nullptr;
        }

        return *this;
    }

    void add(ItemType item)
    {
        if (length_ == allocated_length_)
        {
            reallocate(get_allocate_size(length_ + 1));
        }

        inner_[length_++] = std::move(item);
    }

    template<typename... Items>
    void add_many(ItemType first, const Items&... rest)
    {
        add(first);

        if constexpr (sizeof...(rest) > 0)
        {
            add_many(rest...);
        }
    }

    void add_many(const List<T>& items)
    {
        if (length_ + items.length() > allocated_length_)
        {
            reallocate(get_allocate_size(length_ + items.length()));
        }

        if constexpr (std::is_convertible<T, std::size_t>::value)
        {
            if (items.length() > 0)
            {
                memcpy(inner_ + length_, items.inner_, sizeof(T) * items.length_);
                length_ += items.length_;
            }
        }
        else
        {
            for (const auto& item : items)
            {
                inner_[length_++] = std::move(item);
            }
        }
    }

    void add_unique(ItemType item)
    {
        if (length_ > 0)
        {
            for (uint i = 0; i < length_; i++)
            {
                if (inner_[i] == item) return;;
            }
        }

        add(item);
    }

    bool contains(ItemType item) const
    {
        if (length_ == 0) return false;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return true;
        }
        return false;
    }

    int index_of(ItemType item) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == item) return i;
        }
        return -1;
    }

    int index_of(std::function<bool(ItemType item)> predicate) const
    {
        if (length_ == 0) return -1;
        
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return i;
        }
        return -1;
    }

    void remove_at(uint index)
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

    void remove_range(uint start, uint size)
    {
        if (start + size >= length_)
        {
            throw new std::out_of_range("last remove index is greater than last item index");
        }

        const uint new_allocated_length = get_allocate_size(length_ - size);
        if (new_allocated_length != allocated_length_)
        {
            allocated_length_ = new_allocated_length;
            length_ -= size;
            T* new_inner;

            if (allocated_length_ > 0)
            {
                new_inner = new T[allocated_length_];

                for (uint i = 0; i < start; i++)
                {
                    new_inner[i] = std::move(inner_[i]);
                }

                for (uint i = start; i < length_; i++)
                {
                    new_inner[i] = std::move(inner_[i + size]);
                }
            }
            else
            {
                new_inner = nullptr;
            }

            delete inner_;
            inner_ = new_inner;
        }
        else
        {
            for (uint i = start + size; i < length_ - size; i++)
            {
                inner_[i - size] = std::move(inner_[i]);
                inner_[i] = T();
            }
        }
    }

    void replace(ItemType from, ItemType to)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (inner_[i] == from)
            {
                inner_[i] = to;
            }
        }
    }

    void force_size_fit(uint new_size, ItemType placeholder = T())
    {
        if (length_ < new_size)
        {
            if (new_size >= allocated_length_)
            {
                reallocate(get_allocate_size(new_size));
            }

            for (; length_ < new_size; ++length_)
            {
                inner_[length_] = placeholder;
            }
        }
        else if (length_ > new_size)
        {
            const auto new_allocated_length = get_allocate_size(new_size);
            if (new_allocated_length < allocated_length_)
            {
                reallocate(new_allocated_length);
            }

            for (length_ = allocated_length_; length_ > new_size; --length_)
            {
                inner_[length_] = T();
            }
        }
    }

    void remove(ItemType item)
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

    void remove(bool(* predicate)(ItemType))
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

    FORCEINLINE T& operator[](uint index)
    {
        return at(index);
    }

    FORCEINLINE ItemType operator[](uint index) const
    {
        return at(index);
    }

    FORCEINLINE T& at(uint index)
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    FORCEINLINE ItemType at(uint index) const
    {
        if (index >= length_)
        {
            throw new std::out_of_range("Parameter \"index\" is greater than last item index");
        }

        return inner_[index];
    }

    FORCEINLINE T& first()
    {
        return operator[](0);
    }

    FORCEINLINE ItemType first() const
    {
        return operator[](0);
    }

    FORCEINLINE T& first_or_default()
    {
        return length_ > 0 ? operator[](0) : T();
    }

    FORCEINLINE ItemType first_or_default() const
    {
        return length_ > 0 ? operator[](0) : T();
    }
    
    FORCEINLINE T& last()
    {
        return operator[](length_ - 1);
    }

    FORCEINLINE ItemType last() const
    {
        return operator[](length_ - 1);
    }

    FORCEINLINE T& last_or_default()
    {
        return length_ > 0 ? operator[](length_ - 1) : T();
    }

    FORCEINLINE ItemType last_or_default() const
    {
        return length_ > 0 ? operator[](length_ - 1) : T();
    }

    bool all(std::function<bool(ItemType item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool any(std::function<bool(ItemType item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return true;
        }

        return  false;
    }
    
    bool none(std::function<bool(ItemType item)> predicate) const
    {
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i])) return false;
        }

        return  true;
    }

    bool count(std::function<bool(ItemType item)> predicate) const
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

    List<T> where(std::function<bool(ItemType item)> predicate) const
    {
        List<T> result;
        for (uint i = 0; i < length_; i++)
        {
            if (predicate(inner_[i]))
            {
                result.add(inner_[i]);
            }
        }
        
        return result;
    }

    template<typename ResultType>
    List<ResultType> select(std::function<ResultType(ItemType item)> fetcher)
    {
        List<ResultType> result(length_);

        for (uint i = 0; i < length_; i++)
        {
            result[i] = fetcher(operator[](i));
        }

        return result;
    }

    void insert(ItemType item, uint indexAt)
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

    void clear()
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

    List operator+(ItemType rhs)
    {
        List result = *this;
        result.add(rhs);

        return result;
    }

    List& operator+=(ItemType rhs)
    {
        add(rhs);

        return *this;
    }

    List operator+(const List& rhs)
    {
        List result = *this;
        result.add_many(rhs);

        return result;
    }

    List& operator+=(const List& rhs)
    {
        add_many(rhs);

        return *this;
    }

    List operator*(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (rhs.contains(entry))
            {
                result.add(entry);
            }
        }

        return result;
    }

    List& operator*=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (!rhs.contains(inner_[i]))
            {
                remove_at(i--);
            }
        }

        return *this;
    }

    List operator-(const List& rhs) const
    {
        List result;
        for (auto& entry : *this)
        {
            if (!rhs.contains(entry))
            {
                result.add(entry);
            }
        }

        return result;
    }

    List& operator-=(const List& rhs)
    {
        for (uint i = 0; i < length_; i++)
        {
            if (rhs.contains(inner_[i]))
            {
                remove_at(i--);
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
