#ifndef DICT_H
#define DICT_H

#include <iostream>
#include "AVLTree.h"

template <typename KeyType>
class Dict
{
private:
    KeyType _dict;

public:
    void add(icu::UnicodeString key, unsigned int value = 1)
    {
        _dict.insert(key, value);
    }

    void remove(icu::UnicodeString key, unsigned int value = 1)
    {
        _dict.remove(key, value);
    }

    void update(icu::UnicodeString key, unsigned int value)
    {
        _dict.update(key, value);
    }

    unsigned int size()
    {
       return _dict.size();
    }

    bool find(icu::UnicodeString key)
    {
        return _dict.find(key);
    }

    icu::UnicodeString min()
    {
        return _dict.min();
    }

    icu::UnicodeString max()
    {
        return _dict.max();
    }

    void print()
    {
        _dict.print();
    }
};

#endif