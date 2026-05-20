#pragma once
#include <stdexcept>

template<class T>
class LinkedList {
private:
    struct Node {
        T value;
        Node* next;
        explicit Node(const T& nodeValue) : value(nodeValue), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int size;

    Node* GetNode(int index) const {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");

        Node* current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current;
    }

public:
    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    LinkedList(const T* items, int count) : head(nullptr), tail(nullptr), size(0) {
        if (count < 0) throw std::invalid_argument("count < 0");
        if (count > 0 && items == nullptr) throw std::invalid_argument("items is null");

        for (int i = 0; i < count; ++i) {
            Append(items[i]);
        }
    }

    LinkedList(const LinkedList<T>& other) : head(nullptr), tail(nullptr), size(0) {
        Node* current = other.head;
        while (current != nullptr) {
            Append(current->value);
            current = current->next;
        }
    }

    LinkedList<T>& operator=(const LinkedList<T>& other) {
        if (this == &other) return *this;

        Clear();
        Node* current = other.head;
        while (current != nullptr) {
            Append(current->value);
            current = current->next;
        }
        return *this;
    }

    ~LinkedList() { Clear(); }

    void Clear() {
        Node* current = head;
        while (current != nullptr) {
            Node* nextNode = current->next;
            delete current;
            current = nextNode;
        }

        head = nullptr;
        tail = nullptr;
        size = 0;
    }

    int GetSize() const { return size; }

    const T& GetFirst() const {
        if (head == nullptr) throw std::out_of_range("IndexOutOfRange");
        return head->value;
    }

    const T& GetLast() const {
        if (tail == nullptr) throw std::out_of_range("IndexOutOfRange");
        return tail->value;
    }

    const T& Get(int index) const { return GetNode(index)->value; }
    T& Get(int index) { return GetNode(index)->value; }

    void Append(const T& value) {
        Node* newNode = new Node(value);

        if (head == nullptr) {
            head = newNode;
            tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        ++size;
    }

    void Prepend(const T& value) {
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;

        if (tail == nullptr) {
            tail = newNode;
        }
        ++size;
    }

    void InsertAt(int index, const T& value) {
        if (index < 0 || index > size) throw std::out_of_range("IndexOutOfRange");
        if (index == 0) {
            Prepend(value);
            return;
        }
        if (index == size) {
            Append(value);
            return;
        }

        Node* previous = GetNode(index - 1);
        Node* newNode = new Node(value);
        newNode->next = previous->next;
        previous->next = newNode;
        ++size;
    }

    void Set(int index, const T& value) {
        GetNode(index)->value = value;
    }

    void RemoveAt(int index) {
        if (index < 0 || index >= size) throw std::out_of_range("IndexOutOfRange");

        if (index == 0) {
            Node* oldHead = head;
            head = head->next;
            delete oldHead;
            --size;
            if (size == 0) tail = nullptr;
            return;
        }

        Node* previous = GetNode(index - 1);
        Node* current = previous->next;
        previous->next = current->next;
        if (current == tail) {
            tail = previous;
        }
        delete current;
        --size;
    }
};
