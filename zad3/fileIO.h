#ifndef ZAD3_FILEIO_H
#define ZAD3_FILEIO_H
#include <string>
#include <vector>
#include <map>
#include "typedefs.h"

/*!
 * @brief Zapisanie bufora do pliku
 * @param filePath sciezka do pliku
 * @param fileBuffer bufor zawierajacy bajty
 * @return plik zapisany - 0, w innym wypadku 1
 */
int saveFile(std::string filePath, std::vector<char> fileBuffer);

/*!
 * @brief Wczytywanie pliku
 * @param filePath sciezka do pliku
 * @return vector bajtow pliku
 */
std::vector<char> readFile(std::string filePath);

void saveBitsToFile(std::string bits, std::string path);

std::string readBitsFromFile(const std::string &filename);

void saveDictToFile(std::vector<HuffmanDictElement> dict, std::string filepath);

std::vector<HuffmanDictElement> readDictFromFile(std::string filepath);

#endif // ZAD3_FILEIO_H