#pragma once
namespace eda {

template <typename T>
struct Node {
    T data;
    Node<T>* next;
    explicit Node(const T& d) : data(d), next(nullptr) {}
};

} // namespace eda
