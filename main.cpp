#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unicode/unistr.h>
#include <unicode/ustream.h>
#include <unicode/ucnv.h>
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/coll.h>
#include "./EDs/Dict.h"
#include "./functions.cpp"

using namespace std;
using namespace std::chrono;
using namespace icu;

int main()
{
    // Cria um dicionário de palavras
    Dict<HashTable<UnicodeString>> dict;

    // Define o nome do arquivo
    string filename = "insane";

    // Inicia a contagem do tempo
    auto start = high_resolution_clock::now();

    // Lê o arquivo e insere as palavras no dicionário
    stringstream file = read_file(filename + ".txt");
    dict.add(file);

    // Finaliza a contagem do tempo e calcula a duração
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    dict.print();

    // Imprime o tempo de execução
    cout << "Estrutura de Dados: " << TypeName(typeid(dict).name()) << endl;
    cout << "Nome do arquivo: " << filename << endl;
    cout << "Numero de palavras: " << dict.size() << endl;
    cout << "Numero de Comparações: " << dict.comparisons() << endl;
    cout << "Tempo de execução: " << duration.count() << "ms" << endl;

    return 0;
}

// comparações
// rb - 15030316 - 15M - 882ms
// avl - 15832315 - 15.8M - 1149ms
// hash - 1263239 - 1.2M - 573ms
// hash2 - 1313671 - 1.3M - 552ms