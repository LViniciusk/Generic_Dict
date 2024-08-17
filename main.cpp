#include <iostream>
#include <fstream>
#include <string>
#include "./EDs/AVLTree.h"
#include "./EDs/Dict.h"
#include <unicode/unistr.h>  
#include <unicode/ustream.h> 
#include <unicode/ucnv.h>    
#include <unicode/uchar.h>
#include <unicode/locid.h>
#include <unicode/coll.h>

using namespace std;
using namespace icu;

int main() {

    // cria uma árvore AVL de strings
    //AVLTree<icu::UnicodeString, u_comparator> tree;
    Dict<AVLTree<icu::UnicodeString, u_comparator>> dict;

    // abre arquivo no modo leitura
    ifstream file("exemplo.txt");

    // teste se arquivo foi realmente abertog
    if(!file.is_open()) {
        cerr << "falha ao abrir arquivo" << endl;
        return EXIT_FAILURE;
    }

    // ler as linhas do arquivo
    string utf8line;
    while(getline(file, utf8line)) {
        icu::UnicodeString ustr = icu::UnicodeString::fromUTF8(utf8line);
        ustr.toLower();
        dict.add(ustr);
    }
    file.close();

    // imprime a árvore
    dict.print();


    return EXIT_SUCCESS;
}