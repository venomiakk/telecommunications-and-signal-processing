#include "fileIO.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <bitset>

using namespace std;

int saveFile(string filePath, vector<char> fileBuffer)
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

vector<char> readFile(string filePath)
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
    vector<char> fileBuffer(size);
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

void saveBitsToFile(string bits, string path)
{
    std::ofstream file(path, std::ios::binary); // Otwórz plik do zapisu w trybie binarnym

    for (size_t i = 0; i < bits.size(); i += 8)
    {
        unsigned char byte = 0;
        for (size_t j = 0; j < 8 && i + j < bits.size(); ++j)
        {
            byte |= (bits[i + j] - '0') << (7 - j);
        }
        file.write(reinterpret_cast<char *>(&byte), sizeof(byte));
    }

    file.close();
}

std::string readBitsFromFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);

    std::string bits;
    char byte;
    while (file.read(&byte, sizeof(byte)))
    {
        // konwersja bajtow na ciag bitow
        bits += std::bitset<8>(byte).to_string();
    }

    file.close();

    return bits;
}

void saveDictToFile(std::vector<HuffmanDictElement> dict, std::string filepath)
{
    vector<char> tmp;
    for (auto line : dict)
    {
        int decKey = static_cast<int>(line.key);
        string key = to_string(decKey);
        for (auto c : key)
        {
            tmp.push_back(c);
        }
        tmp.push_back(':');
        for (auto c : line.value)
        {
            tmp.push_back(c);
        }
        tmp.push_back('\n');
    }
    saveFile(filepath, tmp);
}

std::vector<HuffmanDictElement> readDictFromFile(std::string filepath)
{
    vector<HuffmanDictElement> dict;
    ifstream file(filepath);
    string line;
    char delimiter = ':';
    while (getline(file, line))
    {
        if (line == "")
        {
            break;
        }

        size_t pos = line.rfind(delimiter);
        string key = line.substr(0, pos);
        string value = line.substr(pos + 1);
        int keyByte = stoi(key);
        char keyChar = static_cast<char>(keyByte);
        HuffmanDictElement element;
        element.key = keyChar;
        element.value = value;
        dict.push_back(element);
    }
    return dict;
}