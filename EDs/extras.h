#ifndef EXTRAS_H
#define EXTRAS_H

#include <iostream>
#include <string>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/coll.h>

// Template de uma estrutura de comparador genérico
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
        // Função hash que converte icu::UnicodeString para UTF-8 e calcula o hash do std::string resultante
        size_t operator()(const icu::UnicodeString &str) const
        {
            std::string utf8str;
            str.toUTF8String(utf8str);                // Converte para string UTF-8
            return std::hash<std::string>()(utf8str); // Retorna o hash da string UTF-8
        }
    };
}

// Estrutura para comparação de icu::UnicodeString utilizando um icu::Collator
struct u_comparator
{
    icu::Collator *collator; // Ponteiro para um objeto Collator do ICU

    // Construtor que inicializa o comparador com um Collator existente
    u_comparator(icu::Collator *c) { collator = c; }

    // Construtor padrão que cria uma instância do Collator com a configuração padrão
    u_comparator()
    {
        UErrorCode status = U_ZERO_ERROR;
        collator = icu::Collator::createInstance(status);
    }

    // Operador de comparação que utiliza o Collator para comparar duas icu::UnicodeString
    bool operator()(const icu::UnicodeString &a, const icu::UnicodeString &b) const
    {
        UErrorCode status = U_ZERO_ERROR;
        return collator->compare(a, b, status) < 0;
    }
};

#endif
