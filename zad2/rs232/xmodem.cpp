#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <chrono>
#include <cstdint>

using namespace std;

// Bajty uzywane w pakietach protokolu Xmodem
const unsigned char SOH = 0x1;
const unsigned char EOT = 0x4;
const unsigned char ACK = 0x6;
const unsigned char NAK = 0x15;
const unsigned char CAN = 0x18;
const unsigned char C = 0x43;
const unsigned char SUB = 0x1a;

// const LPCSTR COMPORT7 = "COM7";
// const LPCSTR COMPORT8 = "COM8";
// const string IN_FILE_PATH = "input";
// const string OUT_FILE_PATH = "output";

// Zmienne globalne do wyboru sumy kontrolnej
bool CRC16 = true;
int PACKET_SIZE = 133;

// Zmienne globalne do kontrolowania portu
HANDLE hCOM;
bool IS_PORT_READY = false;
DCB dcbCONTROL;
int BAUD_RATE = 9600;

/*!
 * @brief Oblicza Cykliczny Kod Nadmiarowy CRC16
 * @param data tablica bajtow
 * @param length dlugosc tablicy
 * @return 16-sto bitowa wartosc CRC16
 */
uint16_t calcCRC16(unsigned char *data, int length)
{
    uint16_t crc = 0x0000;

    for (int i = 0; i < length; i++)
    {
        // Przesuniecie o 8 bitow i wykonuj XOR z obecna wartoscia CRC
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++)
        {
            // Sprawdzenie czy najbardziej znaczacy bit jest ustawiony
            if (crc & 0x8000)
                // Przesuniecie o 1 bit w lewo (mnozenie przez 2) oraz XOR z 0x1021
                crc = (crc << 1) ^ 0x1021;
            // Jezeli najbardziej znaczacy bit nie jest usawiony
            else
                // Przesuniecie o 1 bit w lewo (mnozenie przez 2)
                crc = crc << 1;
        }
    }

    return crc;
}

/*!
 * @brief Obliczanie sumy z bajtow
 * @param data tablica bajtow
 * @return suma kontrolna
 */
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
 * @param filePath sciezka do pliku
 * @param fileBuffer bufor pliku
 */
int saveFile(string filePath, vector<unsigned char> fileBuffer)
{
    //  Otwarcie pliku w trybie binarnym
    ofstream outputFile(filePath, ios::binary);
    if (!outputFile.is_open())
    {
        cerr << "X | Nie udalo sie otworzyc pliku.\n";
        return 1;
    }
    //  Zapisanie zawartosci bufora jako plik
    outputFile.write(reinterpret_cast<const char *>(fileBuffer.data()), fileBuffer.size());
    outputFile.close();

    return 0;
}

/*!
 * @brief Wczytywanie pliku
 * @param filePath sciezka do pliku
 * @return vector bajtow pliku
 */
vector<unsigned char> readFile(string filePath)
{
    //  Otwarcie pliku w trybie binarnym oraz ustawienie kursora na końcu pliku w celu odczytania rozmiaru
    ifstream inputFile(filePath, ios::binary | ios::ate);
    if (!inputFile.is_open())
    {
        cerr << "X | Nie udalo sie otworzyc pliku." << endl;
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

    inputFile.close();

    return fileBuffer;
}

/*!
 * @brief Inicjalizacja portu szeregoweogo COM
 * @param port numer portu
 * @return {0/1} czy port zostal zainicjalizowany
 */
int initializeComPort(LPCSTR port)
{

    // Tworzenie uchwytu do portu COM
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
        cerr << "X | Nie udalo sie otworzyc portu " << port << "\n";
        return 1;
    }
    else
    {
        cout << "Port " << port << " jest otwarty\n";
    }

    // Czyszczenie bufora
    PurgeComm(hCOM, PURGE_TXCLEAR | PURGE_RXCLEAR);

    IS_PORT_READY = GetCommState(hCOM, &dcbCONTROL);
    dcbCONTROL.BaudRate = BAUD_RATE;  // Szybkosc transmisji
    dcbCONTROL.ByteSize = 8;          // rozmiar bajta
    dcbCONTROL.Parity = NOPARITY;     // EVENPARITY;
    dcbCONTROL.StopBits = ONESTOPBIT; // bity stopu
    dcbCONTROL.fAbortOnError = TRUE;
    dcbCONTROL.fOutX = FALSE;        // XON/XOFF wylaczenie do transmisji
    dcbCONTROL.fInX = FALSE;         // XON/XOFF wylaczenie do odbierania
    dcbCONTROL.fOutxCtsFlow = FALSE; // w CTS flow control
    dcbCONTROL.fRtsControl = RTS_CONTROL_HANDSHAKE;
    dcbCONTROL.fOutxDsrFlow = FALSE; // wlaczenie DSR FLOW CONTROL
    dcbCONTROL.fDtrControl = DTR_CONTROL_ENABLE;
    dcbCONTROL.fDtrControl = DTR_CONTROL_DISABLE;
    dcbCONTROL.fDtrControl = DTR_CONTROL_HANDSHAKE;
    IS_PORT_READY = SetCommState(hCOM, &dcbCONTROL);

    return 0;
}

