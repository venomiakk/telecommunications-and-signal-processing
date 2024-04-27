#ifndef ZAD3_SERVER_H
#define ZAD3_SERVER_H
#include <winsock2.h>

SOCKET initializeServer();

SOCKET waitForConnection(SOCKET ListenSocket);

std::vector<char> receiveFromSocket(SOCKET ClientSocket);

#endif // ZAD3_SERVER_H