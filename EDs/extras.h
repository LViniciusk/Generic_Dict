#ifndef EXTRAS_H
#define EXTRAS_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>

template <typename T>
struct comparator
{
    bool operator()(const T &a, const T &b) const
    {
        return a < b;
    }
};

namespace std
{
    template <>
    struct hash<icu::UnicodeString>
    {
        size_t operator()(const icu::UnicodeString &str) const
        {
            std::string utf8str;
            str.toUTF8String(utf8str);
            return std::hash<std::string>()(utf8str);
        }
    };
}

struct u_comparator
{
    icu::Collator *collator;

    u_comparator(icu::Collator *c) { collator = c; }
    u_comparator()
    {
        UErrorCode status = U_ZERO_ERROR;
        collator = icu::Collator::createInstance(status);
    }

    bool operator()(const icu::UnicodeString &a, const icu::UnicodeString &b) const
    {
        UErrorCode status = U_ZERO_ERROR;
        return collator->compare(a, b, status) < 0;
    }
};

#endif