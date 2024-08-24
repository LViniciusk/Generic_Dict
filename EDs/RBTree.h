#ifndef RBTREE_H
#define RBTREE_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

// Definição das cores dos nós
enum Color
{
    RED,
    BLACK
};

template <typename T>
struct RBNode
{
    T key;
    RBNode<T> *left;
    RBNode<T> *right;
    RBNode<T> *parent;
    Color color;
    int frequence;
    RBNode(T k, unsigned int v = 1) : key(k), left(nullptr), right(nullptr), parent(nullptr), color(RED), frequence(v) {}
};

template <typename T, typename COMPARATOR = comparator<T>>
class RBTree
{
private:
    RBNode<T> *root = nullptr;
    COMPARATOR compare;
    unsigned int comps = 0;
    unsigned int _size = 0;

    void leftRotate(RBNode<T> *x)
    {
        RBNode<T> *y = x->right;
        x->right = y->left;
        if (y->left != nullptr)
        {
            y->left->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr)
        {
            root = y;
        }
        else if (x == x->parent->left)
        {
            x->parent->left = y;
        }
        else
        {
            x->parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(RBNode<T> *x)
    {
        RBNode<T> *y = x->left;
        x->left = y->right;
        if (y->right != nullptr)
        {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == nullptr)
        {
            root = y;
        }
        else if (x == x->parent->right)
        {
            x->parent->right = y;
        }
        else
        {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    void fixupInsert(RBNode<T> *z)
    {
        while (z->parent != nullptr && z->parent->color == RED)
        {
            if (z->parent == z->parent->parent->left)
            {
                RBNode<T> *y = z->parent->parent->right;
                if (y != nullptr && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->right)
                    {
                        z = z->parent;
                        leftRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            }
            else
            {
                RBNode<T> *y = z->parent->parent->left;
                if (y != nullptr && y->color == RED)
                {
                    z->parent->color = BLACK;
                    y->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                }
                else
                {
                    if (z == z->parent->left)
                    {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void fixupDelete(RBNode<T> *x)
    {
        while (x != root && x->color == BLACK)
        {
            if (x == x->parent->left)
            {
                RBNode<T> *w = x->parent->right;
                if (w->color == RED)
                {
                    w->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }
                if ((w->left == nullptr || w->left->color == BLACK) &&
                    (w->right == nullptr || w->right->color == BLACK))
                {
                    w->color = RED;
                    x = x->parent;
                }
                else
                {
                    if (w->right == nullptr || w->right->color == BLACK)
                    {
                        if (w->left != nullptr)
                            w->left->color = BLACK;
                        w->color = RED;
                        rightRotate(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->right != nullptr)
                        w->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root;
                }
            }
            else
            {
                RBNode<T> *w = x->parent->left;
                if (w->color == RED)
                {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }
                if ((w->left == nullptr || w->left->color == BLACK) &&
                    (w->right == nullptr || w->right->color == BLACK))
                {
                    w->color = RED;
                    x = x->parent;
                }
                else
                {
                    if (w->left == nullptr || w->left->color == BLACK)
                    {
                        if (w->right != nullptr)
                            w->right->color = BLACK;
                        w->color = RED;
                        leftRotate(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    if (w->left != nullptr)
                        w->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

    RBNode<T> *_delete(RBNode<T> *node, T key, unsigned int value = 1)
    {
        RBNode<T> *z = root;
        RBNode<T> *y = nullptr;
        RBNode<T> *x = nullptr;

        while (z != nullptr)
        {
            if (key == z->key)
            {
                break;
            }
            else if (compare(key, z->key))
            {
                z = z->left;
            }
            else
            {
                z = z->right;
            }
        }

        if (z == nullptr)
            return root;

        if (z->left == nullptr || z->right == nullptr)
        {
            y = z;
        }
        else
        {
            y = z->right;
            while (y->left != nullptr)
                y = y->left;
        }

        if (y->left != nullptr)
            x = y->left;
        else
            x = y->right;

        if (x != nullptr)
            x->parent = y->parent;

        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;

        if (y != z)
        {
            z->key = y->key;
            z->frequence = y->frequence;
        }

        if (y->color == BLACK && x != nullptr)
            fixupDelete(x);

        delete y;
        _size--;
        return root;
    }

    RBNode<T> *_insert(RBNode<T> *node, T key, unsigned int value = 1)
    {
        if (node == nullptr)
        {
            _size++;
            return new RBNode<T>(key, value);
        }
        comps++;
        if (compare(key, node->key))
        {
            node->left = _insert(node->left, key, value);
            node->left->parent = node;
        }
        else if (compare(node->key, key))
        {
            comps++;
            node->right = _insert(node->right, key, value);
            node->right->parent = node;
        }
        else
        {
            comps++;
            node->frequence += value;
            return node;
        }

        return node;
    }

    void _print(RBNode<T> *node) const
    {
        if (node == nullptr)
            return;

        _print(node->left);
        if constexpr (std::is_same<T, icu::UnicodeString>::value)
        {
            std::string skey;
            node->key.toUTF8String(skey);
            std::cout << skey << " (" << node->frequence << ") [" << (node->color == RED ? "R" : "B") << "]" << std::endl;
        }
        else
        {
            std::cout << node->key << " (" << node->frequence << ") [" << (node->color == RED ? "R" : "B") << "]" << std::endl;
        }
        _print(node->right);
    }

public:
    RBTree(COMPARATOR comp = COMPARATOR()) : compare(comp)
    {
        root = nullptr;
        _size = 0;
    }

    void insert(T key, unsigned int value = 1)
    {
        RBNode<T> *newNode = _insert(root, key, value);
        root = newNode;
        fixupInsert(newNode);
    }

    void remove(T key, unsigned int value = 1)
    {
        root = _delete(root, key, value);
    }

    void update(T key, unsigned int value)
    {
        RBNode<T> *node = root;
        while (node != nullptr)
        {
            if (compare(key, node->key))
            {
                node = node->left;
            }
            else if (compare(node->key, key))
            {
                node = node->right;
            }
            else
            {
                node->frequence = value;
                return;
            }
        }
    }

    T find(T key)
    {
        RBNode<T> *node = root;
        while (node != nullptr)
        {
            if (compare(key, node->key))
            {
                node = node->left;
            }
            else if (compare(node->key, key))
            {
                node = node->right;
            }
            else
            {
                return node->key;
            }
        }
        return T();
    }

    void print() const
    {
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
