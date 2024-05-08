#include "huffSender.h"
#include "fileIO.h"
#include "client.h"
#include <vector>
#include "huffman.h"
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;

void sendHuffmanFile(const char *ipaddr)
{
    const unsigned char DIVIDER = 0x3;
    vector<char> file = readFile("input.txt");
    string encodedStr = codeFile(file);
    map<char, string> codes = getDict();

    int dataSize = encodedStr.size();
    vector<char> saveHelper;
    for (int i = 0; i < dataSize; i++)
    {
        saveHelper.push_back(encodedStr[i]);
    }

    saveFile("inputEncoded.txt", saveHelper);
    saveBitsToFile(encodedStr, "inputEncodedBin.txt");

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
    char fileBuff[dataSize + dictSize + 1];
    int current = 0;
    for (int i = 0; i < dataSize; i++)
    {
        fileBuff[i] = encodedStr[i];
        current++;
    }
    fileBuff[current] = DIVIDER;
    for (int i = 0; i < dictSize; i++)
    {
        fileBuff[i + current+1] = dictFile.at(i);
    }

    cout << "Sent:" << endl;
    for (int i = 0; i < dataSize + dictSize + 1; i++)
    {
        cout << fileBuff[i];
    }
    cout << endl;
    cout << "Sent konice" << endl;

    SOCKET connSock = initializeClient(ipaddr);
    sendToSocket(connSock, fileBuff, dataSize + dictSize + 1);
}
