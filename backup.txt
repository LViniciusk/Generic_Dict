#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unicode/unistr.h>  // for UnicodeString
#include <unicode/ustream.h> // for supporting output of UnicodeString
#include <unicode/ucnv.h>    // For text file encoding conversions
using namespace std;

int main() {
    // abre arquivo no modo leitura
    ifstream file("exemplo.txt");

    // teste se arquivo foi realmente aberto
    if(!file.is_open()) {
        cerr << "falha ao abrir arquivo" << endl;
        return EXIT_FAILURE;
    }

    // ler as linhas do arquivo
    string utf8line;
    vector<string> vec_names;
    while(getline(file, utf8line)) {
        vec_names.push_back(utf8line);
    }
    file.close();

    // cria vetor de UnicodeString
    vector<icu::UnicodeString> vec_unicode_names;

    for(auto& str : vec_names) {
        // converte uma string UTF-8 para uma ICU UnicodeString
        icu::UnicodeString name = icu::UnicodeString::fromUTF8(str);
        // adiciona string convertida ao vetor de UnicodeStrings
        vec_unicode_names.push_back(name);
    }

    // Comparando duas UnicodeString com case-insensitive
    if(vec_unicode_names[1].caseCompare(vec_unicode_names[9], 0) == 0) { 
        cout << "as strings são iguais (case-insensitive)\n";
    } else {
        cout << "as strings são diferentes (case-insensitive)\n";
    }

    // obtem o numero de caracteres numa UnicodeString
    for(auto& uStr : vec_unicode_names) {
        int32_t size = uStr.length();
        string utf8str;
        uStr.toUTF8String(utf8str); // converte a UnicodeString para UTF-8
        cout << "numero de caracteres da string " << utf8str << " é " << size << "\n";
    }

    return EXIT_SUCCESS;
}