#include "huffReceiver.h"
#include "server.h"
#include "fileIO.h"
#include <vector>
#include <iostream>

using namespace std;

void receiveHuffmanFile()
{
    const unsigned char DIVIDER = 0x3;
    SOCKET ListenSocket = initializeServer();
    SOCKET ClientSocket = waitForConnection(ListenSocket);
    vector<char> file = receiveFromSocket(ClientSocket);
    int fileSize = file.size();
    vector<char> data;
    vector<char> dictFile;
    cout << "Received" << endl;
    int stop = 0;
    for (int i = 0; i < fileSize; i++)
    {
        if (file.at(i) == DIVIDER)
        {
            file.erase(file.begin(), file.begin() + i + 1);
            break;
        }
        cout << file.at(i);
        data.push_back(file.at(i));
        stop++;
    }
    for (auto c : file)
    {
        cout << c;
        dictFile.push_back(c);
    }
    cout << endl;
    cout << "Received koniec" << endl;

    saveFile("encoded.txt", data);
    string encoded;
    for (auto c : data)
    {
        encoded += c;
    }
    saveBitsToFile(encoded, "encodedBin.txt");
    saveFile("dict.txt", dictFile);

    vector<HuffmanDictElement> dict = readDictFromFile("dict.txt");
    string decoded;
    size_t pos = 0;
    while (pos < encoded.size())
    {
        for (const auto &elem : dict)
        {
            if (encoded.substr(pos, elem.value.size()) == elem.value)
            {
                decoded += elem.key;
                pos += elem.value.size();
                break;
            }
        }
    }
    vector<char> decodedV;
    for (auto c : decoded)
    {
        decodedV.push_back(c);
    }

    saveFile("decodedOutput.txt", decodedV);
}
