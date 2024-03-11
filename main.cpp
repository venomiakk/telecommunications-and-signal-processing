#include <iostream>
#include <vector>

using namespace std;

const int sbH_ROWS = 4;
const int sbH_COLUMNS = 12; /* 8 bitów wiadomości oraz 4 bity parzystości */
const int dbH_ROWS = 8;
const int dbH_COLUMNS = 16; /* 8 bitów wiadomości oraz 8 bitów parzystości */

int sbH[sbH_ROWS][sbH_COLUMNS] = {
        {1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0},
        {1, 1, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0},
        {1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0},
        {0, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1}
};
int dbH[dbH_ROWS][dbH_COLUMNS] = {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
        {1, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
};



void printMatrix(int matrix[8][16]) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 16; ++j) {
            if (j == 8) {
                cout << " ";
            }
            cout << matrix[i][j];
        }
        cout << endl;
    }
}

void changeBits(int &bit) {
    if (bit == 1){
        bit = 0;
    } else {
        bit = 1;
    }
}

void txtToInt(vector<int> &msg, string txt) {
    for (int i = 0; i < txt.size(); ++i) {
        msg.push_back((int) txt.at(i) - 48);
    }
}

/*
 *  Przy kodowaniu:
 *      Obliczanie bitów parzystości
 *      Każdy bit parzystości jest obliczany jako iloczyn bitów wiadomości i bitów danego wiersza
 */
int multiplyVectors(vector<int> &T, int row) {
    int output = 0;
    for (int i = 0; i < row; ++i) {
        output += sbH[row][i] * T[i];
        cout<<output<<", ";
    }
    return output % 2;
}

//Kodowanie podanej wiadomości poprzez dodanie bitów parzystości
void encode(vector<int> &msg) {
    for (int i = 0; i < sbH_ROWS; ++i) {
        msg.push_back(multiplyVectors(msg, i));
    }
}

/*
 * Dekodowanie wiadomości
 */
void checkMsg (vector<int> &T){
    vector<int> E;
    int sum = 0;
    for (int i = 0; i < sbH_ROWS; ++i) {
        E.push_back(multiplyVectors(T, i));
        cout<<E[i]<<endl;
        sum += E[i];
    }
    if (sum!=0){
        cout<<"Wrong msg!";
    } else{
        cout<<"Correct msg!";
    }
}

int main() {

    vector<int> input;
    string txt = "11000101";
    txtToInt(input, txt);
    encode(input);
    for (int i = 0; i < input.size(); ++i) {
        cout << input[i];
    }
//    system("pause");
    cout<<endl;

    vector<int> invalidInput;
    string valid = "110001010111";
    string invalid = "11000010111";
    txtToInt(invalidInput, invalid);
    checkMsg(invalidInput);
    return 0;
}
