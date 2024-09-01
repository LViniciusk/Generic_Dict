#ifndef RBTREE_H
#define RBTREE_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

// Definição das cores dos nós em uma árvore rubro negra
enum Color
{
    RED,
    BLACK
};

// Estrutura de um nó da árvore rubro negra
template <typename T, typename Value = int>
struct RBNode
{
    T key;                    // Chave do nó
    RBNode<T, Value> *left;   // Ponteiro para o filho esquerdo
    RBNode<T, Value> *right;  // Ponteiro para o filho direito
    RBNode<T, Value> *parent; // Ponteiro para o nó pai
    Color color;              // Cor do nó (vermelho ou preto)
    Value frequence;          // Frequência associada à chave

    // Construtor do nó
    RBNode(T k, Value v = 1) : key(k), left(nullptr), right(nullptr), parent(nullptr), color(RED), frequence(v) {}
};

// Classe da árvore rubro negra
template <typename T, typename Value = int, typename COMPARATOR = comparator<T>>
class RBTree
{
private:
    RBNode<T, Value> *root = nullptr; // Raiz da árvore
    COMPARATOR compare;               // Função de comparação
    unsigned int comps = 0;           // Contador de comparações
    unsigned int _size = 0;           // Tamanho da árvore (número de nós)

    // Rotação à esquerda para manutenção da propriedade da árvore rubro negra
    void leftRotate(RBNode<T, Value> *x)
    {
        RBNode<T, Value> *y = x->right;
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

    // Rotação à direita para manutenção da propriedade da árvore rubro negra
    void rightRotate(RBNode<T, Value> *x)
    {
        RBNode<T, Value> *y = x->left;
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

    // Ajuste da árvore após a inserção para manter as propriedades rubro negra
    void fixupInsert(RBNode<T, Value> *z)
    {
        while (z->parent != nullptr && z->parent->color == RED)
        {
            if (z->parent == z->parent->parent->left)
            {
                RBNode<T, Value> *y = z->parent->parent->right;
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
                RBNode<T, Value> *y = z->parent->parent->left;
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

    // Ajuste da árvore após a remoção para manter as propriedades rubro negra
    void fixupDelete(RBNode<T, Value> *x)
    {
        while (x != root && x->color == BLACK)
        {
            if (x == x->parent->left)
            {
                RBNode<T, Value> *w = x->parent->right;
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
                RBNode<T, Value> *w = x->parent->left;
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

    // Função auxiliar para remoção de um nó com uma determinada chave
    RBNode<T, Value> *_delete(RBNode<T, Value> *node, T key, unsigned int value = 1)
    {
        RBNode<T, Value> *z = root;
        RBNode<T, Value> *y = nullptr;
        RBNode<T, Value> *x = nullptr;

        // Encontra o nó a ser removido
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

        // Se o nó encontrado tem no máximo um filho, y será z
        if (z->left == nullptr || z->right == nullptr)
        {
            y = z;
        }
        else
        {
            // Caso contrário, encontre o sucessor de z
            y = z->right;
            while (y->left != nullptr)
                y = y->left;
        }

        // x é o filho de y
        if (y->left != nullptr)
            x = y->left;
        else
            x = y->right;

        // Reconecta o pai de y ao filho de y
        if (x != nullptr)
            x->parent = y->parent;

        if (y->parent == nullptr)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;

        // Se y não é o nó a ser removido, mova os dados de y para z
        if (y != z)
        {
            z->key = y->key;
            z->frequence = y->frequence;
        }

        // Se y era preto, a árvore pode precisar de ajustes
        if (y->color == BLACK && x != nullptr)
            fixupDelete(x);

        // Libera a memória de y e atualiza o tamanho da árvore
        delete y;
        _size--;
        return root;
    }

    // Função auxiliar para inserção de um novo nó
    RBNode<T, Value> *_insert(RBNode<T, Value> *node, T key, unsigned int value = 1)
    {
        if (node == nullptr)
        {
            _size++;
            return new RBNode<T, Value>(key, value);
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

    // Função auxiliar para imprimir a árvore em ordem
    void _print(RBNode<T, Value> *node) const
    {
        if (node == nullptr)
            return;

        _print(node->left);
        if constexpr (std::is_same<T, icu::UnicodeString>::value)
        {
            std::string skey;
            node->key.toUTF8String(skey);
            std::cout << skey << ": " << node->frequence << std::endl;
        }
        else
        {
            std::cout << node->key << ": " << node->frequence << std::endl;
        }
        _print(node->right);
    }

    // Função auxiliar para limpar a árvore
    void _clear(RBNode<T, Value> *node)
    {
        if (node == nullptr)
            return;

        // Primeiro, limpe as subárvores à esquerda e à direita
        _clear(node->left);
        _clear(node->right);

        // Depois de limpar as subárvores, delete o nó atual
        delete node;
    }

    // Função auxiliar para verificar se a árvore contém uma chave
    bool _contains(RBNode<T, Value> *node, T key)
    {
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
                return true;
            }
        }
        return false;
    }

public:

    // Construtor da árvore, inicializa os membros
    RBTree(COMPARATOR comp = COMPARATOR()) : compare(comp)
    {
        root = nullptr;
        _size = 0;
    }

    // Função para inserir um nó na árvore
    void insert(T key, unsigned int value = 1)
    {
        RBNode<T, Value> *newNode = _insert(root, key, value);
        root = newNode;
        fixupInsert(newNode);
    }

    // Função para remover um nó da árvore
    void remove(T key, unsigned int value = 1)
    {
        root = _delete(root, key, value);
    }

    // Função para atualizar o valor associado a uma chave na árvore
    void update(T key, unsigned int value)
    {
        RBNode<T, Value> *node = root;
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

    // Função para encontrar uma chave na árvore
    T find(T key)
    {
        RBNode<T, Value> *node = root;
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

    // Função para imprimir a árvore em ordem
    void print() const
    {
        _print(root);
    }

    // Função para limpar a árvore
    void clear()
    {
        _clear(root);

        // Após limpar, defina a raiz como nullptr e o tamanho da árvore como 0
        root = nullptr;
        _size = 0;
    }

    // Função para verificar se a árvore contém uma chave
    bool contains(T key)
    {
        return _contains(root, key);
    }

    // Retorna o número de comparações realizadas
    size_t comparisons()
    {
        return comps;
    }

    // Retorna o tamanho da árvore (número de nós)
    size_t size() const
    {
        return _size;
    }
};

#endif
