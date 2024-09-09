#ifndef DICT_H
#define DICT_H

#include <iostream>
#include "AVLTree.h"
#include "RBTree.h"
#include "Hash.h"
#include "Hash2.h"

template <typename EDType>
class Dict
{
private:
    EDType _dict;

public:
    void add(icu::UnicodeString key, unsigned int value = 1)
    {
        try
        {
            _dict[key] += value;
        }
        catch (std::out_of_range &e)
        {
            _dict.insert(key, value);
        }
    }

    void remove(icu::UnicodeString key)
    {
        try
        {
            _dict[key] -= 1;
            if (_dict[key] <= 0)
                _dict.remove(key);
        }
        catch (std::out_of_range &e)
        {
            std::cerr << "Key not found" << std::endl;
        }
    }

    void update(icu::UnicodeString key, unsigned int value)
    {
        _dict.update(key, value);
    }

    int find(icu::UnicodeString key)
    {
        return _dict.find(key);
    }

    void clear()
    {
        _dict.clear();
    }

    bool contains(icu::UnicodeString key)
    {
        return _dict.contains(key);
    }

    size_t size()
    {
        return _dict.size();
    }

    size_t comparisons()
    {
        return _dict.comparisons();
    }

    void print()
    {
        _dict.print();
    }
};

#endif