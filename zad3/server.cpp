#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <vector>
#include <fstream>

using namespace std;

/*
 *   This only works for Visual C++ compiler
 *   #pragma comment(lib, "Ws2_32.lib")
 *
 *   Add to makefile '-L"ws2_32"'
 *   Or add command-line argument '-lwsock32'
 */

#define DEFAULT_PORT "27015"
#define IP_ADDR "192.168.8.109"
#define DEFAULT_BUFLEN 512

WSADATA wsaDataServer;

SOCKET initializeServer()
{
    int iResult;

    //* Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaDataServer);
    if (iResult != 0)
    {
        printf("SERVER | WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //* Resolve the local address and port to be used by the server
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("SERVER | getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ListenSocket = INVALID_SOCKET;

    //* Create a SOCKET for the server to listen for client connections
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ListenSocket == INVALID_SOCKET)
    {
        printf("SERVER | Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    //* Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("SERVER | bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Once the bind function is called,
    // the address information returned by the getaddrinfo function is no longer needed.
    freeaddrinfo(result);
    return ListenSocket;
}

SOCKET waitForConnection(SOCKET ListenSocket)
{
    //* Listen on a socket
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        printf("SERVER | Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    //* Accepting a connection
    SOCKET ClientSocket;
    ClientSocket = INVALID_SOCKET;

    // Accept a client socket
    cout << "SERVER | waiting for connection...\n";
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET)
    {
        printf("SERVER | accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    return ClientSocket;
}

vector<char> receiveFromSocket(SOCKET ClientSocket)
{

    //* Receiving data
    char recvbuf[DEFAULT_BUFLEN];
    int iResult, iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;
    vector<char> fileBuffer;

    // Receive until the peer shuts down the connection
    do
    {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            printf("SERVER | Bytes received: %d\n", iResult);

            for (size_t i = 0; i < iResult; i++)
            {
                fileBuffer.push_back(recvbuf[i]);
            }

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR)
            {
                printf("SERVER | send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return {};
            }
            printf("SERVER | Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
        {
            printf("SERVER | Connection closing...\n");
            closesocket(ClientSocket);
            WSACleanup();
        }
        else
        {
            printf("SERVER | recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return {};
        }

    } while (iResult > 0);

    cout << "SERVER | End of transmission\n";
    return fileBuffer;
}
