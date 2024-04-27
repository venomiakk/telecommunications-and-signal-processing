#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <queue>
#include "huffman.h"

#include "fileIO.h"

using namespace std;

//* HUFFMAN CODING

#define MAX_TREE_HT 256;

// to map each character its huffman value
map<char, string> codes;

// To store the frequency of character of the input data
map<char, int> freq;

// utility function to print characters along with
// there huffman value
void printCodes(struct TNODE *root, string str)
{
    if (!root)
        return;
    if (root->character != '$')
        cout << root->character << ": " << str << "\n";
    printCodes(root->left, str + "0");
    printCodes(root->right, str + "1");
}

// utility function to store characters along with
// there huffman value in a hash table, here we
// have C++ STL map
void storeCodes(struct TNODE *root, string str)
{
    if (root == NULL)
        return;
    if (root->character != '$')
        codes[root->character] = str;
    storeCodes(root->left, str + "0");
    storeCodes(root->right, str + "1");
}

priority_queue<TNODE *, vector<TNODE *>, compare> minHeap;

// function to build the Huffman tree and store it
// in minHeap
void HuffmanCodes(int size)
{
    struct TNODE *left, *right, *top;
    for (map<char, int>::iterator v = freq.begin();
         v != freq.end(); v++)
        minHeap.push(new TNODE(v->first, v->second));
    while (minHeap.size() != 1)
    {
        left = minHeap.top();
        minHeap.pop();
        right = minHeap.top();
        minHeap.pop();
        top = new TNODE('$',
                        left->frequency + right->frequency);
        top->left = left;
        top->right = right;
        minHeap.push(top);
    }
    storeCodes(minHeap.top(), "");
}

// utility function to store map each character with its
// frequency in input string
void calcFreq(string str, int n)
{
    for (int i = 0; i < str.size(); i++)
        freq[str[i]]++;
}

// function iterates through the encoded string s
// if s[i]=='1' then move to node->right
// if s[i]=='0' then move to node->left
// if leaf node append the node->data to our output string
string decode_file(struct TNODE *root, string s)
{
    string ans = "";
    struct TNODE *curr = root;
    for (int i = 0; i < s.size(); i++)
    {
        if (s[i] == '0')
            curr = curr->left;
        else
            curr = curr->right;

        // reached leaf node
        if (curr->left == NULL and curr->right == NULL)
        {
            ans += curr->character;
            curr = root;
        }
    }
    // cout<<ans<<endl;
    return ans + '\0';
}

int huffmanGFG()
{
    vector<char> file = readFile("input.txt");
    string str = "";
    for (auto c : file)
    {
        str += c;
    }
    cout << str + '\n';
    string encodedString, decodedString;
    calcFreq(str, str.length());
    HuffmanCodes(str.length());
    cout << "Character With there Frequencies:\n";
    for (auto v = codes.begin(); v != codes.end(); v++)
        cout << v->first << ' ' << v->second << endl;

    for (auto i : str)
        encodedString += codes[i];

    cout << "\nEncoded Huffman data:\n"
         << encodedString << endl;
    saveBitsToFile(encodedString, "bits.txt");
    // Function call
    encodedString = readBitsFromFile("bits.txt");
    decodedString = decode_file(minHeap.top(), encodedString);
    cout << "\nDecoded Huffman Data:\n"
         << decodedString << endl;
    return 0;
}
