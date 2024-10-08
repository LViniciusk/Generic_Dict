#ifndef AVLTREE_H
#define AVLTREE_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

// Estrutura de nó da árvore AVL
template <typename T, typename Value>
struct Node
{
    std::pair<T, Value> key; // Chave do nó
    Node<T, Value> *left;    // Ponteiro para o filho esquerdo
    Node<T, Value> *right;   // Ponteiro para o filho direito
    int height;              // Altura do nó
    // Construtor do nó
    Node(T k, Value v) : key({k, v}), left(nullptr), right(nullptr), height(1) {}
};

// Implementação da árvore AVL com balanceamento automático
template <typename T, typename Value = int, typename COMPARATOR = comparator<T>>
class AVLTree
{
private:
    Node<T, Value> *root = nullptr; // Raiz da árvore
    COMPARATOR compare;             // Função de comparação personalizada
    unsigned int comps = 0;         // Contador de comparações
    unsigned int _size = 0;         // Número de elementos na árvore

    // Função para obter a altura de um nó
    int height(Node<T, Value> *node)
    {
        return (node == nullptr) ? 0 : node->height;
    }

    // Função para calcular o fator de balanceamento de um nó
    int balance(Node<T, Value> *node)
    {
        return (node == nullptr) ? 0 : height(node->right) - height(node->left);
    }

    // Função auxiliar para determinar o valor máximo entre dois inteiros
    int max(int a, int b)
    {
        return (a > b) ? a : b;
    }

