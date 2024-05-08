#include "huffReceiver.h"
#include "server.h"
#include "fileIO.h"
#include <vector>

using namespace std;

void receiveHuffmanFile()
{
    SOCKET ListenSocket = initializeServer();

    SOCKET ClientSocket1 = waitForConnection(ListenSocket);
    vector<char> file = receiveFromSocket(ClientSocket1);
    saveFile("encoded.txt", file);
    string encoded;
    for (auto c : file)
    {
        encoded += c;
    }
    saveBitsToFile(encoded, "encodedBin.txt");

    SOCKET ClientSocket2 = waitForConnection(ListenSocket);
    vector<char> dictFile = receiveFromSocket(ClientSocket2);
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
