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

template <typename Key, typename Value = int, typename COMPARATOR = comparator<Key>, typename Hash = std::hash<Key>>
class HashTable2
{
private:
    enum EntryState { EMPTY, OCCUPIED, DELETED };

    struct Entry {
        Key key;
        Value value;
        EntryState state;

        Entry() : state(EMPTY) {}
    };

    size_t m_number_of_elements;
    size_t m_table_size;
    std::vector<Entry> m_table;
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

    size_t hash_code(const Key &k, size_t i) const
    {
        return (m_hashing(k) + i) % m_table_size;
    }

    void ordered_print()
    {
        std::vector<std::pair<Key, Value>> elements;
        elements.reserve(m_number_of_elements);

        for (size_t i = 0; i < m_table_size; ++i)
        {
            if (m_table[i].state == OCCUPIED)
            {
                elements.push_back({m_table[i].key, m_table[i].value});
            }
        }

        std::sort(elements.begin(), elements.end(), [this](const std::pair<Key, Value> &a, const std::pair<Key, Value> &b)
        {
            return compare(a.first, b.first);
        });

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
    HashTable2(const HashTable2 &t) = delete;
    HashTable2 &operator=(const HashTable2 &t) = delete;

    HashTable2(size_t tableSize = 19, const Hash &hf = Hash(), COMPARATOR comp = COMPARATOR() )
    {
        compare = comp;
        m_number_of_elements = 0;
        m_table_size = tableSize;
        m_table.resize(m_table_size);
        m_load_factor = 0.75;
        m_max_load_factor = 1.0;
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

    void clear()
    {
        m_table.clear();
        m_table.resize(m_table_size);
        m_number_of_elements = 0;
    }

    float load_factor() const
    {
        return static_cast<float>(m_number_of_elements) / m_table_size;
    }

    float max_load_factor() const
    {
        return m_max_load_factor;
    }

    ~HashTable2()
    {
        clear();
    }

    bool insert(const Key &k, const Value &v = 1)
    {
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
                m_table[index].key = k;
                m_table[index].value = v;
                m_table[index].state = OCCUPIED;
                m_number_of_elements++;
                return true;
            }
            else if (m_table[index].key == k)
            {
                comps++;
                m_table[index].value += v;
                return true;
            }
            comps++;
        } while (i < m_table_size);

        return false;
    }

    bool contains(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == EMPTY)
                return false;
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
                return true;
        } while (i < m_table_size);
        return false;
    }

    Value &at(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
                return m_table[index].value;
        } while (i < m_table_size);
        throw std::out_of_range("Key not found");
    }

    void rehash(size_t m)
    {
        if (m <= m_table_size)
            return;

        size_t new_size = get_next_prime(m);
        std::vector<Entry> new_table(new_size);

        for (size_t i = 0; i < m_table_size; i++)
        {
            if (m_table[i].state == OCCUPIED)
            {
                size_t j = 0;
                size_t index;
                do
                {
                    index = (m_hashing(m_table[i].key) + j++) % new_size;
                } while (new_table[index].state == OCCUPIED);

                new_table[index] = m_table[i];
            }
        }

        m_table = std::move(new_table);
        m_table_size = new_size;
    }

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
                m_table[index].state = DELETED;
                m_number_of_elements--;
                return true;
            }
        } while (i < m_table_size);
        return false;
    }

    bool update(const Key &k, const Value &v)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
            {
                m_table[index].value = v;
                return true;
            }
        } while (i < m_table_size);
        return false;
    }

    void print()
    {
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
                return m_table[index].value;
            }
        } while (i < m_table_size);
        throw std::out_of_range("Key not found");
    }

    const Value &operator[](const Key &k) const
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if (m_table[index].state == OCCUPIED && m_table[index].key == k)
            {
                return m_table[index].value;
            }
        } while (i < m_table_size);
        throw std::out_of_range("Key not found");
    }
};

#endif
