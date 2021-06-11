/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#pragma once
#include <mutex>
#include <iostream>

namespace haisu
{

template <typename T>
class synchronized
{
public:
    class proxy
    {
    public:
        explicit proxy(synchronized& s)
            : base_(&s)
        {
            base_->lock();
        }

        proxy(const proxy&) = delete;
        proxy& operator =(const proxy&) = delete;
        proxy(proxy&& rhs)
        {
            base_ = rhs.base_;
            rhs.base_ = nullptr;
        }

        proxy& operator =(proxy&& rhs)
        {
            if (base_)
            {
                base_->unlock();
                base_ = nullptr;
            }

            base_ = rhs.base_;
            rhs.base_ = nullptr;
            return *this;
        }

        ~proxy()
        {
            if (base_)
            {
                base_->unlock();
            }
        }

        T* operator ->()
        {
            assert(base_ != nullptr);
            return &base_->t_;
        }

        const T* operator ->() const
        {
            return &base_->t_;
        }

    private:
        synchronized* base_ = nullptr;
    };

    proxy operator ->()
    {
        return proxy(*this);
    }

    const proxy operator->() const
    {
        return proxy(*this);
    }

    template <typename ...Args>
    synchronized(Args&&... args) : t_(std::forward<Args>(args)...)
    {
    }

    void lock()
    {
        mutex_.lock();
    }

    void unlock()
    {
        mutex_.unlock();
    }

private:

    T t_;
    std::recursive_mutex mutex_;
};

} // namespace haisu

