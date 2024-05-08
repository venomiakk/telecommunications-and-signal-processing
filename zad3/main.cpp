#include "fileIO.h"
#include "server.h"
#include "client.h"
#include "huffman.h"
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <thread>
#include <cstring>
#include <algorithm>
#include "typedefs.h"
#include "huffSender.h"
#include "huffReceiver.h"

using namespace std;

void server()
{
    vector<char> file;
    SOCKET ListenSocket = initializeServer();
    SOCKET ClientSocket = waitForConnection(ListenSocket);
    file = receiveFromSocket(ClientSocket);
    for (auto c : file)
    {
        cout << c;
    }
    cout << "\n";

    cout << "Server - 2nd send" << endl;
    SOCKET ClientSocket2 = waitForConnection(ListenSocket);
    file = receiveFromSocket(ClientSocket2);
    for (auto c : file)
    {
        cout << c;
    }
    cout << "\n";
}

void client()
{
    vector<char> fileV = readFile("input.txt");
    int dataSize = fileV.size();
    char fileBuf[dataSize];

    for (int i = 0; i < dataSize; i++)
    {
        fileBuf[i] = fileV.at(i);
    }

    SOCKET connSock = initializeClient();
    sendToSocket(connSock, fileBuf, dataSize);

    for (int i = 0; i < dataSize; i++)
    {
        fileBuf[i] = fileV.at(i);
    }
    cout << "CLIENT - 2nd send" << endl;
    SOCKET connSock2 = initializeClient();
    sendToSocket(connSock2, fileBuf, dataSize);
}

void testfunc()
{
    // thread tServer(server);
    // thread tCient(client);

    // huffmanGFG();

    //? dlaczego zwracany string sie różni i ma znak é???
    // TODO: konwersja na boole/inty w huffmanie?
    vector<char> file = readFile("input.txt");
    string encodedStr = codeFile(file);
    map<char, string> codes = getDict();

    // cout << "main: " << endl;
    // cout << encodedStr;
    // for (auto v = codes.begin(); v != codes.end(); v++)
    // {
    //     cout << v->first << ' ' << v->second << endl;
    // }

    const char *str1 = encodedStr.c_str();
    char str2[] = "0110111011110011010110111001111100101111011011110100000011000000011";
    cout << strcmp(str1, str2) << endl;

    vector<char> tmp;
    for (auto c : encodedStr)
    {
        tmp.push_back(c);
    }

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

    // for (auto ele : dict)
    // {
    //     cout << ele.key << " : " << ele.value << endl;
    // }

    saveDictToFile(dict, "dict.txt");
    cout << "odczyt pliku" << endl;
    vector<HuffmanDictElement> readDict = readDictFromFile("dict.txt");
    int dasd = 0;
    for (auto elem : readDict)
    {

        cout << ++dasd << "| " << elem.key << ":" << elem.value << endl;
    }

    cout << encodedStr << endl;

    std::string decodedStr;
    size_t pos = 0;
    while (pos < encodedStr.size())
    {
        for (const auto &elem : readDict)
        {
            if (encodedStr.substr(pos, elem.value.size()) == elem.value)
            {
                decodedStr += elem.key;
                pos += elem.value.size();
                break;
            }
        }
    }
    cout << decodedStr << endl;
    // saveFile("output.txt", decodedStr);
    vector<char> test;
    for (auto c : decodedStr)
    {
        test.push_back(c);
    }
    saveFile("output.txt", test);
}

int main()
{
    // thread tServer(receiveHuffmanFile);
    // thread tClient(sendHuffmanFile);
    int mode = 0;
    cout << "1. Odbieranie (Serwer)\n.2 Wysylanie (Klient)\n";
    cin >> mode;
    if (mode == 1)
    {
        receiveHuffmanFile();
    }
    else
    {
        sendHuffmanFile();
    }

    return 0;
}