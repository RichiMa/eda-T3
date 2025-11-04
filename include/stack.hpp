#pragma once
#include "linked_list.hpp"

namespace eda {

template <typename T>
class Stack {
    LinkedList<T> list;
public:
    void push(const T& v){ list.push_front(v); }
    void pop(){ list.pop_front(); }
    T& top(){ return list.front(); }
    const T& top() const { return list.front(); }
    bool empty() const { return list.size()==0; }
    std::size_t size() const { return list.size(); }
};

} // namespace eda
