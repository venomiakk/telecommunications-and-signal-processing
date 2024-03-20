import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Main {

    static int[] readFile(String path){
        // Tworzenie obiektu File
        File file = new File(path);

        // Tworzenie tablicy bitów do przechowywania danych
        int[] fileBytes = new int[(int) (file.length() * 8)]; // *8 dla każdego bajtu

        try {
            // Tworzenie strumienia FileInputStream
            FileInputStream fileInputStream = new FileInputStream(file);

            // Odczytywanie danych z pliku do tablicy bitów
            int bytesRead;
            int index = 0;
            while ((bytesRead = fileInputStream.read()) != -1) {
                // Konwersja bajtu na tablicę bitów i zapis do tablicy bitów
                for (int i = 7; i >= 0; i--) {
                    fileBytes[index++] = (byte) ((bytesRead >> i) & 1);
                }
            }

            // Zamykanie strumienia
            fileInputStream.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
        return fileBytes;
    }

    static void createFile(int[] fileBits) {
        String outputPath = "output.txt";
        byte[] bytes = new byte[fileBits.length / 8]; // Dziel przez 8, ponieważ potrzebujemy 8 bitów na bajt
        for (int i = 0; i < fileBits.length / 8; i++) {
            for (int j = 0; j < 8; j++) {
                bytes[i] |= (byte) ((fileBits[i * 8 + j] & 1) << (7 - j)); // Przekształć 8 bitów na bajt
            }
        }

        try {
            FileOutputStream fileOutputStream = new FileOutputStream(outputPath);
            fileOutputStream.write(bytes);
            fileOutputStream.close();
            System.out.println("Tablica bitów została zapisana jako znaki w pliku.");
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        int[] bits = readFile("input.txt");
        System.out.println("Tekst oryg: " + Arrays.toString(bits));

        ArrayList<Integer> tbc = twoBit.encode(bits);
        //System.out.println(tbc);
        int[] tbcBits = new int[tbc.size()];
        for (int i = 0; i < tbcBits.length; i++) {
            tbcBits[i] = tbc.get(i);
        }
        //System.out.println();
        tbcBits[0] = 1;
        tbcBits[7] = 0;

        createFile(tbcBits);
        //ArrayList<Integer> decd = twoBit.decode(tbcBits);
        //System.out.println(decd);

        int[] bits2 = readFile("output.txt");
        ArrayList<Integer> decoded = twoBit.decode(bits2);
        int[] bits3 = new int[decoded.size()];
        for (int i = 0; i < bits3.length; i++) {
            bits3[i] = decoded.get(i);
        }
        createFile(bits3);

    }


}