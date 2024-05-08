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

WSADATA wsaDataClient;

SOCKET initializeClient(const char *ipaddr)
{
    int iResult;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaDataClient);
    if (iResult != 0)
    {
        printf("CLIENT | WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ipaddr, DEFAULT_PORT, &hints, &result);
    if (iResult != 0)
    {
        printf("CLIENT | getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET ConnectSocket = INVALID_SOCKET;

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                           ptr->ai_protocol);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("CLIENT | Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET)
    {
        printf("CLIENT | Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    return ConnectSocket;
}

int sendToSocket(SOCKET ConnectSocket, char *fileBuffer, int dataSize)
{

    //* Sending data

    int recvbuflen = dataSize;

    const char *sendbuf = fileBuffer;
    char recvbuf[dataSize];

    int iResult;

    // Send an initial buffer
    iResult = send(ConnectSocket, sendbuf, dataSize, 0);
    if (iResult == SOCKET_ERROR)
    {
        printf("CLIENT | send failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    printf("CLIENT | Bytes Sent: %ld\n", iResult);

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR)
    {
        printf("CLIENT | shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive data until the server closes the connection
    do
    {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("CLIENT | Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("CLIENT | Connection closed\n");
        else
            printf("CLIENT | recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    cout << "CLIENT | End of transmission\n";
    return 0;
}
