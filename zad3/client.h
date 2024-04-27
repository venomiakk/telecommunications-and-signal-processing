#ifndef ZAD3_CLIENT_H
#define ZAD3_CLIENT_H

#include <winsock2.h>

int sendToSocket(SOCKET ConnectSocket, char *fileBuffer, int dataSize);

SOCKET initializeClient();

#endif // ZAD3_CLIENT_H