/*!
 * @brief wyslanie bajtow na port szeregowy
 * @param sendBuffer tablica bajtow do wyslania
 * @param packetLength dlugosc tablicy
 */
void sendToComPort(unsigned char *sendBuffer, int packetLength)
{
    DWORD dwBytesWritten;
    if (!WriteFile(hCOM, sendBuffer, packetLength, &dwBytesWritten, NULL))
    {
        cerr << "   X | Nie udalo sie wyslac pakietu!" << endl;
        return;
    }
}

/*!
 * @brief odczytanie bajtow z portu szeregowego
 * @param receiveBuffer bufor prztrzymujacy odebrane bajty
 * @param expectedSize ilosc odbieranych bajtow
 */
void readFromComPort(unsigned char *receiveBuffer, int expectedSize)
{
    DWORD dwBytesRead;
    ReadFile(hCOM, receiveBuffer, expectedSize, &dwBytesRead, NULL);
}

// FUNKCJE ODBIORNIKA

/*!
 * @brief odbieranie pliku z portu COM, protokol Xmodem
 * @param outputPath miejsce zapisania odebranego pliku
 */
void receiveFile(string outputPath)
{
    // Wyslanie bajtu inicjalizujacego transmisje z CRC16 lub suma kontrolna
    unsigned char signBuffer[1] = {CRC16 ? C : NAK};
    sendToComPort(signBuffer, 1);

    int receivedPacket = 1;
    vector<unsigned char> fileBuffer;
    while (true)
    {
        // Odczytywanie naglowka pakietu
        readFromComPort(signBuffer, 1);
        // Naglowek EOT, zakonczenie transmisji
        if (EOT == signBuffer[0])
        {
            // Wyslanie pakietu ACK potwierdzajacego zakonczenie transmisji
            signBuffer[0] = ACK;
            sendToComPort(signBuffer, 1);
            cout << "Transmisja zakonczona, odebrano znak EOT" << endl;
            break;
        }

        // Odbieranie pojedynczego pakietu
        unsigned char receiveBuffer[PACKET_SIZE - 1] = {NULL};
        unsigned char fileData[128] = {NULL};
        readFromComPort(receiveBuffer, PACKET_SIZE - 1);
        for (int i = 0; i < 128; i++)
        {
            fileData[i] = receiveBuffer[i + 2];
        }

        // Sprawdzenie poprawnosci danych poprzez obliczenie CRC16 lub sumy kontrolnej
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

        // Gdy dane sa poprawne, zapisujemy do bufora
        if (signBuffer[0] == ACK)
        {
            for (int i = 0; i < 128; i++)
            {
                fileBuffer.push_back(fileData[i]);
            }
        }

        cout << "   pakiet: " << receivedPacket++ << endl;
        cout << "   odpowiedz: " << (static_cast<int>(signBuffer[0]) == 6 ? "ACK" : "NACK") << endl
             << endl;

        // Wyslanie pakietu ACK/NACK informujacego o poprawnosci danych
        sendToComPort(signBuffer, 1);
    }

    // Usuniecie dopisanych bajtow dopelniajacych ostatni pakiet
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
    saveFile(outputPath, fileBuffer);
}

