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

int main()
{
    cout << "Adrian Michalek, Hanna Mikolajczyk, Maksymilian Paluskiewicz" << endl;
    cout << "------------------------------------------------------------" << endl;
    int mode = 0;
    cout << "1. Odbieranie (Serwer)\n2. Wysylanie (Klient)\n";
    cin >> mode;
    if (mode == 1)
    {
        cout << "Serwer:\n";
        receiveHuffmanFile();
    }
    else
    {
        string temp;
        cout << "Klient:\n";
        cout << "Adres serwera: ";
        cin >> temp;
        const char *ipaddr = temp.c_str();
        sendHuffmanFile(ipaddr);
    }

    return 0;
}