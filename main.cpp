#include <chrono>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <variant>
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

int main(int argc, char *argv[])
{
    // Verifica se o número de argumentos está correto
    if (argc != 3)
    {
        cerr << "Invalid Arguments, open Readme.txt" << endl;
        return 1;
    }

    // Abre o arquivo de saída
    ofstream out("./output/saida.txt");
    if (!out)
    {
        cerr << "Error opening output file" << endl;
        return 1;
    }

    // Redireciona cout para o arquivo de saída
    streambuf *coutbuf = cout.rdbuf(); // salva o buffer original
    cout.rdbuf(out.rdbuf());           // redireciona cout para o arquivo

    // Define o modo de estrutura
    int mode_structure = std::stoi(argv[1]);

    // Define o nome do arquivo
    string filename = argv[2];

    // Verifica se a estrutura fornecida é válida
    if (mode_structure == 1) // AVL
    {
        Dict<AVLTree<UnicodeString, int, u_comparator>> dict;

        // Inicia a contagem do tempo
        auto start = high_resolution_clock::now();

        // Lê o arquivo e insere as palavras no dicionário
        stringstream file = LoadFile("./Textos/" + filename);
        dict.add(file);

        // Finaliza a contagem do tempo e calcula a duração
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        // Imprime o tempo de execução
        cout << "Estrutura de Dados: " << TypeName(typeid(dict).name()) << endl;
        cout << "Nome do arquivo: " << filename << endl;
        cout << "Numero de palavras: " << dict.size() << endl;
        cout << "Numero de Comparações: " << dict.comparisons() << endl;
        cout << "Tempo de execução: " << duration.count() << "ms" << endl;
        cout << "Lista de palavras: " << endl
             << endl;
        dict.print();
    }
    else if (mode_structure == 2) // RB
    {
        Dict<RBTree<UnicodeString, int, u_comparator>> dict;

        // Inicia a contagem do tempo
        auto start = high_resolution_clock::now();

        // Lê o arquivo e insere as palavras no dicionário
        stringstream file = LoadFile("./Textos/" + filename);
        dict.add(file);

        // Finaliza a contagem do tempo e calcula a duração
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        // Imprime o tempo de execução
        cout << "Estrutura de Dados: " << TypeName(typeid(dict).name()) << endl;
        cout << "Nome do arquivo: " << filename << endl;
        cout << "Numero de palavras: " << dict.size() << endl;
        cout << "Numero de Comparações: " << dict.comparisons() << endl;
        cout << "Tempo de execução: " << duration.count() << "ms" << endl;
        cout << "Lista de palavras: " << endl
             << endl;
        dict.print();
    }
    else if (mode_structure == 3) // Hash2
    {
        Dict<Hash2Table<UnicodeString, int, u_comparator>> dict;

        // Inicia a contagem do tempo
        auto start = high_resolution_clock::now();

        // Lê o arquivo e insere as palavras no dicionário
        stringstream file = LoadFile("./Textos/" + filename);
        dict.add(file);

        // Finaliza a contagem do tempo e calcula a duração
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        // Imprime o tempo de execução
        cout << "Estrutura de Dados: " << TypeName(typeid(dict).name()) << endl;
        cout << "Nome do arquivo: " << filename << endl;
        cout << "Numero de palavras: " << dict.size() << endl;
        cout << "Numero de Comparações: " << dict.comparisons() << endl;
        cout << "Tempo de execução: " << duration.count() << "ms" << endl;
        cout << "Lista de palavras: " << endl
             << endl;
        dict.print();
    }
    else if (mode_structure == 4) // Hash
    {
        Dict<HashTable<UnicodeString, int, u_comparator>> dict;

        // Inicia a contagem do tempo
        auto start = high_resolution_clock::now();

        // Lê o arquivo e insere as palavras no dicionário
        stringstream file = LoadFile("./Textos/" + filename);
        dict.add(file);

        // Finaliza a contagem do tempo e calcula a duração
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        // Imprime o tempo de execução
        cout << "Estrutura de Dados: " << TypeName(typeid(dict).name()) << endl;
        cout << "Nome do arquivo: " << filename << endl;
        cout << "Numero de palavras: " << dict.size() << endl;
        cout << "Numero de Comparações: " << dict.comparisons() << endl;
        cout << "Tempo de execução: " << duration.count() << "ms" << endl;
        cout << "Lista de palavras: " << endl
             << endl;
        dict.print();
    }
    else
    {
        cerr << "Invalid Arguments, open Readme.txt" << endl;
        return 1;
    }

    // Restaura o buffer original do cout
    cout.rdbuf(coutbuf);

    return 0;
}

// comparações
// rb - 15030316 - 15M - 882ms
// avl - 24979970 - 24.9M - 1149ms
// hash - 1263239 - 1.2M - 573ms
// hash2 - 1313671 - 1.3M - 552ms