#pragma once
#include <stdexcept>
#include <cstddef>
#include <utility>
#include "node.hpp"

namespace eda {

template <typename T>
class LinkedList {
    Node<T>* head{nullptr};
    Node<T>* tail{nullptr};
    std::size_t sz{0};

public:
    LinkedList() = default;
    ~LinkedList(){ clear(); }

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    // move semantics para devolver por valor sin copias
    LinkedList(LinkedList&& other) noexcept
        : head(other.head), tail(other.tail), sz(other.sz) {
        other.head = other.tail = nullptr; other.sz = 0;
    }
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head = other.head; tail = other.tail; sz = other.sz;
            other.head = other.tail = nullptr; other.sz = 0;
        }
        return *this;
    }

    struct iterator {
        Node<T>* p;
        explicit iterator(Node<T>* q):p(q){}
        T& operator*() const { return p->data; }
        iterator& operator++(){ p = p->next; return *this; }
        bool operator!=(const iterator& o) const { return p!=o.p; }
    };

    iterator begin() const { return iterator(head); }
    iterator end() const { return iterator(nullptr); }

    bool empty() const { return sz==0; }
    std::size_t size() const { return sz; }

    void push_back(const T& v){
        auto* n = new Node<T>(v);
        if (!head){ head = tail = n; }
        else { tail->next = n; tail = n; }
        ++sz;
    }
    void push_front(const T& v){
        auto* n = new Node<T>(v);
        n->next = head; head = n; if(!tail) tail=n; ++sz;
    }
    T& front(){
        if(!head) throw std::runtime_error("Lista vacía");
        return head->data;
    }
    const T& front() const {
        if(!head) throw std::runtime_error("Lista vacía");
        return head->data;
    }
    void pop_front(){
        if(!head) throw std::runtime_error("Lista vacía");
        auto* n = head; head = head->next; delete n; --sz; if(!head) tail=nullptr;
    }

    void clear(){ while(!empty()) pop_front(); }
};

} // namespace eda
