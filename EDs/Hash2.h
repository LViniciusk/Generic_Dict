#ifndef HASHTABLE2_H
#define HASHTABLE2_H

#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <functional>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>
#include "extras.h"

// Template de classe Hash2Table com parâmetros genéricos para a chave (Key), valor (Value),
// comparador (COMPARATOR) e função de hash (Hash)
template <typename Key, typename Value = int, typename COMPARATOR = comparator<Key>, typename Hash = std::hash<Key>>
class Hash2Table
{
private:
    // Enumeração que define o estado de uma entrada na tabela de hash
    enum EntryState
    {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    // Estrutura que representa uma entrada na tabela de hash
    struct Entry
    {
        Key key;
        Value value;
        EntryState state;

        Entry() : state(EMPTY) {} // Construtor que inicializa a entrada como vazia
    };

    size_t m_number_of_elements; // Número de elementos inseridos na tabela
    size_t m_table_size;         // Tamanho da tabela de hash (número de buckets)
    std::vector<Entry> m_table;  // Vetor que representa a tabela de hash
    float m_load_factor;         // Fator de carga atual da tabela (número de elementos / tamanho da tabela)
    float m_max_load_factor;     // Fator de carga máximo permitido antes de rehashing
    Hash m_hashing;              // Função de hash
    unsigned int comps = 0;      // Contador de comparações realizadas
    COMPARATOR compare;          // Comparador para ordenar os elementos

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

    // Função privada que calcula o código de hash para uma chave e um índice de tentativa (para resolução de colisões)
    size_t hash_code(const Key &k, size_t i) const
    {
        return (m_hashing(k) + i) % m_table_size;
    }

    // Função privada que imprime os elementos da tabela de hash de forma ordenada
    void ordered_print()
    {
        std::vector<std::pair<Key, Value>> elements;
        elements.reserve(m_number_of_elements); // Reserva espaço para todos os elementos

        // Coleta todos os elementos ocupados da tabela de hash
        for (size_t i = 0; i < m_table_size; ++i)
        {
            if (m_table[i].state == OCCUPIED)
            {
                elements.push_back({m_table[i].key, m_table[i].value});
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
            {
                std::cout << p.first << ": " << p.second << std::endl;
            }
        }
        std::cout << std::endl;
    }

public:
    // Desabilita a cópia da tabela hash
    Hash2Table(const Hash2Table &t) = delete;
    Hash2Table &operator=(const Hash2Table &t) = delete;

    // Construtor que inicializa a tabela de hash com um tamanho inicial e outros parâmetros opcionais
    Hash2Table(size_t tableSize = 19, const Hash &hf = Hash(), COMPARATOR comp = COMPARATOR())
    {
        compare = comp;
        m_number_of_elements = 0;
        m_table_size = tableSize;
        m_table.resize(m_table_size); // Redimensiona o vetor de entradas para o tamanho inicial
        m_load_factor = 0.75;
        m_max_load_factor = 1.0;
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

    // Limpa a tabela de hash, removendo todos os elementos
    void clear()
    {
        m_table.clear();
        m_table.resize(m_table_size); // Redimensiona a tabela para o tamanho inicial
        m_number_of_elements = 0;
    }

    // Retorna o fator de carga atual
    float load_factor() const
    {
        return static_cast<float>(m_number_of_elements) / m_table_size;
    }

    // Retorna o fator de carga máximo
    float max_load_factor() const
    {
        return m_max_load_factor;
    }

    // Destrutor que limpa a tabela
    ~Hash2Table()
    {
        clear();
    }

    // Insere uma chave e um valor na tabela de hash, realiza rehash se necessário
    bool insert(const Key &k, const Value &v = 1)
    {
        // Verifica se o fator de carga ultrapassou o limite e realiza rehash se necessário
        if (load_factor() > m_load_factor)
        {
            rehash(2 * m_table_size);
        }

        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == EMPTY || m_table[index].state == DELETED)
            {
                // Insere nova chave-valor na posição encontrada
                m_table[index].key = k;
                m_table[index].value = v;
                m_table[index].state = OCCUPIED;
                m_number_of_elements++;
                return true;
            }
            else if (m_table[index].key == k)
            {
                comps++;
                return false;
            }
            comps++;
        } while (i < m_table_size);

        return false; // Retorna falso se não conseguir inserir (caso de tabela cheia)
    }

    // Verifica se uma chave está presente na tabela
    bool contains(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == EMPTY)
                return false;
            comps++;
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
                return true;
        } while (i < m_table_size);
        return false;
    }

    // Busca o valor associado a uma chave na tabela
    Value &find(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            comps++;
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
                return m_table[index].value;
        } while (i < m_table_size);
        throw std::out_of_range("Key not found"); // Lança exceção se a chave não for encontrada
    }


    // Reorganiza a tabela de hash com um novo tamanho
    void rehash(size_t m)
    {
        if (m <= m_table_size)
            return;

        size_t new_size = get_next_prime(m); // Obtém o próximo primo para o novo tamanho
        std::vector<Entry> new_table(new_size);

        for (size_t i = 0; i < m_table_size; i++)
        {
            if (m_table[i].state == OCCUPIED)
            {
                size_t j = 0;
                size_t index;
                do
                {
                    index = (m_hashing(m_table[i].key) + j++) % new_size; // Recalcula o índice para a nova tabela
                } while (new_table[index].state == OCCUPIED);

                new_table[index] = m_table[i]; // Move a entrada para a nova tabela
            }
        }

        m_table = std::move(new_table); // Substitui a tabela antiga pela nova
        m_table_size = new_size;
    }

    // Remove um elemento da tabela com base na chave
    bool remove(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == EMPTY)
                return false;
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
            {
                m_table[index].state = DELETED; // Marca a entrada como deletada
                m_number_of_elements--;
                return true;
            }
        } while (i < m_table_size);
        return false;
    }

    // Atualiza o valor associado a uma chave na tabela
    bool update(const Key &k, const Value &v)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            comps++;
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
            {
                m_table[index].value = v; // Atualiza o valor da chave
                return true;
            }
        } while (i < m_table_size);
        return false;
    }

    // Imprime a tabela (por padrão, imprime de forma ordenada)
    void print()
    {
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

    // Operador de índice para acessar ou criar elementos na tabela
    Value &operator[](const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == EMPTY)
            {
                m_table[index].key = k;
                m_table[index].value = Value();
                m_table[index].state = OCCUPIED;
                m_number_of_elements++;
                return m_table[index].value;
            }
            else if (m_table[index].key == k)
            {
                comps++;
                return m_table[index].value;
            }
        } while (i < m_table_size);
        throw std::out_of_range("Key not found");
    }

    // Operador de índice const para acessar elementos na tabela
    // const Value &operator[](const Key &k) const
    // {
    //     size_t i = 0;
    //     size_t index;
    //     do
    //     {
    //         index = hash_code(k, i++);
    //         if (m_table[index].state == OCCUPIED && m_table[index].key == k)
    //         {
    //             return m_table[index].value;
    //         }
    //     } while (i < m_table_size);
    //     throw std::out_of_range("Key not found"); // Lança exceção se a chave não for encontrada
    // }
};

#endif
