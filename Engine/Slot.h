#pragma once

#include "BasicTypes.h"
#include "SlotBase.h"

template<class C>
class Slot : public SlotBase
{
public:
    Slot()
        : is_active_(true)
    {
    }

    Slot(const Slot& rhs)
        : item_(rhs.item_)
        , is_active_(rhs.is_active_)
    {
        if (is_active_) inc_ref(item_.get());
    }
    
    Slot(const Shared<C>& rhs)
        : item_(rhs)
        , is_active_(true)
    {
        if (is_active_) inc_ref(item_.get());
    }
    
    explicit Slot(bool state)
        : is_active_(state)
    {
    }
    
    Slot(const Shared<C>& item, bool state)
        : item_(item)
        , is_active_(state)
    {
    }
    
    ~Slot()
    {
        if (is_active_) dec_ref(item_.get());
    }

    void activate()
    {
        if (!is_active_)
        {
            is_active_ = true;
    
            inc_ref(item_.get());
        }
    }
    
    void deactivate()
    {
        if (is_active_)
        {
            is_active_ = false;
    
            dec_ref(item_.get());
        }
    }
    
    Slot& operator=(const Slot& rhs)
    {
        if (item_ != rhs.item_)
        {
            if (is_active_) dec_ref(item_.get());
            item_ = rhs.item_;
            is_active_ = rhs.is_active_;
            if (is_active_) inc_ref(item_.get());
        }
        else
        {
            if (is_active_ != rhs.is_active_)
            {
                is_active_ = rhs.is_active_;
                if (is_active_)
                {
                    activate();
                }
                else
                {
                    deactivate();
                }
            }
        }
        return *this;
    }
    
    const Shared<C>& operator->() const
    {
        return item_;
    }
    
    const Shared<C>& operator*() const
    {
        return item_;
    }

private:
    Shared<C> item_;
    bool is_active_;
};
