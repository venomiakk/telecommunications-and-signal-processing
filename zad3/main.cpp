#include "fileIO.h"
#include "server.h"
#include "client.h"
#include "huffman.h"
#include <iostream>
#include <stdio.h>
#include <winsock2.h>
#include <thread>

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
}

int main()
{
    thread tServer(server);
    thread tCient(client);
}
