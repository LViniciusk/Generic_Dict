#include <chrono>
#include <iostream>
#include <fstream>
#include <filesystem>
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

// função que executa a estrutura de dados
template <typename dicts>
void run(dicts &dict, string filename)
{
    // Inicia a contagem do tempo
    auto start = high_resolution_clock::now();

    // Lê o arquivo e insere as palavras no dicionário
    stringstream file = LoadFile("./Textos/" + filename);

    std::string word;
    while (file >> word)
    {
        dict.add(icu::UnicodeString::fromUTF8(
            icu::StringPiece(word.c_str(), word.size())));
    }

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

int main(int argc, char *argv[])
{
    // Verifica se o número de argumentos está correto
    if (argc != 3)
    {
        cerr << "Invalid Arguments, open Readme.txt" << endl;
        return 1;
    }

    namespace fs = std::filesystem;

    std::string dirPath = "./output/" + std::string(argv[2]).substr(0, std::string(argv[2]).size() - 4) + "/";

    // Verifica se o diretório existe; se não, cria o diretório
    if (!fs::exists(dirPath))
    {
        fs::create_directories(dirPath);
    }

    // Abre o arquivo de saída
    std::ofstream out(dirPath + std::string(argv[1]) + '-' + std::string(argv[2]));

    if (!out)
    {
        cerr << "Error opening output file" << endl;
        return 1;
    }

    // Redireciona cout para o arquivo de saída
    streambuf *coutbuf = cout.rdbuf(); // salva o buffer original
    cout.rdbuf(out.rdbuf());           // redireciona cout para o arquivo

    // Define o modo de estrutura
    int mode = std::stoi(argv[1]);

    // Define o nome do arquivo
    string filename = argv[2];

    // Verifica se a estrutura fornecida é válida
    if (mode == 1) // AVL
    {
        Dict<AVLTree<UnicodeString, int, u_comparator>> dict;
        run(dict, filename);
    }
    else if (mode == 2) // RB
    {
        Dict<RBTree<UnicodeString, int, u_comparator>> dict;
        run(dict, filename);
    }
    else if (mode == 3) // Hash2
    {
        Dict<Hash2Table<UnicodeString, int, u_comparator>> dict;
        run(dict, filename);
    }
    else if (mode == 4) // Hash
    {
        Dict<HashTable<UnicodeString, int, u_comparator>> dict;
        run(dict, filename);
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

// comparações ANTES
// AVLTree - 24979970 - 24.9M - 1149ms
// RBTree - 15030316 - 15M - 882ms
// Hash2Table - 1313671 - 1.3M - 552ms
// HashTable - 1263239 - 1.2M - 573ms

// comparações DEPOIS
// AVLTree - 16373306 - 16.3M - 719ms
// RBTree - 15428509 - 15.4M - 693ms
// Hash2Table - 977375 - 0.9M - 467ms
// HashTable - 1274778 - 1.2M - 434ms