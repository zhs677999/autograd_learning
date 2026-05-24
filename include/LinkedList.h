#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <cstddef>
#include <stdexcept>

template <typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;

        Node(T value) : data(value), next(nullptr) {}
    };

    Node* head;
    size_t count;

public:
    LinkedList() : head(nullptr), count(0) {}
    LinkedList(const LinkedList& other) : head(nullptr), count(0) {
        Node* current = other.head;
        while (current) {
            add(current->data);
            current = current->next;
        }
    }

    LinkedList& operator=(const LinkedList& other) {
        if (this == &other) {
            return *this;
        }
        clear();
        Node* current = other.head;
        while (current) {
            add(current->data);
            current = current->next;
        }
        return *this;
    }

    ~LinkedList() {
        clear();
    }

    void add(T value) {
        Node* newNode = new Node(value);
        count++;

        if (!head) {
            head = newNode;
            return;
        }

        Node* current = head;
        while (current->next) {
            current = current->next;
        }
        current->next = newNode;
    }

    T getParent(int index) const {
        if (index < 0 || index >= static_cast<int>(count)) {
            throw std::out_of_range("Parent index out of bounds");
        }

        Node* current = head;
        for (int i = 0; i < index; i++) {
            current = current->next;
        }
        return current->data;
    }

    size_t parentCount() const {
        return count;
    }

    void clear() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = nullptr;
        count = 0;
    }
};

#endif