// FUNKCJE NADAJNIKA

/*!
 * @brief Nawiazywanie polaczenia miedzy odbiorca a nadawca
 * @return {true/false} czy polaczenie zostalo nawiazane
 */
bool establishConnection()
{
    cout << "   Nawiazywanie polacznenia..." << endl;
    unsigned char receiveBuffer[128] = {NULL};

    auto waitStart = chrono::high_resolution_clock::now();
    auto waitEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> wait = waitEnd - waitStart;

    bool isTransmissionEstablished = false;
    while (wait.count() < 60.0)
    {
        // Odczytywanie danych z portu
        // Ustawianie transmisji na CRC16 lub sume kontrolna
        readFromComPort(receiveBuffer, 1);
        if (C == receiveBuffer[0])
        {
            isTransmissionEstablished = true;
            break;
        }
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
        cerr << "   X | Nie otrzymano odpowiedzi od odbiornika!" << endl;
    }

    return isTransmissionEstablished;
}

/*!
 * @brief wysylanie pliku na port COM, protokol Xmodem
 * @param fileBuffer bajty pliku
 */
void sendFile(vector<unsigned char> fileBuffer)
{
    // Obliczanie ilosci pakietow
    int dataSize = fileBuffer.size();
    int numOfPackets = (dataSize / 128) + 1;
    int bufferIndex = 0;

    // Obliczanie ilosci bajtow dopelnienia pakietu
    // Dodanie bajtow dopelniajacych do bufora
    int remaining = dataSize % 128;
    if (remaining != 0)
    {
        int padding = 128 - remaining;
        for (int i = 0; i < padding; i++)
        {
            fileBuffer.push_back(SUB);
        }
    }

    // Dzielenie pliku na pakiety po 128 bajtow
    unsigned char dataPackets[numOfPackets][128];
    for (int i = 0; i < numOfPackets; i++)
    {
        for (int j = 0; j < 128; j++)
        {
            dataPackets[i][j] = fileBuffer.at(bufferIndex);
            bufferIndex++;
        }
    }

    // Wysylanie pakietow
    int sentPackets = 0;
    while (sentPackets < numOfPackets)
    {
        // Obliczanie numeru pakietu
        unsigned char packetNo = (sentPackets + 1) % 256;
        unsigned char sendBuffer[PACKET_SIZE] = {NULL};

        // Ustawienie pierwszych 3 bajtow pakietu
        sendBuffer[0] = SOH;
        sendBuffer[1] = packetNo;
        sendBuffer[2] = 255 - packetNo;

        // Ustawienie bajtow danych pakietu
        for (int i = 3; i < 128 + 3; i++)
        {
            sendBuffer[i] = dataPackets[sentPackets][i - 3];
        }

        // Obliczanie i dodawanie CRC lub sumy kontrolnej do pakietu
        if (CRC16)
        {
            uint16_t crc = calcCRC16(dataPackets[sentPackets], 128);
            unsigned char crcBytes[2];
            sendBuffer[131] = (crc >> 8) & 0xFF; // Wyodrębnienie starszego bajtu
            sendBuffer[132] = crc & 0xFF;        // Wyodrebnienie mlodszego bajtu
        }
        else
        {
            sendBuffer[131] = calcChecksum(dataPackets[sentPackets]);
        }

        // Wyslanie pakietu
        sendToComPort(sendBuffer, PACKET_SIZE);

        auto waitStart = chrono::high_resolution_clock::now();
        auto waitEnd = chrono::high_resolution_clock::now();
        chrono::duration<double> wait = waitEnd - waitStart;

        unsigned char replay[100] = {NULL};
        bool isConnected = false;

        // Oczekiwanie na odpowiedz, czy pakiet doszedl poprawny
        while (wait.count() < 60.0)
        {
            readFromComPort(replay, 1);
            if (ACK == replay[0])
            {
                isConnected = true;
                cout << "   pakiet: " << static_cast<int>(sentPackets) + 1 << " / " << numOfPackets << endl;
                cout << "   odpowiedz: " << (static_cast<int>(replay[0]) == 6 ? "ACK" : "NACK") << endl
                     << endl;
                sentPackets++;
                break;
            }
            else if (NAK == replay[0])
            {
                isConnected = true;
                cout << "   Ponowienie pakietu " << static_cast<int>(sentPackets) << endl;
                break;
            }

            waitEnd = chrono::high_resolution_clock::now();
            wait = waitEnd - waitStart;
        }

        // Sprawdzenie czy nie doszlo do zerwania polaczenia
        if (!isConnected)
        {
            cerr << "X | Polaczenie przerwane. Rozlaczanie..." << endl;
            return;
        }
    }

    // Koniec transmisji, wyslanie pakietu z bajtem EOT
    unsigned char eotBuffer[1] = {EOT};
    sendToComPort(eotBuffer, 1);

    auto waitStart = chrono::high_resolution_clock::now();
    auto waitEnd = chrono::high_resolution_clock::now();
    chrono::duration<double> wait = waitEnd - waitStart;

    unsigned char replay[100] = {NULL};
    bool isConnected = false;
    while (wait.count() < 60.0)
    {
        // Oczekiwanie na potwierdzenie zakonczenia transmisji
        readFromComPort(replay, 1);
        if (ACK == replay[0])
        {
            isConnected = true;
            break;
        }
        waitEnd = chrono::high_resolution_clock::now();
        wait = waitEnd - waitStart;
    }

    // Sprawdzenie czy nie doszlo do zerwania polaczenia
    if (!isConnected)
    {
        cerr << "   X | Polaczenie przerwane. Rozlaczanie..." << endl;
        return;
    }

    cout << "Transmisja zakonczona pomyslnie" << endl;
}

