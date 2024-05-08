#include "huffSender.h"
#include "fileIO.h"
#include "client.h"
#include <vector>
#include "huffman.h"
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

void sendHuffmanFile()
{
    /* Wysylanie pliku */
    vector<char> file = readFile("input.txt");
    string encodedStr = codeFile(file);
    map<char, string> codes = getDict();
    int dataSize = encodedStr.size();
    char fileBuf[dataSize];
    vector<char> saveHelper;

    for (int i = 0; i < dataSize; i++)
    {
        fileBuf[i] = encodedStr[i];
        saveHelper.push_back(encodedStr[i]);
    }

    // for (int i = 0; i < dataSize; i++)
    // {
    //     cout << fileBuf[i];
    // }

    SOCKET connSock1 = initializeClient();
    sendToSocket(connSock1, fileBuf, dataSize);

    saveFile("inputEncoded.txt", saveHelper);
    saveBitsToFile(encodedStr, "inputEncodedBin.txt");

    /* Wysylanie slownika */
    vector<HuffmanDictElement> dict;

    for (auto v = codes.begin(); v != codes.end(); v++)
    {
        HuffmanDictElement tmp;
        tmp.key = v->first;
        tmp.value = v->second;
        dict.push_back(tmp);
    }

    std::sort(dict.begin(), dict.end(), [](const HuffmanDictElement &a, const HuffmanDictElement &b)
              { return a.value.size() < b.value.size(); });

    saveDictToFile(dict, "inputDict.txt");

    vector<char> dictFile = readFile("inputDict.txt");
    int dictSize = dictFile.size();
    char dictBuf[dictSize];
    for (int i = 0; i < dictSize; i++)
    {
        dictBuf[i] = dictFile.at(i);
    }

    SOCKET connSock2 = initializeClient();
    sendToSocket(connSock2, dictBuf, dictSize);
}