    // Rotação à direita
    Node<T, Value> *rightRotate(Node<T, Value> *x)
    {
        Node<T, Value> *y = x->left;
        Node<T, Value> *z = y->right;
        x->left = z;
        y->right = x;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    // Rotação à esquerda
    Node<T, Value> *leftRotate(Node<T, Value> *x)
    {
        Node<T, Value> *y = x->right;
        Node<T, Value> *z = y->left;
        x->right = z;
        y->left = x;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    // Função para corrigir o balanceamento após uma inserção
    Node<T, Value> *fixupInsert(Node<T, Value> *node)
    {
        node->height = max(height(node->left), height(node->right)) + 1;
        int bal = balance(node);
        comps++; // Incrementa o contador de comparações

        // Realiza rotações conforme o balanceamento do nó
        if (bal > 1 && balance(node->right) >= 0)
        {
            return leftRotate(node);
        }
        else if (bal > 1 && balance(node->right) < 0)
        {
            comps++;
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        else if (bal < -1 && balance(node->left) <= 0)
        {
            comps += 2;
            return rightRotate(node);
        }
        else if (bal < -1 && balance(node->left) > 0)
        {
            comps += 3;
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        return node;
    }

    // Função para corrigir o balanceamento após uma remoção
    Node<T, Value> *fixupDelete(Node<T, Value> *node)
    {
        node->height = max(height(node->left), height(node->right)) + 1;
        int bal = balance(node);

        // Realiza rotações conforme o balanceamento do nó
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

    // Função para encontrar e deletar o sucessor de um nó
    Node<T, Value> *delete_successor(Node<T, Value> *node, Node<T, Value> *successor)
    {
        if (successor->left != nullptr)
            successor->left = delete_successor(node, successor->left);
        else
        {
            node->key.first = successor->key.first; // Substitui a chave pelo sucessor
            node->key.second = successor->key.second;
            Node<T, Value> *aux = successor->right;
            delete successor;
            return aux;
        }
        successor = fixupDelete(successor); // Corrige o balanceamento do sucessor
        return successor;
    }

    // Função recursiva para remover um nó da árvore
    Node<T, Value> *_delete(Node<T, Value> *node, T key)
    {
        if (node == nullptr)
            return node;

        // Navega pela árvore até encontrar o nó
        if (key < node->key.first)
        {
            node->left = _delete(node->left, key);
        }
        else if (key > node->key.first)
        {
            node->right = _delete(node->right, key);
        }
        else if (node->right == nullptr)
        {
            Node<T, Value> *child = node->left;
            delete node;
            return child;
        }
        else
            node->right = delete_successor(node, node->right);

        node = fixupDelete(node); // Corrige o balanceamento do nó removido
        return node;
    }

    // Função recursiva para inserir um novo nó na árvore
    Node<T, Value> *_insert(Node<T, Value> *node, T key, Value value)
    {
        if (node == nullptr)
        {
            _size++;
            return new Node<T, Value>(key, value);
        }
        comps++; // Incrementa o contador de comparações

        // Navega pela árvore para encontrar a posição de inserção
        if (compare(key, node->key.first))
        {
            node->left = _insert(node->left, key, value);
        }
        else if (compare(node->key.first, key))
        {
            comps++;
            node->right = _insert(node->right, key, value);
        }
        else
        {
            comps++;
            return node;
        }

        node = fixupInsert(node); // Corrige o balanceamento após a inserção
        return node;
    }

    // Função para atualizar a frequência de um nó
    Node<T, Value> *_update(Node<T, Value> *node, T key, Value value)
    {
        if (node == nullptr)
            return node;

        // Navega pela árvore para encontrar a chave
        comps++;
        if (compare(key, node->key.first))
        {
            node->left = _update(node->left, key, value);
        }
        else if (compare(node->key.first, key))
        {
            comps++;
            node->right = _update(node->right, key, value);
        }
        else
        {
            comps++;
            node->key.second = value; // Atualiza a frequência
        }
        return node;
    }

    // Função recursiva para imprimir a árvore em ordem (in-order)
    void _print(Node<T, Value> *node) const
    {
        if (node == nullptr)
            return;

        _print(node->left); // Imprime a subárvore esquerda
        if constexpr (std::is_same<T, icu::UnicodeString>::value)
        {
            std::string skey;
            node->key.first.toUTF8String(skey);
            std::cout << skey << ": " << node->key.second << std::endl;
        }
        else
            std::cout << node->key.first << ": " << node->key.second << std::endl;
        _print(node->right); // Imprime a subárvore direita
    }

    // Função para imprimir a árvore visualmente
    void bshow(Node<T, Value> *node, std::string heranca) const
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
            node->key.first.toUTF8String(skey);
            std::cout << skey << " (" << node->key.second << ")" << std::endl;
        }
        else
            std::cout << node->key.first << " (" << node->key.second << ")" << std::endl;
        if (node != nullptr && (node->left != nullptr || node->right != nullptr))
        {
            bshow(node->left, heranca + "l");
        }
    }

    // Função auxiliar para limpar a árvore
    void _clear(Node<T, Value> *node)
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
    bool _contains(Node<T, Value> *node, const T &key)
    {
        if (node == nullptr)
            return false; // Se o nó é nulo, a chave não está na árvore
        comps++;
        if (compare(key, node->key.first))
        {
            return _contains(node->left, key); // Procura na subárvore esquerda se a chave é menor que a chave do nó atual
        }

        else if (compare(node->key.first, key))
        {
            comps++;
            return _contains(node->right, key); // Procura na subárvore direita se a chave é maior que a chave do nó atual
        }
        else
        {
            comps++;
            return true; // A chave foi encontrada
        }
    }

public:
    // Construtor da árvore AVL
    AVLTree(COMPARATOR comp = COMPARATOR()) : compare(comp)
    {
        root = nullptr;
        _size = 0;
    }

    // Função para inserir uma chave na árvore
    void insert(T key, Value value)
    {
        root = _insert(root, key, value);
    }

    // Função para remover uma chave da árvore
    void remove(T key)
    {
        root = _delete(root, key);
    }

    // Função para atualizar a frequência de uma chave
    void update(T key, Value value)
    {
        root = _update(root, key, value);
    }

    // Função para buscar uma chave na árvore
    Value find(T key)
    {
        Node<T, Value> *node = root;
        while (node != nullptr)
        {
            comps++;
            if (compare(key, node->key.first))
            {
                node = node->left;
            }
            else if (compare(node->key.first, key))
            {
                comps++;
                node = node->right;
            }
            else
            {
                comps++;
                return node->key.second;
            }
        }
        return Value(); // Retorna um objeto default se não encontrar a chave
    }


    // Operador de índice const para acessar elementos na tabela
    Value &operator[](const T &key)
    {
        Node<T, Value> *node = root;
        while (node != nullptr)
        {
            comps++;
            if (compare(key, node->key.first))
            {
                node = node->left;
            }
            else if (compare(node->key.first, key))
            {
                comps++;
                node = node->right;
            }
            else
            {
                comps++;
                return node->key.second;
            }
        }
        throw std::out_of_range("Key not found"); // Lança exceção se a chave não for encontrada
    }

    // Função para imprimir a árvore
    void print() const
    {
        _print(root);
    }

    // Função para limpar a árvore
    void clear()
    {
        _clear(root);

        root = nullptr;
        _size = 0;
    }

    // Função que verifica se a árvore contém uma chave
    bool contains(const T &key)
    {
        return _contains(root, key); // Inicia a busca a partir da raiz
    }

    // Função para retornar o número de comparações feitas
    size_t comparisons()
    {
        return comps;
    }

    // Função para retornar o número de elementos na árvore
    size_t size() const
    {
        return _size;
    }
};

#endif
