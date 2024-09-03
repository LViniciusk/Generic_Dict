#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <functional>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

// Template de classe HashTable com parâmetros genéricos para a chave (Key), valor (Value),
// comparador (COMPARATOR) e função de hash (Hash)
template <typename Key, typename Value = int, typename COMPARATOR = comparator<Key>, typename Hash = std::hash<Key>>
class HashTable
{
private:
    size_t m_number_of_elements;                            // Número de elementos inseridos na tabela
    size_t m_table_size;                                    // Tamanho da tabela de hash (número de buckets)
    std::vector<std::list<std::pair<Key, Value>>> *m_table; // Ponteiro para o vetor de listas que representa a tabela de hash
    float m_load_factor;                                    // Fator de carga atual da tabela (número de elementos / tamanho da tabela)
    float m_max_load_factor;                                // Fator de carga máximo permitido antes de rehashing
    Hash m_hashing;                                         // Função de hash
    unsigned int comps = 0;                                 // Contador de comparações realizadas
    COMPARATOR compare;                                     // Comparador para ordenar os elementos

    // Função privada que retorna o próximo número primo maior ou igual a x
    size_t get_next_prime(size_t x)
    {
        if (x <= 2)
            return 3;
        x = (x % 2 == 0) ? x + 1 : x;
        bool not_prime = true;
        while (not_prime)
        {
            not_prime = false;
            for (int i = 2; i <= sqrt(x); ++i)
            {
                if (x % i == 0)
                {
                    not_prime = true;
                    break;
                }
            }
            x += 2;
        }
        return x - 2;
    }

    // Função privada que calcula o código de hash para uma chave e mapeia para o índice da tabela
    size_t hash_code(const Key &k) const
    {
        return m_hashing(k) % m_table_size;
    }

    // Função privada que imprime a tabela de hash de forma não ordenada (bucket por bucket)
    void unordered_print()
    {
        std::cout << "HashTable" << std::endl;
        for (size_t i = 0; i < m_table_size; i++)
        {
            std::cout << i << ": ";
            for (auto &p : (*m_table)[i])
            {
                // Se a chave for do tipo icu::UnicodeString, converte para UTF-8 antes de imprimir
                if constexpr (std::is_same<Key, icu::UnicodeString>::value)
                {
                    std::string skey;
                    p.first.toUTF8String(skey);
                    std::cout << skey << " ";
                }
                else
                    std::cout << p.first << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Função privada que imprime os elementos da tabela de hash de forma ordenada
    void ordered_print()
    {
        std::vector<std::pair<Key, Value>> elements;
        elements.reserve(m_number_of_elements); // Reserva espaço para todos os elementos

        // Coleta todos os elementos da tabela de hash
        for (size_t i = 0; i < m_table_size; ++i)
        {
            for (const auto &p : (*m_table)[i])
            {
                elements.push_back(p);
            }
        }

        // Ordena os elementos usando o comparador fornecido
        std::sort(elements.begin(), elements.end(), [this](const std::pair<Key, Value> &a, const std::pair<Key, Value> &b)
                  { return compare(a.first, b.first); });

        // Imprime os elementos ordenados
        for (const auto &p : elements)
        {
            if constexpr (std::is_same<Key, icu::UnicodeString>::value)
            {
                std::string skey;
                p.first.toUTF8String(skey);
                std::cout << skey << ": " << p.second << std::endl;
            }
            else
                std::cout << p.first << ": " << p.second << std::endl;
        }
        std::cout << std::endl;
    }

public:
    // Desabilita a cópia da tabela hash
    HashTable(const HashTable &t) = delete;
    HashTable &operator=(const HashTable &t) = delete;

    // Construtor que inicializa a tabela de hash com um tamanho inicial e outros parâmetros opcionais
    HashTable(size_t tableSize = 19, const Hash &hf = Hash(), COMPARATOR comp = COMPARATOR())
    {
        compare = comp;
        m_number_of_elements = 0;
        m_table_size = tableSize;
        m_table = new std::vector<std::list<std::pair<Key, Value>>>(m_table_size);
        m_load_factor = 0.75;
        m_max_load_factor = 1;
        m_hashing = hf;
    }

    // Retorna o número de elementos na tabela
    size_t size() const
    {
        return m_number_of_elements;
    }

    // Verifica se a tabela está vazia
    bool empty() const
    {
        return m_number_of_elements == 0;
    }

    // Retorna o número de buckets na tabela
    size_t bucket_count() const
    {
        return m_table_size;
    }

    // Retorna o número de elementos em um bucket específico
    size_t bucket_size(size_t n) const
    {
        return (*m_table)[n].size();
    }

    // Retorna o índice do bucket para uma dada chave
    size_t bucket(const Key &k) const
    {
        return hash_code(k);
    }

    // Limpa a tabela de hash, removendo todos os elementos
    void clear()
    {
        for (size_t i = 0; i < m_table_size; i++)
        {
            (*m_table)[i].clear();
        }
        m_number_of_elements = 0;
    }

    // Retorna o fator de carga atual
    float load_factor() const
    {
        return m_load_factor;
    }

    // Retorna o fator de carga máximo
    float max_load_factor() const
    {
        return m_max_load_factor;
    }

    // Destrutor que limpa a tabela e libera a memória alocada
    ~HashTable()
    {
        clear();
        delete m_table;
    }

    // Insere uma chave e um valor na tabela de hash, realiza rehash se necessário
    bool insert(const Key &k, const Value &v = 1)
    {
        // Verifica se o fator de carga ultrapassou o limite e realiza rehash se necessário
        if (m_number_of_elements / m_table_size > m_load_factor)
        {
            rehash(2 * m_table_size);
        }
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            comps++;
            if (p.first == k)
            {
                return false;
            }
        }
        (*m_table)[i].push_back(std::make_pair(k, v)); // Insere nova chave-valor
        m_number_of_elements++;
        return true;
    }

    // Verifica se uma chave está presente na tabela
    bool contains(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            comps++;
            if (p.first == k)
            {
                return true;
            }
        }
        return false;
    }

    // Busca o valor associado a uma chave na tabela
    Value &find(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            comps++;
            if (p.first == k)
            {
                return p.second;
            }
        }
        throw std::out_of_range("Key not found"); // Lança exceção se a chave não for encontrada
    }

