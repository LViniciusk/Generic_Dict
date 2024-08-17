#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
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
    ifstream file("biblia-em-txt.txt");

    // teste se arquivo foi realmente abertog
    if(!file.is_open()) {
        cerr << "falha ao abrir arquivo" << endl;
        return EXIT_FAILURE;
    }

    string arq((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    icu::UnicodeString uarq = icu::UnicodeString::fromUTF8(arq);
    uarq.toLower();
    for(int i = 0; i < uarq.length(); i++) {
        if(!u_isUAlphabetic(uarq[i])) {
            uarq.replace(i, 1, ' ');
        }
    }

    uarq.toUTF8String(arq);

    stringstream iss(arq);

    string word;
    while(iss >> word) {
        icu::UnicodeString uword = icu::UnicodeString::fromUTF8(word);
        dict.add(uword);
    }
    

    file.close();

    string k;

    cout << dict.max().toUTF8String(k) << endl;

    // imprime a árvore
    dict.print();


    return EXIT_SUCCESS;
}