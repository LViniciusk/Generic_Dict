#ifndef DICT_H
#define DICT_H

#include <iostream>
#include "AVLTree.h"
#include "RBTree.h"
#include "Hash.h"
#include "Hash2.h"

template <typename KeyType>
class Dict
{
private:
    KeyType _dict;

public:
    void add(std::stringstream &file)
    {
        std::string word;
        while (file >> word)
        {
            _dict.insert(icu::UnicodeString::fromUTF8(
                icu::StringPiece(word.c_str(), word.size())));
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