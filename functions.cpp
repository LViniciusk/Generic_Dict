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

void format_text(UnicodeString &text)
{
    text.toLower();

    for (int i = 0; i < text.length(); i++)
    {

        UChar character = text[i];

        if (!u_isalpha(character))
        {

            bool is_hyphen_between_words =
                (character == '-') && (i > 0 && u_isalpha(text[i - 1])) &&
                (i < text.length() - 1 && u_isalpha(text[i + 1]));

            if (!is_hyphen_between_words)
            {
                text.replace(i, 1, ' ');
            }
        }
    }
}

stringstream read_file(const string &file_path)
{
    // Abre o arquivo
    ifstream input_file(file_path, ios::binary);
    if (!input_file.is_open())
    {
        cerr << "Error: Could not open file " << file_path << endl;
        exit(1);
    }

    string file_content((istreambuf_iterator<char>(input_file)),
                        istreambuf_iterator<char>());
    UnicodeString unicode_content =
        UnicodeString::fromUTF8(StringPiece(file_content));

    format_text(unicode_content);

    string processed_content;
    unicode_content.toUTF8String(processed_content);

    return stringstream(processed_content);
}

string TypeName(string type)
{
    if (type == "4DictI9HashTableIN6icu_7513UnicodeStringEi12u_comparatorSt4hashIS2_EEE")
        return "HashTable Separate Chaining";
    else if (type == "4DictI7AVLTreeIN6icu_7513UnicodeStringE12u_comparatorEE")
        return "AVLTree";
    else if (type == "4DictI6RBTreeIN6icu_7513UnicodeStringE12u_comparatorEE")
        return "RBTree";
    else if (type == "4DictI10HashTable2IN6icu_7513UnicodeStringEi12u_comparatorSt4hashIS2_EEE")
        return "HashTable Open Addressing";
    else
        return "Unknown";
}

void display_usage(const char *program_name) {
    cerr << endl << "Uso: " << " <modo_estrutura> <arquivo(deve estar na pasta Textos)>" << endl
         << "Modos de estrutura suportados: "<< endl << "1 = dict_avl" << endl << "2 = dict_rb" << endl << "3 = dict_hash_addressing" << endl <<  "4 = dict_hash_chaining" << endl;
}


bool is_valid_structure(const int &mode_structure) {
    return mode_structure >= 1 && mode_structure <= 4;
}