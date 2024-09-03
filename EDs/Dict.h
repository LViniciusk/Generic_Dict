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
        try{
            _dict[key] += value;
        } catch (std::out_of_range &e){
            _dict.insert(key, value);
        }
    }

    void print()
    {
        _dict.print();
    }

    void remove(icu::UnicodeString key)
    {
        _dict.remove(key);
    }

    void update(icu::UnicodeString key, unsigned int value)
    {
        _dict.update(key, value);
    }

    void find(icu::UnicodeString key)
    {
        _dict.find(key);
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
};

#endif