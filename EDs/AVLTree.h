#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

template <typename T>
struct Node
{
    T key;
    Node<T> *left;
    Node<T> *right;
    int height;
    int frequence;
    Node(T k, unsigned int v = 1) : key(k), left(nullptr), right(nullptr), height(1), frequence(v) {}
};

template <typename T, typename COMPARATOR = comparator<T>>
class AVLTree
{
private:
    Node<T> *root = nullptr;
    COMPARATOR compare;
    unsigned int comps = 0;
    unsigned int _size = 0;

    int height(Node<T> *node)
    {
        return (node == nullptr) ? 0 : node->height;
    }

    int balance(Node<T> *node)
    {
        return (node == nullptr) ? 0 : height(node->right) - height(node->left);
    }

    int max(int a, int b)
    {
        return (a > b) ? a : b;
    }

    Node<T> *rightRotate(Node<T> *x)
    {
        Node<T> *y = x->left;
        Node<T> *z = y->right;
        x->left = z;
        y->right = x;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Node<T> *leftRotate(Node<T> *x)
    {
        Node<T> *y = x->right;
        Node<T> *z = y->left;
        x->right = z;
        y->left = x;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    Node<T> *fixupInsert(Node<T> *node)
    {
        node->height = max(height(node->left), height(node->right)) + 1;
        int bal = balance(node);

        if (bal > 1 && balance(node->right) >= 0)
        {
            return leftRotate(node);
        }
        else if (bal > 1 && balance(node->right) < 0)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        else if (bal < -1 && balance(node->left) <= 0)
        {
            return rightRotate(node);
        }
        else if (bal < -1 && balance(node->left) > 0)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        return node;
    }

    Node<T> *fixupDelete(Node<T> *node)
    {
        node->height = max(height(node->left), height(node->right)) + 1;
        int bal = balance(node);

        if (bal > 1 && balance(node->right) >= 0)
        {
            return leftRotate(node);
        }
        else if (bal > 1 && balance(node->right) < 0)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        else if (bal < -1 && balance(node->left) <= 0)
        {
            return rightRotate(node);
        }
        else if (bal < -1 && balance(node->left) > 0)
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        return node;
    }

    Node<T> *delete_successor(Node<T> *node, Node<T> *successor)
    {
        if (successor->left != nullptr)
            successor->left = delete_successor(node, successor->left);
        else
        {
            node->key = successor->key;
            node->frequence = successor->frequence;
            Node<T> *aux = successor->right;
            delete successor;
            return aux;
        }
        successor = fixupDelete(successor);
        return successor;
    }

    Node<T> *_delete(Node<T> *node, T key, unsigned int value = 1)
    {
        if (node == nullptr)
            return node;
        comps++;
        if (key < node->key)
        {
            node->left = _delete(node->left, key, value);
        }
        else if (key > node->key)
        {
            comps++;
            node->right = _delete(node->right, key, value);
        }
        else if (node->frequence > value)
        {
            node->frequence -= value;
            return node;
        }
        else if (node->right == nullptr)
        {
            Node<T> *child = node->left;
            delete node;
            return child;
        }
        else
            node->right = delete_successor(node, node->right);
        node = fixupDelete(node);
        return node;
    }

    Node<T> *_insert(Node<T> *node, T key, unsigned int value = 1)
    {
        if (node == nullptr)
        {
            _size++;
            return new Node<T>(key, value);
        }
        comps++;
        if (compare(key, node->key))
        {
            node->left = _insert(node->left, key, value);
        }
        else if (compare(node->key, key))
        {
            comps++;
            node->right = _insert(node->right, key, value);
        }
        else
        {
            comps++;
            node->frequence += value;
            return node;
        }

        node = fixupInsert(node);

        return node;
    }

    Node<T> *_update(Node<T> *node, T key, unsigned int value)
    {
        if (node == nullptr)
            return node;
        comps++;
        if (compare(key, node->key))
        {
            node->left = _update(node->left, key, value);
        }
        else if (compare(node->key, key))
        {
            comps++;
            node->right = _update(node->right, key, value);
        }
        else
        {
            comps++;
            node->frequence = value;
        }
        return node;
    }

    void _print(Node<T> *node) const
    {
        if (node == nullptr)
            return;

        _print(node->left);
        if constexpr (std::is_same<T, icu::UnicodeString>::value)
        {
            std::string skey;
            node->key.toUTF8String(skey);
            std::cout << skey << " (" << node->frequence << ")" << std::endl;
        }
        else
            std::cout << node->key << " (" << node->frequence << ")" << std::endl;
        _print(node->right);
    }

    // Imprime a árvore na tela
    void bshow(Node<T> *node, std::string heranca) const
    {
        if (node != nullptr && (node->left != nullptr || node->right != nullptr))
        {
            bshow(node->right, heranca + "r");
        }
        for (int i = 0; i < (int)heranca.size() - 1; i++)
        {
            std::cout << (heranca[i] != heranca[i + 1] ? "│   " : "    ");
        }
        if (heranca != "")
        {
            std::cout << (heranca.back() == 'r' ? "┌───" : "└───");
        }
        if (node == nullptr)
        {
            std::cout << "#" << std::endl;
            return;
        }
        if constexpr (std::is_same<T, icu::UnicodeString>::value)
        {
            std::string skey;
            node->key.toUTF8String(skey);
            std::cout << skey << " (" << node->frequence << ")" << std::endl;
        }
        else
            std::cout << node->key << " (" << node->frequence << ")" << std::endl;
        if (node != nullptr && (node->left != nullptr || node->right != nullptr))
        {
            bshow(node->left, heranca + "l");
        }
    }

public:
    AVLTree(COMPARATOR comp = COMPARATOR()) : compare(comp)
    {
        root = nullptr;
        _size = 0;
    }

    void insert(T key, unsigned int value = 1)
    {
        root = _insert(root, key, value);
    }

    void remove(T key, unsigned int value = 1)
    {
        root = _delete(root, key, value);
    }

    void update(T key, unsigned int value)
    {
        root = _update(root, key, value);
    }

    T find(T key)
    {
        Node<T> *node = root;
        while (node != nullptr)
        {
            comps++;
            if (compare(key, node->key))
            {
                node = node->left;
            }
            else if (compare(node->key, key))
            {
                comps++;
                node = node->right;
            }
            else
            {
                comps++;
                return node->key;
            }
        }
        return T();
    }

    void print() const
    {
        // bshow(root, "");
        _print(root);
    }

    size_t comparisons()
    {
        return comps;
    }

    size_t size() const
    {
        return _size;
    }
};

#endif
