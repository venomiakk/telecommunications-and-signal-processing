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

/*
 * Funkcja zamieniajaca wprowadzony tekst na tablice vector intów
 */
void txtToInt(vector<int> &msg, string txt) {
    for (int i = 0; i < txt.size(); ++i) {
        msg.push_back((int) txt.at(i) - 48);
    }
}

/*
 * Funkcja odpowiedzialna za mnożenie wektorów
 */
int multiplyVectors(vector<int> &T, int row) {
    int output = 0;
    for (int i = 0; i < 8; ++i) {
        output += sbH[row][i] * T[i];
    }
    return output % 2;
}

/*
 * Kodowanie podanej wiadomości poprzez dodanie bitów parzystości
 * Jest to rozwiązane poprzez mnożenie wektorów
 * Każdy wiersz macierzy H jest mnożony przez wektor wiadomości T
 * Następnie z sumy elementów wektora wyjściowego wyciągana jest wartość 0 lub 1 poprzez operację modulo 2
*/
 void encode(vector<int> &msg) {
    for (int i = 0; i < sbH_ROWS; ++i) {
        msg.push_back(multiplyVectors(msg, i));
    }
}

/*
 * Funkcja odpowiadająca za poprawianie wiadomości
 * Wektor błędów porównywany jest z odpowiednią kolumną macierzy H
 * Gdy wektory są sobie równe, błąd wystąpił na miejscu odpowiadającym danej kolumnie macierzy H
 */

void correctMsg(vector<int> &E, vector<int> &T){
    for (int i = 0; i < 8; ++i) {
        int count = 0;
        for (int j = 0; j < 4; ++j) {
            if (E[j] == sbH[j][i]){
                count++;
            }
        }
        if (count == 4){
            cout<<"Blad wystapil na miejscu "<<i+1<<endl;
            if (T[i] == 1){
                T[i] = 0;
            } else {
                T[i] = 1;
            }
            return;
        }
    }
}

/*
 * Funkcja odpowiedzialna za sprawdzenie czy wiadomosc jest poprawna
 * Rzędy macierzy H są mnożone z wektorem wiadomości T a do wyniku dodawany jest odpowiedni bit parzystości
 * Gdy suma elementów wektora wyjściowego jest równa 0, wtedy błąd nie wystąpił
 * Gdy suma nie jest równa 0, następuje poprawienie wiadomości
 */
void checkMsg (vector<int> &T){
    vector<int> E;
    int sum = 0;
    for (int i = 0; i < sbH_ROWS; ++i) {
        E.push_back(multiplyVectors(T, i) + T[8+i]);
        E[i] = E[i] % 2;
        sum += E[i];
    }

    if (sum!=0){
        cout<<"Niepoprawna wiadomosc"<<endl;
        correctMsg(E, T);
        cout<<"Poprawiona wiadomosc: ";
        for (int i = 0; i < T.size(); ++i) {
            cout<<T[i];
        }
    } else{
        cout<<"Poprawna wiadomosc";
    }
}

int main() {

    cout << "Kodowanie:" <<endl;
    vector<int> input;
    string txt = "01100001";
    txtToInt(input, txt);
    encode(input);
    for (int i = 0; i < input.size(); ++i) {
        cout << input[i];
    }
//    system("pause");
    cout<<endl;
    cout << "Dekodowanie:" <<endl;
    vector<int> invalidInput;
    string valid = "110001011100";
    string invalid = "111001011100";
    txtToInt(invalidInput, invalid);
    checkMsg(invalidInput);
    return 0;
}
