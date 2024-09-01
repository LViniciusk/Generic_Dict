#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>
#include <unicode/ustream.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;
using namespace icu;

// Função que retorna o tipo da estrutura de dados
string TypeName(string type)
{
    if (type.find("HashTable") != string::npos)
        return "HashTable Separate Chaining";
    else if (type.find("AVLTree") != string::npos)
        return "AVLTree";
    else if (type.find("RBTree") != string::npos)
        return "RBTree";
    else if (type.find("Hash2Table") != string::npos)
        return "HashTable Open Addressing";
    else
        return "Unknown";
}

// Função que normaliza um texto Unicode, convertendo para minúsculas e substituindo caracteres não alfabéticos por espaços
void Normalize(UnicodeString &text)
{
    text.toLower(); // Converte todo o texto para minúsculas

    for (int i = 0; i < text.length(); ++i)
    {
        UChar c = text[i];
        // Verifica se o caractere não é uma letra
        if (!u_isalpha(c))
        {
            // Verifica se o caractere é um hífen entre duas letras, caso sim, mantém o hífen
            bool vHifen = (c == '-') && (i > 0 && i < text.length() - 1) && u_isalpha(text[i - 1]) && u_isalpha(text[i + 1]);

            if (!vHifen)
            {
                text.setCharAt(i, ' '); // Substitui o caractere por um espaço
            }
        }
    }
}

// Função que carrega o conteúdo de um arquivo e retorna como um stringstream após normalização
stringstream LoadFile(const string &path)
{
    ifstream file(path, ios::binary); // Abre o arquivo em modo binário
    if (!file.is_open())
    {
        cerr << "Error: Could not open file " << path << endl;
        exit(EXIT_FAILURE); // Sai do programa em caso de erro na abertura do arquivo
    }

    // Carrega o conteúdo do arquivo em uma string
    string fdata((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    // Converte a string carregada para UnicodeString, normaliza e retorna como stringstream
    UnicodeString udata = UnicodeString::fromUTF8(fdata); // Converte para UnicodeString
    Normalize(udata);                                     // Normaliza o texto

    string normalizedText;
    udata.toUTF8String(normalizedText);  // Converte de volta para string UTF-8
    return stringstream(normalizedText); // Retorna um stringstream com o texto normalizado
}