int main()
{
    // KOMUNIKACJA MUSI BYC ZAINICJALIZOWANA PRZEZ ODBIORCE
    // DLATEGO NALEZY NAJPIERW URUCHOMIC NADAWCE
    // TAK SAMO Z HYPERTERMINALEM

    cout << "Adrian Michalek, Hanna Mikolajczyk, Maksymilian Paluskiewicz" << endl;
    cout << "IMPLEMENTACJA PROTOKOLU XMODEM" << endl
         << endl;

    string port;
    cout << "Numer portu COM: ";
    cin >> port;
    port = "COM" + port;
    if (initializeComPort(port.c_str()))
    {
        system("pause");
        return 1;
    }
    cout << endl;
    string title = "title " + port;
    system(title.c_str());
    cout << "--------------------------------" << endl;

    while (true)
    {
        cout << "1. Odbieranie\n2. Wysylanie\n: ";
        int mode;
        cin >> mode;
        cout << "--------------------------------" << endl;
        if (mode == 1)
        {
            cout << "  Sprawdzanie danych" << endl;
            cout << "  1. CRC16\n  2. Checksum" << endl;
            int checking;
            cout << "  : ";
            cin >> checking;
            if (checking == 2)
            {
                CRC16 = false;
                PACKET_SIZE = 132;
            }
        }

        cout << "Sciezka do pliku: ";
        string path;
        cin >> path;
        cout << "--------------------------------" << endl;
        system("pause");

        if (mode == 1)
        {
            /* ODBIERANIE */
            receiveFile(path);
            cout << "--------------------------------" << endl;
        }
        else if (mode == 2)
        {
            /* NADAWANIE */
            vector<unsigned char> fileBuffer = readFile(path);
            if (fileBuffer.size() == 0)
            {
                system("pause");
                return 1;
            }

            if (establishConnection())
            {
                cout << "Polaczenie nawiazane, przesylanie danych..." << endl;
                sendFile(fileBuffer);
                cout << "--------------------------------" << endl;
            }
            else
            {
                cout << "Nie udalo sie nawiazac polaczenia" << endl;
                system("pause");
                return 1;
            }
        }
        else
        {
            cout << "Wystapil blad, sprobuj ponownie";
        }
        system("pause");
        cout << "--------------------------------" << endl;
    }

    system("pause");
    return 0;
}