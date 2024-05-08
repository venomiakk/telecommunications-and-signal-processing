#ifndef ZAD3_HUFFMAN_H
#define ZAD3_HUFFMAN_H

#include <map>

/*!
 * @brief Galaz drzewa Huffmana
 */
struct TNODE
{
    // Wczytany znak
    char character;
    // Ilosc powtorzen znaku w tekscie
    int frequency;
    // Lewe i prawe dziecko
    TNODE *left, *right;

    TNODE(char character, int frequency)
    {
        left = right = NULL;
        this->character = character;
        this->frequency = frequency;
    }
};

/*!
 * @brief Porownywanie dwoch galezi drzewa
 */
struct compare
{
    bool operator()(TNODE *l, TNODE *r)
    {
        return (l->frequency > r->frequency);
    }
};

void printCodes(struct TNODE *root, std::string str);

void storeCodes(struct TNODE *root, std::string str);

void HuffmanCodes(int size);

void calcFreq(std::string str, int n);

std::string decode_file(struct TNODE *root, std::string s);

std::string codeFile(std::vector<char> input);

std::map<char, std::string> getDict();

// TESTING FUNCTIONS
int huffmanGFG();

#endif // ZAD3_HUFFMAN_H