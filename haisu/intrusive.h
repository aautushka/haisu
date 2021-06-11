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

namespace haisu
{

// just a regular doubly linked list
// does not manage memory
// relies on calling code for memory managment 
// the interface is inspided by std::list
template <typename T>
class intrusive_list
{
public:
    using self_type = intrusive_list<T>;

    intrusive_list()
        : head(nullptr)
        , tail(nullptr)
    {
    }

    intrusive_list(const self_type&) = delete;
    self_type& operator =(const self_type&) = delete;


    intrusive_list(self_type&& other)
    {
        *this = std::move(other);
    }

    self_type& operator =(self_type&& other)
    {
        head = other.head;
        tail = other.tail;
        other.head = other.tail = nullptr;
        return *this;
    }    

    struct node
    {
        node() : prev(nullptr), next(nullptr){}
        node(T t) : data(std::move(t)){}
        T data;
        node* prev;
        node* next;

        operator T() const
        {
            return data;
        }
    };

    bool empty() const
    {
        return head == nullptr;
    }

    size_t size() const
    {
        size_t res = 0;
        node* cur = head;
        while (cur != tail)
        {
            res += 1;
            cur = cur->next;    
        }    
        
        res += (head == nullptr ? 0 : 1);
        return res;
    }

    void push_back(node& n)
    {
        if (empty())
        {
            init(n);
        }
        else
        {
            tail->next = &n;
            n.next = nullptr;
            n.prev = tail;
            tail = &n;
        }
    }

    void push_front(node& n)
    {
        if (empty())
        {
            init(n);
        }
        else
        {
            head->prev = &n;
            n.prev = nullptr;
            n.next = head;
            head = &n;
        }
    }    

    void push_back(self_type&& ll)
    {
        if (!head)
        {
            *this = std::move(ll);
        }
        else if (ll.head)
        {
            tail->next = ll.head;
            ll.head->prev = tail;
            tail = ll.tail;

            ll.head = ll.tail = nullptr;
        }
    }

    node& pop_back()
    {
        assert(tail != nullptr);
        node* res = tail;

        if (tail->prev)
        {
            tail->prev->next = nullptr;
            tail = tail->prev;
        }
        else
        {
            clear();
        }
        res->prev = nullptr;
        return *res;
    }

    node& pop_front()
    {
        assert(head != nullptr);
        node* res = head;
        
        if (head->next)
        {
            head->next->prev = nullptr;
            head = head->next;
        }
        else
        {
            clear();
        }
        res->next = nullptr;
        return *res;
    }

    void erase(node& n)
    {
        if (n.prev && n.next)
        {
            n.prev->next = n.next;
            n.next->prev = n.prev;
        }
        else if (n.prev)
        {
            tail = n.prev;
            tail->next = nullptr;
        }
        else if (n.next)
        {
            head = n.next;
            head->prev = nullptr;
        }
        else
        {
            clear();
        }

        n.next = n.prev = nullptr;
    }

    void clear()
    {
        tail = head = nullptr;
    }

    const node& front() const
    {
        assert(head != nullptr);
        return *head;
    }


    const node& back() const
    {
        assert(tail != nullptr);
        return *tail;    
    }    


    node& front() 
    {
        assert(head != nullptr);
        return *head;
    }

    node& back()
    {
        assert(tail != nullptr);
        return *tail;
    }

    class iterator
    {
    public:
        iterator() : _cur(nullptr) { }
        explicit iterator(intrusive_list<T>& l) : _cur(l.head) { }
        bool operator ==(const iterator& other) const {return _cur == other._cur; }
        bool operator !=(const iterator& other) const { return !(*this == other); }
        T& operator *() {return _cur->data; }
        const T& operator *() const {return _cur->data;}
        iterator& operator ++() { _cur = _cur->next; return *this; }
        iterator operator ++(int) { iterator out(*this); ++(*this); return out;}
        T* operator ->() { return &**this; }
        const T* operator ->() const {return &**this;}

    private:
        typename intrusive_list<T>::node* _cur;
    };

    class const_iterator
    {
    public:
        const_iterator() : _cur(nullptr) { }
        explicit const_iterator(const intrusive_list<T>& l) : _cur(l.head) { }
        bool operator ==(const const_iterator& other) const {return _cur == other._cur; }
        bool operator !=(const const_iterator& other) const { return !(*this == other); }
        const T& operator *() {return _cur->data; }
        const T& operator *() const {return _cur->data;}
        const_iterator& operator ++() { _cur = _cur->next; return *this; }
        const_iterator operator ++(int) { const_iterator out(*this); ++(*this); return out;}
        const T* operator ->() const {return &**this;}

    private:
        const typename intrusive_list<T>::node* _cur;
    };

    iterator begin() { return iterator(*this); }
    iterator end() { return iterator(); }
    const_iterator begin() const { return const_iterator(*this); }
    const_iterator end() const { return const_iterator(); }

    
private:
    void init(node& n)
    {
        head = &n;
        tail = &n;
        head->next = nullptr;
        head->prev = nullptr;
    }

    node* head;
    node* tail;
};

// a wrapper acound intrusive_list
// adds memory managments, allocs list nodes on heap
template <typename T>
class list
{
public:
    list()
    {
    }

    list(std::initializer_list<T> ll)
    {
        for (auto l : ll) push_back(l);
    }

    ~list()
    {
        clear();
    }

    list(const list<T>&) = delete;
    list<T>& operator =(const list<T>&) = delete;

    list(list<T>&& other)
        : _list(std::move(other._list))
    {
    }

    list<T>& operator =(list<T>&& other) 
    {
        clear();
        _list = std::move(other._list);
        return *this;
    }

    void push_back(T t)
    {
        node_t* node = new node_t(std::move(t));
        _list.push_back(*node);
    }

    void push_front(T t)
    {
        node_t* node = new node_t(std::move(t));
        _list.push_front(*node);
    }

    T& emplace_back()
    {
        node_t* node = new node_t;
        _list.push_back(*node);
        return node->data;
    }

    T& emplace_front()
    {
        node_t* node = new node_t;
        _list.push_front(*node);
        return node->data;
    }

    void push_back(list<T>&& tt)
    {
        _list.push_back(std::move(tt._list));
    }

    bool empty() const
    {
        return _list.empty();
    }

    size_t size() const
    {
        return _list.size();
    }

    void clear()
    { 
        while (!empty())
        {
            erase_back();
        }
    }

    T pop_front()
    {
        node_t& node = _list.pop_front();
        T t = std::move(node.data);
        delete &node;
        return std::move(t);
    }

    T pop_back()
    {
        node_t& node = _list.pop_back();
        T t = std::move(node.data);
        delete &node;
        return std::move(t);
    }

    void erase_back()
    {
        node_t& node = _list.pop_back();
        delete &node;
    }

    void erase_front()
    {
        node_t& node = _list.pop_front();
        delete &node;
    }

    T& back() { return _list.back().data; }
    T& front() {return _list.front().data; }
    const T& back() const {return _list.back().data; }
    const T& front() const {return _list.front().data; }


    using iterator = typename intrusive_list<T>::iterator;
    using const_iterator = typename intrusive_list<T>::const_iterator;

    iterator begin()
    {
        return _list.begin();
    }

    iterator end()
    {
        return _list.end();
    }

    const_iterator begin() const
    {
        return _list.begin();
    }

    const_iterator end() const
    {
        return _list.end();
    }

private:
    using list_t = intrusive_list<T>;
    using node_t = typename list_t::node;
    list_t _list;
};

} // namespace haisu