    // Reorganiza a tabela de hash com um novo tamanho
    void rehash(size_t m)
    {
        if (m <= m_table_size)
            return;
        size_t new_size = get_next_prime(m); // Obtém o próximo primo para o novo tamanho
        std::vector<std::list<std::pair<Key, Value>>> *new_table = new std::vector<std::list<std::pair<Key, Value>>>(new_size);
        for (size_t i = 0; i < m_table_size; i++)
        {
            for (auto &p : (*m_table)[i])
            {
                size_t j = m_hashing(p.first) % new_size; // Recalcula o índice para a nova tabela
                (*new_table)[j].push_back(p);
            }
        }
        delete m_table; // Libera a memória da tabela antiga
        m_table = new_table;
        m_table_size = new_size;
    }

    // Remove um elemento da tabela com base na chave
    bool remove(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto it = (*m_table)[i].begin(); it != (*m_table)[i].end(); ++it)
        {
            if (it->first == k)
            {
                (*m_table)[i].erase(it); // Remove o par chave-valor do bucket
                m_number_of_elements--;
                return true;
            }
        }
        return false;
    }

    // Atualiza o valor associado a uma chave na tabela
    bool update(const Key &k, const Value &v)
    {
        size_t i = hash_code(k);
        for (auto it = (*m_table)[i].begin(); it != (*m_table)[i].end(); ++it)
        {
            comps++;
            if (it->first == k)
            {
                it->second = v; // Atualiza o valor da chave
                return true;
            }
        }
        return false;
    }

    // Imprime a tabela (por padrão, imprime de forma ordenada)
    void print()
    {
        // unordered_print();
        ordered_print();
    }

    // Retorna o número de comparações realizadas
    size_t comparisons()
    {
        return comps;
    }

    // Garante que a tabela tenha espaço suficiente para um certo número de elementos
    void reserve(size_t n)
    {
        if (n > m_table_size * m_load_factor)
        {
            rehash(n);
        }
    }

    // Define o fator de carga máximo e ajusta o tamanho da tabela se necessário
    void load_factor(float lf)
    {
        if (lf <= 0 || lf > m_max_load_factor)
        {
            throw std::out_of_range("out of range load factor");
        }
        m_load_factor = lf;
        reserve(m_number_of_elements);
    }

    // // Operador de índice para acessar ou criar elementos na tabela
    // Value &operator[](const Key &k)
    // {
    //     size_t i = hash_code(k);
    //     for (auto &p : (*m_table)[i])
    //     {
    //         comps++;
    //         if (p.first == k)
    //         {
    //             return p.second;
    //         }
    //     }
    //     (*m_table)[i].push_back(std::make_pair(k, Value())); // Cria um novo par chave-valor se a chave não existir
    //     m_number_of_elements++;
    //     return (*m_table)[i].back().second;
    // }

    // Operador de índice const para acessar elementos na tabela
    Value &operator[](const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            comps++;
            if (p.first == k)
            {
                return p.second;
            }
        }
        throw std::out_of_range("Key not found"); // Lança exceção se a chave não for encontrada
    }
};

#endif
