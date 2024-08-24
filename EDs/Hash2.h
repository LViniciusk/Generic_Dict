#ifndef HASHTABLE2_H
#define HASHTABLE2_H

#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>

template <typename Key, typename Value = int, typename Hash = std::hash<Key>>
class HashTable2
{
private:
    enum State
    {
        EMPTY,
        OCCUPIED,
        DELETED
    };

    struct Entry
    {
        Key key;
        Value value;
        State state;
        Entry() : state(EMPTY) {}
    };

    size_t m_number_of_elements;
    size_t m_table_size;
    std::vector<Entry> *m_table;
    float m_load_factor;
    float m_max_load_factor;
    Hash m_hashing;
    unsigned int comps = 0;

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

    void unordered_print()
    {
        std::cout << "HashTable" << std::endl;
        for (size_t i = 0; i < m_table_size; i++)
        {
            std::cout << i << ": ";
            if ((*m_table)[i].state == OCCUPIED)
            {
                if constexpr (std::is_same<Key, icu::UnicodeString>::value)
                {
                    std::string skey;
                    (*m_table)[i].key.toUTF8String(skey);
                    std::cout << skey << " ";
                }
                else
                {
                    std::cout << (*m_table)[i].key << " ";
                }
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
            if ((*m_table)[i].state == OCCUPIED)
            {
                elements.emplace_back((*m_table)[i].key, (*m_table)[i].value);
            }
        }

        std::sort(elements.begin(), elements.end(), [](const std::pair<Key, Value> &a, const std::pair<Key, Value> &b)
                  { return a.first < b.first; });

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

    HashTable2(size_t tableSize = 19, const Hash &hf = Hash())
    {
        m_number_of_elements = 0;
        m_table_size = tableSize;
        m_table = new std::vector<Entry>(m_table_size);
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
        return ((*m_table)[n].state == OCCUPIED) ? 1 : 0;
    }

    size_t bucket(const Key &k) const
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
                return index;
        } while ((*m_table)[index].state != EMPTY);
        return index;
    }

    void clear()
    {
        for (size_t i = 0; i < m_table_size; i++)
        {
            (*m_table)[i] = Entry();
        }
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
        delete m_table;
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
            if ((*m_table)[index].state == EMPTY || (*m_table)[index].state == DELETED)
            {
                (*m_table)[index].key = k;
                (*m_table)[index].value = v;
                (*m_table)[index].state = OCCUPIED;
                m_number_of_elements++;
                return true;
            }
            else if ((*m_table)[index].key == k)
            {
                comps++;
                (*m_table)[index].value += v;
                return true;
            }
            comps++;
        } while (true);
    }

    bool contains(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if ((*m_table)[index].state == EMPTY)
                return false;
            comps++;
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
                return true;
        } while ((*m_table)[index].state != EMPTY);
        return false;
    }

    Value &at(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            comps++;
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
                return (*m_table)[index].value;
        } while ((*m_table)[index].state != EMPTY);
        throw std::out_of_range("Key not found");
    }

    void rehash(size_t m)
    {
        if (m <= m_table_size)
            return;
        size_t new_size = get_next_prime(m);
        std::vector<Entry> *new_table = new std::vector<Entry>(new_size);
        for (size_t i = 0; i < m_table_size; i++)
        {
            if ((*m_table)[i].state == OCCUPIED)
            {
                size_t j = 0;
                size_t index;
                do
                {
                    index = (m_hashing((*m_table)[i].key) + j++) % new_size;
                } while ((*new_table)[index].state == OCCUPIED);
                (*new_table)[index] = (*m_table)[i];
            }
        }
        delete m_table;
        m_table = new_table;
        m_table_size = new_size;
    }

    bool remove(const Key &k)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if ((*m_table)[index].state == EMPTY)
                return false;
            comps++;
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
            {
                (*m_table)[index].state = DELETED;
                m_number_of_elements--;
                return true;
            }
        } while ((*m_table)[index].state != EMPTY);
        return false;
    }

    bool update(const Key &k, const Value &v)
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if ((*m_table)[index].state == EMPTY)
                return false;
            comps++;
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
            {
                (*m_table)[index].value = v;
                return true;
            }
        } while ((*m_table)[index].state != EMPTY);
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
            if ((*m_table)[index].state == EMPTY)
            {
                (*m_table)[index].key = k;
                (*m_table)[index].value = Value();
                (*m_table)[index].state = OCCUPIED;
                m_number_of_elements++;
                return (*m_table)[index].value;
            }
            else if ((*m_table)[index].key == k)
            {
                comps++;
                return (*m_table)[index].value;
            }
            comps++;
        } while (true);
    }

    const Value &operator[](const Key &k) const
    {
        size_t i = 0;
        size_t index;
        do
        {
            index = hash_code(k, i++);
            if ((*m_table)[index].state == OCCUPIED && (*m_table)[index].key == k)
            {
                return (*m_table)[index].value;
            }
        } while (true);
        throw std::out_of_range("Key not found");
    }
};

#endif
