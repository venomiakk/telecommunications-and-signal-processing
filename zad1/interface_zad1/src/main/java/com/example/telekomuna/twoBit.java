package com.example.telekomuna;

import java.util.ArrayList;
import java.util.Arrays;

abstract public class twoBit {

    private static final int H_ROWS = 8;
    private static final int H_COLUMNS = 16; /* 8 bitów wiadomości oraz 8 bitów parzystości */

    //private static final int[][] H = {
    //        {1, 1, 1, 1, 1, 1, 1, 1,    1, 0, 0, 0, 0, 0, 0, 0},
    //        {1, 1, 0, 1, 1, 1, 1, 1,    0, 1, 0, 0, 0, 0, 0, 0},
    //        {1, 1, 0, 1, 1, 1, 0, 1,    0, 0, 1, 0, 0, 0, 0, 0},
    //        {1, 1, 0, 1, 0, 1, 0, 1,    0, 0, 0, 1, 0, 0, 0, 0},
    //        {1, 0, 0, 1, 0, 1, 0, 1,    0, 0, 0, 0, 1, 0, 0, 0},
    //        {1, 0, 0, 1, 0, 1, 0, 0,    0, 0, 0, 0, 0, 1, 0, 0},
    //        {1, 0, 0, 1, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 1, 0},
    //        {1, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 1}
    //};

    private static final int[][] H = {
            {0, 1, 1, 1, 1, 1, 1, 1,    1, 0, 0, 0, 0, 0, 0, 0},
            {1, 0, 1, 1, 1, 1, 1, 1,    0, 1, 0, 0, 0, 0, 0, 0},
            {1, 1, 0, 1, 1, 1, 1, 1,    0, 0, 1, 0, 0, 0, 0, 0},
            {1, 1, 1, 0, 1, 1, 1, 1,    0, 0, 0, 1, 0, 0, 0, 0},
            {1, 1, 1, 1, 0, 1, 1, 1,    0, 0, 0, 0, 1, 0, 0, 0},
            {1, 1, 1, 1, 1, 0, 1, 1,    0, 0, 0, 0, 0, 1, 0, 0},
            {1, 1, 1, 1, 1, 1, 0, 1,    0, 0, 0, 0, 0, 0, 1, 0},
            {1, 1, 1, 1, 1, 1, 1, 0,    0, 0, 0, 0, 0, 0, 0, 1},
    };


    static ArrayList<Integer> encode(int[] paramBits) {
        ArrayList<Integer> output = new ArrayList<>();
        int[] msgBits = new int[paramBits.length];
        for (int i = 0; i < paramBits.length; i++) {
            msgBits[i] = paramBits[i];
        }
        //int blocks = (msgBits.length / 8) + (msgBits.length % 8 == 0 ? 0 : 1);
        int blocks = msgBits.length / 8; //Zakładamy że zawsze plik będzie miał rozmiar wielokrotności 8
        //System.out.println(msgBits.length);
        //System.out.println(blocks);


        int index = 0;
        for (int j = 0; j < blocks; j++) {
            int[] tempMsg = new int[8];
            for (int k = 0; k < 8; k++) {
                tempMsg[k] = msgBits[index++];
            }

            ArrayList<Integer> interim = new ArrayList<>();
            for (int i = 0; i < tempMsg.length; i++) {
                interim.add(tempMsg[i]);
            }

            int[] parityBits = new int[H_ROWS];
            for (int i = 0; i < H_ROWS; i++) {
                parityBits[i] = multiplyVectors(tempMsg, i);
            }

            for (int i = 0; i < parityBits.length; i++) {
                interim.add(parityBits[i]);
            }

            output.addAll(interim);
        }
        //System.out.println(index);
        //System.out.println(output);
        return output;
    }

    static ArrayList<Integer> decode(int[] bits) {
        ArrayList<Integer> output = new ArrayList<>();
        int blocks = bits.length / 16;
        int index = 0;
        for (int i = 0; i < blocks; i++) {
            int[] temp = new int[H_COLUMNS];
            for (int j = 0; j < H_COLUMNS; j++) {
                temp[j] = bits[index++];
            }
            int[] interim = checkMsg(temp);

            for (int j = 0; j < interim.length; j++) {
                output.add(interim[j]);
            }
        }
        return output;
    }

    static int multiplyVectors(int[] T, int row) {
        int output = 0;
        //TODO: Czy trzeba zmienić dla 2 bitów?
        for (int i = 0; i < 8; i++) {
            output += H[row][i] * T[i];
        }
        //System.out.println(output + " " + output % 2);
        return output % 2;
    }


    static int[] checkMsg(int[] block) {
        int[] T = new int[H_COLUMNS];
        for (int i = 0; i < block.length; i++) {
            T[i] = block[i];
        }
        //System.out.println(Arrays.toString(T));
        int[] E = new int[H_ROWS];
        int sum = 0;
        for (int i = 0; i < H_ROWS; i++) {
            E[i] = multiplyVectors2(T, i) ;
            //System.out.println(E[i]);
            E[i] = E[i] % 2;
            sum += E[i];
        }
        if (0 != sum) {
            boolean isCorrected = correct1bit(E, T);
            System.out.println("Niepoprawna wiadomość!");
            if (!isCorrected) {
                correct2bits(E, T);
            }


            //System.out.println(Arrays.toString(T));
            //T = correctMsg(E, T);
        }
        int[] T2 = new int[8];
        for (int i = 0; i < T2.length; i++) {
            T2[i] = T[i];
        }
        return T2;
    }

    static int multiplyVectors2(int[] T, int row) {
        int output = 0;
        for (int i = 0; i < 16; i++) {
            output += H[row][i] * T[i];
        }
        //System.out.println(output + " " + output % 2);
        return output % 2;
    }


    static boolean correct1bit(int[] E, int[] T) {
        int errors = 0;
        for (int i = 0; i < 8; i++) {
            int count = 0;
            for (int j = 0; j < H_ROWS; j++) {
                if (E[j] == H[j][i]) {
                    count++;
                }
            }
            if (count == H_ROWS) {
                errors++;
                System.out.println("Blad wystapil na miejscu " + (i + 1));
                if (T[i] == 1) {
                    T[i] = 0;
                } else {
                    T[i] = 1;
                }
            }
            if (1 == errors) {
                return true;
            }

        }
        return false;
    }

    static void correct2bits(int[] E, int[] T){
        int col1, col2;
        // Tablica wyników
        int[] sums = new int[E.length];

        // Sumowanie kolumn
        for (int i = 0; i < 8; i++) {
            for (int j = i; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    sums[k] = H[k][i] + H[k][j];
                    sums[k] = sums[k] % 2;
                }
                //System.out.println(Arrays.toString(sums));
                // Porównanie z tablicą E
                boolean equal = true;
                for (int i2 = 0; i2 < E.length; i2++) {
                    if (sums[i2] != E[i2]) {
                        equal = false;
                        break;
                    }
                }
                if (equal) {
                    col1 = i;
                    col2 = j;
                    System.out.println("Błędy wystąpiły na miejscach: "+ " " + (i+1) + ", " + (j+1));
                    if (T[i] == 1) {
                        T[i] = 0;
                    } else {
                        T[i] = 1;
                    }
                    if (T[j] == 1) {
                        T[j] = 0;
                    } else {
                        T[j] = 1;
                    }

                    return;
                }
            }

        }

    }

}

