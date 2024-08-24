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

template <typename Key, typename Value = int, typename COMPARATOR = comparator<Key>, typename Hash = std::hash<Key>>
class HashTable
{
private:
    size_t m_number_of_elements;
    size_t m_table_size;
    std::vector<std::list<std::pair<Key, Value>>> *m_table;
    float m_load_factor;
    float m_max_load_factor;
    Hash m_hashing;
    unsigned int comps = 0;
    COMPARATOR compare;

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

    size_t hash_code(const Key &k) const
    {
        return m_hashing(k) % m_table_size;
    }

    void unordered_print()
    {
        std::cout << "HashTable" << std::endl;
        for (size_t i = 0; i < m_table_size; i++)
        {
            std::cout << i << ": ";
            for (auto &p : (*m_table)[i])
            {
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

    void ordered_print()
    {
        std::cout << "Ordered HashTable" << std::endl;
        std::vector<std::pair<Key, Value>> elements;
        elements.reserve(m_number_of_elements);

        for (size_t i = 0; i < m_table_size; ++i)
        {
            for (const auto &p : (*m_table)[i])
            {
                elements.push_back(p);
            }
        }

        std::sort(elements.begin(), elements.end(), [this](const std::pair<Key, Value> &a, const std::pair<Key, Value> &b)
                  { return compare(a.first, b.first); });

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
    HashTable(const HashTable &t) = delete;
    HashTable &operator=(const HashTable &t) = delete;

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

    size_t size() const
    {
        return m_number_of_elements;
    }

    bool empty() const
    {
        return m_number_of_elements == 0;
    }

    size_t bucket_count() const
    {
        return m_table_size;
    }

    size_t bucket_size(size_t n) const
    {
        return (*m_table)[n].size();
    }

    size_t bucket(const Key &k) const
    {
        return hash_code(k);
    }

    void clear()
    {
        for (size_t i = 0; i < m_table_size; i++)
        {
            (*m_table)[i].clear();
        }
        m_number_of_elements = 0;
    }

    float load_factor() const
    {
        return m_load_factor;
    }

    float max_load_factor() const
    {
        return m_max_load_factor;
    }

    ~HashTable()
    {
        clear();
        delete m_table;
    }

    bool insert(const Key &k, const Value &v = 1)
    {
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
                p.second += v;
                return true;
            }
        }
        (*m_table)[i].push_back(std::make_pair(k, v));
        m_number_of_elements++;
        return true;
    }

    bool contains(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            if (p.first == k)
            {
                return true;
            }
        }
        return false;
    }

    Value &at(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            if (p.first == k)
            {
                return p.second;
            }
        }
        throw std::out_of_range("Key not found");
    }

    void rehash(size_t m)
    {
        if (m <= m_table_size)
            return;
        size_t new_size = get_next_prime(m);
        std::vector<std::list<std::pair<Key, Value>>> *new_table = new std::vector<std::list<std::pair<Key, Value>>>(new_size);
        for (size_t i = 0; i < m_table_size; i++)
        {
            for (auto &p : (*m_table)[i])
            {
                size_t j = m_hashing(p.first) % new_size;
                (*new_table)[j].push_back(p);
            }
        }
        delete m_table;
        m_table = new_table;
        m_table_size = new_size;
    }

    bool remove(const Key &k)
    {
        size_t i = hash_code(k);
        for (auto it = (*m_table)[i].begin(); it != (*m_table)[i].end(); ++it)
        {
            if (it->first == k)
            {
                (*m_table)[i].erase(it);
                m_number_of_elements--;
                return true;
            }
        }
        return false;
    }

    bool update(const Key &k, const Value &v)
    {
        size_t i = hash_code(k);
        for (auto it = (*m_table)[i].begin(); it != (*m_table)[i].end(); ++it)
        {
            if (it->first == k)
            {
                it->second = v;
                return true;
            }
        }
        return false;
    }

    void print()
    {
        //unordered_print();
        ordered_print();
    }

    size_t comparisons()
    {
        return comps;
    }

    void reserve(size_t n)
    {
        if (n > m_table_size * m_load_factor)
        {
            rehash(n);
        }
    }

    void load_factor(float lf)
    {
        if (lf <= 0 || lf > m_max_load_factor)
        {
            throw std::out_of_range("out of range load factor");
        }
        m_load_factor = lf;
        reserve(m_number_of_elements);
    }

    Value &operator[](const Key &k)
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            if (p.first == k)
            {
                return p.second;
            }
        }
        (*m_table)[i].push_back(std::make_pair(k, Value()));
        m_number_of_elements++;
        return (*m_table)[i].back().second;
    }

    const Value &operator[](const Key &k) const
    {
        size_t i = hash_code(k);
        for (auto &p : (*m_table)[i])
        {
            if (p.first == k)
            {
                return p.second;
            }
        }
        throw std::out_of_range("Key not found");
    }
};

#endif
