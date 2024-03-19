import java.util.ArrayList;
import java.util.Arrays;

abstract public class oneBit {

    private static final int H_ROWS = 4;
    private final int H_COLUMNS = 12; /* 8 bitów wiadomości oraz 4 bity parzystości */
    private static final int[][] H = {
        {1, 1, 1, 0, 1, 1, 1, 0,     1, 0, 0, 0},
        {1, 1, 0, 1, 1, 0, 0, 1,     0, 1, 0, 0},
        {1, 0, 1, 1, 0, 1, 0, 0,     0, 0, 1, 0},
        {0, 1, 1, 1, 0, 0, 1, 1,     0, 0, 0, 1}
    };

    public oneBit(){
        System.out.println("oneBit class");
        System.out.println(Arrays.deepToString(H));
    }

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
        for (int i = 0; i < T.length; i++) {
            output += H[row][i] * T[i];
        }
        return output%2;
    }

    static int[] checkMsg(int[] block){
        int[] T = new int[8];
        for (int i = 0; i < block.length; i++) {
            T[i] = block[i];
        }
        //TODO zamienić na 8 dla korekcji 2 bitów
        int[] E = new int[H_ROWS];
        for (int i = 0; i < H_ROWS; i++) {
            E[i] = multiplyVectors(T, i) + T[8 + i];
        }

        return T;
    }
}
