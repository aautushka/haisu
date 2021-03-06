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

#include <functional>
#include <type_traits>

namespace haisu
{

template <typename T>
struct is_function_pointer : std::integral_constant<bool, 
    std::is_pointer<T>{} && std::is_function<typename std::remove_pointer_t<T>>{}>
{
};

template <typename T>
struct casts_to_function_pointer : std::integral_constant<bool,
    std::is_function<T>{} || is_function_pointer<T>{}>
{
};

class track_storage_safety
{
public:
    void mark_safe() noexcept
    {
        safe_to_store_ = true;
    }

    void mark_unsafe() noexcept
    {
        safe_to_store_ = false;
    }

    bool safe_to_store() const noexcept
    {
        return safe_to_store_;
    }

    void swap(track_storage_safety& rhs) noexcept
    {
        std::swap(safe_to_store_, rhs.safe_to_store_);
    }

private:
    bool safe_to_store_{true};
};

class no_storage_safety_tracker
{
public:
    void mark_safe() noexcept {}
    void mark_unsafe() noexcept {}
    bool safe_to_store() const noexcept {return false;}
    void swap(no_storage_safety_tracker&) noexcept {}
};

// This is a generic function reference class inspired by LLVM's function_ref.
// There are some differences though:
// 1) haisu::function_ref stores function pointers directly, which allows object to be safely copied and stored
// 2) haisu::function_ref tries to convert a lambda (or any other function object for that matter) to a function pointer
// 3) more closely follows std::function interface (admittedly, not quite yet)

template <typename T>
class function_ref;

template <typename Ret, typename ... Arg>
class function_ref<Ret(Arg...)> final : private track_storage_safety
{
public:
    using track_storage_safety::safe_to_store;

    function_ref(const function_ref&) = default;
    function_ref& operator =(const function_ref&) = default;

    function_ref() noexcept
    {
        assign(nullptr);
    }

    function_ref(std::nullptr_t) noexcept
    {
        assign(nullptr);
    }

    template <typename Callable, typename = std::enable_if_t<!std::is_same<std::decay_t<Callable>, function_ref>{}>>
    function_ref(Callable&& callable) noexcept
    {
        assign(std::move(callable));
    }

    template <typename R, typename ... A>
    function_ref(R (*func)(A...)) noexcept
    {
        assign(func);
    }

    function_ref(Ret(*func)(Arg...)) noexcept
    {
        assign(func);
    }

    template <typename Callable, typename = std::enable_if_t<!std::is_same<std::decay_t<Callable>, function_ref>{}>>
    function_ref& operator =(Callable&& callable) noexcept
    {
        return assign(std::move(callable));
    }

    template <typename Callable, typename = 
        std::enable_if_t<
            !casts_to_function_pointer<Callable>{} && 
            !std::is_constructible<Ret(*)(Arg...), Callable>{} && 
            !std::is_same<std::decay_t<Callable>, function_ref>{}
        >
    >
    function_ref& assign(Callable&& callable) noexcept
    {
        stored_func_ = std::addressof(callable);
        caller_ = call_stored_object<std::decay_t<Callable>>;
        mark_unsafe();
        return *this;
    }

    template <typename R, typename ... A>
    function_ref& assign(R (*func)(A...)) noexcept
    {
        stored_func_ = reinterpret_cast<void*>(func);
        caller_ = call_stored_function<decltype(func)>;
        mark_safe();
        return *this;
    }

    function_ref& assign(Ret(*func)(Arg...)) noexcept
    {
        stored_func_ = (void*)func;
        caller_ = call_stored_function<decltype(func)>;
        mark_safe();
        return *this;
    }

    function_ref& assign(std::nullptr_t) noexcept
    {
        mark_safe();
        caller_ = [](void*, Arg...) -> Ret {throw std::bad_function_call();};
        stored_func_ = nullptr;
        return *this;
    }


    template <typename ... T>
    Ret operator ()(T&& ... args) const
    {
        return caller_(stored_func_, std::forward<T>(args)...);
    }

    operator bool() const noexcept
    {
        return stored_func_ != nullptr;
    }

    void swap(function_ref& rhs) noexcept
    {
        std::swap(stored_func_, rhs.stored_func_);
        std::swap(caller_, rhs.caller_);

        this->track_storage_safety::swap(rhs);
    }

private:
    template <typename Callable>
    static Ret call_stored_object(void* obj, Arg ... arg)
    {
        return (*reinterpret_cast<Callable*>(obj))(std::forward<Arg>(arg)...);
    }

    template <typename Func>
    static Ret call_stored_function(void* func, Arg ... arg)
    {
        return (reinterpret_cast<Func>(func))(std::forward<Arg>(arg)...);
    }

    void* stored_func_{nullptr};
    Ret (*caller_)(void* f, Arg...);
};

template <typename Ret, typename ... Args>
bool operator ==(const function_ref<Ret(Args...)>& f, std::nullptr_t) noexcept
{
    return !f;
}

template <typename Ret, typename ... Args>
bool operator ==(std::nullptr_t, const function_ref<Ret(Args...)>& f) noexcept
{
    return !f;
}

template <typename Ret, typename ... Args>
bool operator !=(const function_ref<Ret(Args...)>& f, std::nullptr_t) noexcept
{
    return f;
}

template <typename Ret, typename ... Args>
bool operator !=(std::nullptr_t, const function_ref<Ret(Args...)>& f) noexcept
{
    return f;
}

} // namespace haisu

namespace std
{
template <typename Ret, typename ... Args>
void swap(haisu::function_ref<Ret(Args...)>& lhs, haisu::function_ref<Ret(Args...)>& rhs)
{
    lhs.swap(rhs);
}
} // namespace std


