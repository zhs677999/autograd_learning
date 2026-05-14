#include <iostream>
#include <stdexcept>

template <typename T>
class LinkedList {
private:    struct Node {
        T* data;
        Node* next;
        Node(T* d) : data(d), next(nullptr) {}
    };
    Node* head;
    public:
    LinkedList() : head(nullptr) {}
    ~LinkedList() {
        Node* current = head;
        while (current) {
            Node* next = current->next;
            delete current->data;
            delete current;
            current = next;
        }
    }   
    void add(T* data) {
        Node* newNode = new Node(data);
        if (!head) {
            head = newNode;
        } else {
            Node* current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
    }
}
