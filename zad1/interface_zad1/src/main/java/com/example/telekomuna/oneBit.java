package com.example.telekomuna;

import java.util.ArrayList;
import java.util.Arrays;

abstract public class oneBit {

    private static final int H_ROWS = 4;
    private static final int H_COLUMNS = 12; /* 8 bitów wiadomości oraz 4 bity parzystości */
    private static final int[][] H = {
        {1, 1, 1, 0, 1, 1, 1, 0,     1, 0, 0, 0},
        {1, 1, 0, 1, 1, 0, 0, 1,     0, 1, 0, 0},
        {1, 0, 1, 1, 0, 1, 0, 0,     0, 0, 1, 0},
        {0, 1, 1, 1, 0, 0, 1, 1,     0, 0, 0, 1}
    };


    static ArrayList<Integer> encode(int[] paramBits) {
        ArrayList<Integer> interim = new ArrayList<>();

        int[] msgBits = new int[paramBits.length];
        for (int i = 0; i < paramBits.length; i++) {
            msgBits[i] = paramBits[i];
        }
        int blocks = (msgBits.length+1)%8;


        for (int i = 0; i < msgBits.length; i++) {
            interim.add(msgBits[i]);
        }

    //TODO: podzielić na bloki po 8 bitów i kodować każdy pojedyńczo
    //    Tablica ArrayList?

        int[] parityBits = new int[H_ROWS];
        for (int i = 0; i < H_ROWS; i++) {
            parityBits[i] = multiplyVectors(msgBits, i);
        }

        for (int i = 0; i < parityBits.length; i++) {
            interim.add(parityBits[i]);
        }

        return interim;
    }

    void decode() {}

    static int multiplyVectors(int[] T, int row){
        int output = 0;
        //TODO: Czy trzeba zmienić dla 2 bitów?
        for (int i = 0; i < 8; i++) {
            output += H[row][i] * T[i];
        }
        return output%2;
    }

    static int[] checkMsg(ArrayList<Integer> block){
        int[] T = new int[H_COLUMNS]; //TODO:Zamienić na 16 dla 2 bitów
        for (int i = 0; i < block.size(); i++) {
            T[i] = block.get(i);
        }
        System.out.println(Arrays.toString(T));
        //TODO zamienić na 8 dla korekcji 2 bitów
        int[] E = new int[H_ROWS];
        int sum = 0;
        for (int i = 0; i < H_ROWS; i++) {
            E[i] = multiplyVectors(T, i) + T[8 + i];
            System.out.println(E[i]);
            E[i] = E[i] % 2;
            sum += E[i];
        }

        if (0 != sum){
            correctMsg(E, T);
            System.out.println("Niepoprawna wiadomość!");
            System.out.println(Arrays.toString(T));
            //T = correctMsg(E, T);
        }
        int[] T2 = new int[8];
        for (int i = 0; i < T2.length; i++) {
            T2[i] = T[i];
        }
        return T2;
    }

    static void correctMsg(int[] E, int[] T){
        int errors = 0;
        for (int i = 0; i < 8; i++) {
            int count = 0;
            for (int j = 0; j < H_ROWS; j++) {
                if (E[j] == H[j][i]){
                    count++;
                }
            }
            if (count==H_ROWS){
                errors++;
                System.out.println("Blad wystapil na miejscu " +  (i+1));
                if (T[i]== 1){
                    T[i] = 0;
                } else {
                    T[i] = 1;
                }
            }
            if (1 == errors){
                return;
            }
        }

    }
}
