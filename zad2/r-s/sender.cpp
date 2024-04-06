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
 * @brief Funkcja wczytujaca plik
 * @param filePath - siciezka do pliku
 * @return bajty pliku jako vector<char>
 */
vector<unsigned char> readFile(string filePath)
{
    //  Otwarcie pliku w trybie binarnym oraz ustawienie kursora na końcu pliku w celu odczytania rozmiaru
    ifstream inputFile(filePath, ios::binary | ios::ate);
    if (!inputFile.is_open())
    {
        cerr << "Nie mozna otworzyc pliku." << endl;
        return {};
    }

    //  size przetrzymuje pozycje ostatniego bajta tj. zapisujemy rozmiar pliku
    streamsize size = inputFile.tellg();
    vector<unsigned char> fileBuffer(size);
    //  ustawiamy kursor na pierwszy bajt pliku
    inputFile.seekg(0, ios::beg);

    if (inputFile.read(reinterpret_cast<char *>(fileBuffer.data()), size))
    {
        /* Plik został pomyślnie zapisany do bufora */
    }
    else
    {
        cerr << "Nie mozna odczytac pliku.\n";
    }
    // Zamknięcie pliku
    inputFile.close();

    return fileBuffer;
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

//* input z konsoli
void sending(LPCSTR comport, string inputData)
{
    DWORD dwBytesWritten;
    const char *data = inputData.c_str(); // Dane do wysłania

    if (!WriteFile(hCOM, data, strlen(data), &dwBytesWritten, NULL))
    {
        cerr << "Nie udalo sie wyslac danych\n";
        CloseHandle(hCOM);
        return;
    }
    // cout << dwBytesWritten << endl;
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

bool establishConnection()
{
    cout << "Nawiazywanie polacznenia..." << endl;
    unsigned char receiveBuffer[128] = {NULL};
    auto waitStart = chrono::high_resolution_clock::now();
    auto waitEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> wait = waitEnd - waitStart;
    bool isTransmissionEstablished = false;
    int i = 0;
    while (wait.count() < 60.0)
    {
        readFromComPort(receiveBuffer, 1);
        if (C == receiveBuffer[0]) // TODO: C as 0x43
        {
            isTransmissionEstablished = true;
            break;
        }
        // TODO transmission without CRC
        else if (NAK == receiveBuffer[0])
        {
            isTransmissionEstablished = true;
            CRC16 = false;
            PACKET_SIZE = 132;
            break;
        }

        waitEnd = chrono::high_resolution_clock::now();
        wait = waitEnd - waitStart;
    }

    if (!isTransmissionEstablished)
    {
        cerr << "Nie otrzymano odpowiedzi od odbiornika!" << endl;
    }

    return isTransmissionEstablished;
}

void startTransmission(vector<unsigned char> fileBuffer)
{
    int dataSize = fileBuffer.size();
    int numOfPackets = (dataSize / 128) + 1;
    int bufferIndex = 0;

    int remaining = dataSize % 128;
    if (remaining != 0)
    {
        int padding = 128 - remaining;
        for (int i = 0; i < padding; i++)
        {
            fileBuffer.push_back(SUB); // Dodaj bajty dopełniające
        }
    }

    unsigned char dataPackets[numOfPackets][128];
    for (int i = 0; i < numOfPackets; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            dataPackets[i][j] = fileBuffer.at(bufferIndex);
            bufferIndex++;
        }
    }

    int sentPackets = 0;
    while (sentPackets < numOfPackets)
    {
        unsigned char packetNo = sentPackets + 1;
        if (0 == packetNo)
        {
            packetNo = 1;
        }

        unsigned char sendBuffer[PACKET_SIZE] = {NULL};
        // sendBuffer[0] = CRC16 ? C : SOH;
        sendBuffer[0] = SOH;
        sendBuffer[1] = packetNo;
        sendBuffer[2] = 255 - packetNo;
        for (int i = 3; i < 128 + 3; i++)
        {
            sendBuffer[i] = dataPackets[sentPackets][i - 3];
        }
        if (CRC16)
        {
            uint16_t crc = calcCRC16(dataPackets[sentPackets], 128);
            unsigned char crcBytes[2];
            sendBuffer[131] = (crc >> 8) & 0xFF; // Wyodrębnij starszy bajt
            sendBuffer[132] = crc & 0xFF;        // Wyodrębnij młodszy bajt
            // ? moze zamiast takich fikolkow
            // ? to poprostu wyslac tego uinta
            // ? po wyslaniu juz calego bufora
            // ? patrz linia 308
        }
        else
        {
            sendBuffer[131] = calcChecksum(dataPackets[sentPackets]);
        }

        sendToComPort(sendBuffer, PACKET_SIZE);
        // waitForConnection(sendBuffer, PACKET_SIZE);

        unsigned char replay[100] = {NULL};
        auto waitStart = chrono::high_resolution_clock::now();
        auto waitEnd = chrono::high_resolution_clock::now();
        chrono::duration<double> wait = waitEnd - waitStart;

        // TODO: obsluga bledow, jakis bool?
        bool isConnected = false;
        // while (wait.count() < 60.0)
        while (true)
        {
            readFromComPort(replay, 1);
            if (ACK == replay[0]) // TODO ACK
            {
                isConnected = true;
                cout << "replay: " << static_cast<int>(replay[0]) << endl;
                cout << "packet: " << static_cast<int>(packetNo) << endl;
                sentPackets++;
                break;
            }
            else if (NAK == replay[0]) // TODO NACK
            {
                isConnected = true;
                cout << "Ponowienie pakietu " << static_cast<int>(packetNo) << endl;
                break;
            }

            waitEnd = chrono::high_resolution_clock::now();
            wait = waitEnd - waitStart;
            //? sleep?
        }
        if (!isConnected)
        {
            cerr << "Connection interrupted. Closing..." << endl;
            return;
        }
    }
    //* Koniec transmisji
    unsigned char eotBuffer[1] = {EOT};
    sendToComPort(eotBuffer, 1);
    unsigned char replay[100] = {NULL};
    auto waitStart = chrono::high_resolution_clock::now();
    auto waitEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> wait = waitEnd - waitStart;

    // TODO: obsluga bledow, jakis bool?
    bool isConnected = false;
    while (wait.count() < 60.0)
    {
        readFromComPort(replay, 1);
        if (ACK == replay[0]) // TODO ACK
        {
            isConnected = true;
            break;
        }
        waitEnd = chrono::high_resolution_clock::now();
        wait = waitEnd - waitStart;
        //? sleep?
    }
    if (!isConnected)
    {
        cerr << "Connection interrupted. Closing..." << endl;
        return;
    }

    cout << "Transmisja zakonczona pomyslnie" << endl;
}

int main()
{
    vector<unsigned char> fileBuffer = readFile(IN_FILE_PATH);
    cout << "File size: " << fileBuffer.size() << endl;

    if (initializeComPort(COMPORT7))
    {
        return 1;
    };

    if (establishConnection())
    {
        cout << "Polaczenie nawiazane, przesylanie danych..." << endl;
        startTransmission(fileBuffer);
    }
    // startTransmission(fileBuffer);
    return 0;
}

/*
* note:
 te wszystkie timery nie maja sensu
 funkcja czytajaca dane z bufora portu jest blokujace
 wiec petla i tak nie przejdzie dalej
 dopoki cos nie zostanie odebrane z poru
TODO: usunac timery
TODO: rozwazyc zmiane na 1 program z mozliwosciami wyboru s/r, plik/tekst
TODO: mozna dodac wybieranie pliku wpisujac w terminalu nazwe
*/