#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>

using namespace std;

const unsigned char SOH = 0x1;
const unsigned char EOT = 0x4;
const unsigned char ACK = 0x6;
const unsigned char NAK = 0x15;
const unsigned char CAN = 0x18;
const unsigned char C = 0x43;
const unsigned char SUB = 0x1a;

const LPCSTR COMPORT7 = "COM7";
const LPCSTR COMPORT8 = "COM8";
const string IN_FILE_PATH = "input";
const string OUT_FILE_PATH = "output";

bool CRC16 = true;
int PACKET_SIZE = 133;

HANDLE hCOM;
bool IS_PORT_READY = false;
DCB dcbCONTROL;
int BAUD_RATE = 9600;

uint16_t calcCRC16(unsigned char *data, int length)
{
    uint16_t crc = 0x0000;

    for (int i = 0; i < length; i++)
    {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
        {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc = crc << 1;
        }
    }

    return crc;
}

unsigned char calcChecksum(unsigned char *data)
{
    unsigned char sum = 0;
    int intsum = 0;
    for (int i = 0; i < 128; i++)
    {
        sum += data[i];
    }
    return sum;
}

/*!
 * @brief Funkcja zapisujaca bufor jako plik
 * @param filePath
 * @param fileBuffer
 */
int saveFile(string filePath, vector<unsigned char> fileBuffer)
{
    //  Otwieramy plik w trybie binarnym
    ofstream outputFile(filePath, ios::binary);
    if (!outputFile.is_open())
    {
        cerr << "Nie udalo sie otworzyc pliku.\n";
        return 1;
    }
    //  Zapisujemy zawartosc bufora jako plik
    outputFile.write(reinterpret_cast<const char *>(fileBuffer.data()), fileBuffer.size());
    //  Zamykamy plik
    outputFile.close();

    return 0;
}

int initializeComPort(LPCSTR port)
{

    hCOM = CreateFileA(
        port,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hCOM == INVALID_HANDLE_VALUE)
    {
        cerr << "Nie udalo sie otworzyc portu " << port << "\n";
        return 1;
    }
    else
    {
        cout << "Port " << port << " jest otwarty\n";
    }

    // Czyszczenie bufora
    PurgeComm(hCOM, PURGE_TXCLEAR | PURGE_RXCLEAR);

    //  to chyba nic nie robi
    // isPortReady = SetupComm(hCOM, 1, 128); // USTAWIANIE WIELKOSCI BUFORA
    IS_PORT_READY = GetCommState(hCOM, &dcbCONTROL);
    dcbCONTROL.BaudRate = BAUD_RATE;
    dcbCONTROL.ByteSize = 8;
    dcbCONTROL.Parity = NOPARITY; // EVENPARITY;
    dcbCONTROL.StopBits = ONESTOPBIT;
    dcbCONTROL.fAbortOnError = TRUE;
    dcbCONTROL.fOutX = FALSE;        // XON/XOFF WYLACZANIE DO TRANSMISJI
    dcbCONTROL.fInX = FALSE;         // XON/XOFF WYLACZANIE DO ODBIERANIA
    dcbCONTROL.fOutxCtsFlow = FALSE; // WLACZANIE CTS flow control
    dcbCONTROL.fRtsControl = RTS_CONTROL_HANDSHAKE;
    dcbCONTROL.fOutxDsrFlow = FALSE; // WLACZENIE DSR FLOW CONTROL
    dcbCONTROL.fDtrControl = DTR_CONTROL_ENABLE;
    dcbCONTROL.fDtrControl = DTR_CONTROL_DISABLE;
    dcbCONTROL.fDtrControl = DTR_CONTROL_HANDSHAKE;
    IS_PORT_READY = SetCommState(hCOM, &dcbCONTROL);

    return 0;
}

void sendToComPort(unsigned char *sendBuffer, int packetLength)
{
    // TODO
    // TODO: czy czyscic bufor transmisji przed wyslaniem?
    DWORD dwBytesWritten;
    if (!WriteFile(hCOM, sendBuffer, packetLength, &dwBytesWritten, NULL))
    {
        cerr << "Nie udalo sie wyslac pakietu!" << endl;
        return;
    }
}

void readFromComPort(unsigned char *receiveBuffer, int expectedSize)
{
    // TODO: czy czyscic bufor odbioru przed czytaniem?

    DWORD dwBytesRead;
    DWORD errors;
    COMSTAT status;
    ReadFile(hCOM, receiveBuffer, expectedSize, &dwBytesRead, NULL);
}

void receiveData()
{
    unsigned char signBuffer[1] = {CRC16 ? C : NAK};
    sendToComPort(signBuffer, 1);

    vector<unsigned char> fileBuffer;
    while (true)
    {
        readFromComPort(signBuffer, 1);
        if (EOT == signBuffer[0])
        {
            signBuffer[0] = ACK;
            sendToComPort(signBuffer, 1);
            cout << "Transmisja zakonczona, " << static_cast<int>(signBuffer[0]) << endl;
            break;
        }
        unsigned char receiveBuffer[PACKET_SIZE - 1] = {NULL};
        unsigned char fileData[128] = {NULL};
        readFromComPort(receiveBuffer, PACKET_SIZE - 1);
        for (int i = 0; i < 128; i++)
        {
            fileData[i] = receiveBuffer[i + 2];
        }

        signBuffer[0] = NAK;
        if (CRC16)
        {
            uint16_t crc = ((uint16_t)receiveBuffer[130] << 8) | receiveBuffer[131];
            if (calcCRC16(fileData, 128) == crc)
            {
                signBuffer[0] = ACK;
            }
        }
        else
        {
            unsigned char sum = receiveBuffer[130];
            if (calcChecksum(fileData) == sum)
            {
                signBuffer[0] = ACK;
            }
        }

        if (signBuffer[0] == ACK)
        {
            for (int i = 0; i < 128; i++)
            {
                fileBuffer.push_back(fileData[i]);
            }
        }
        cout << "replay: " << static_cast<int>(signBuffer[0]) << endl;
        sendToComPort(signBuffer, 1);
    }

    for (int i = fileBuffer.size() - 1; i >= 0; i--)
    {
        if (SUB == fileBuffer.at(i))
        {
            fileBuffer.pop_back();
        }
        else
        {
            break;
        }
    }
    saveFile(OUT_FILE_PATH, fileBuffer);

    // readFromComPort(receiveBuffer, PACKET_SIZE);
}

int main()
{
    cout << "Wybierz CRC16 lub Checksum:\n1. CRC16\n2. Checksum " << endl;
    int choice;
    cin >> choice;
    if (choice == 2)
    {
        cout << "Wybrano: Checksum" << endl;
        CRC16 = false;
        PACKET_SIZE = 132;
    }
    else
    {
        cout << "Wybrano: CRC16" << endl;
    }

    if (initializeComPort(COMPORT8))
    {
        return 1;
    }

    receiveData();
    // wait 1 or 3 seconds for packet, send another C, repeat for 1 minute
    return 0;
}