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

    void size()
    {
        _dict.size();
    }
};

#endif