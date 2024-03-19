import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;

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

        // Ścieżka do pliku wyjściowego
        String outputPath = "output.txt";

        //bity
        //// Konwersja tablicy bitów na ciąg znaków
        //StringBuilder stringBuilder = new StringBuilder();
        //for (int b : fileBits) {
        //    stringBuilder.append(b);
        //}
        //String content = stringBuilder.toString();
        //
        //
        //try {
        //    // Tworzenie strumienia FileOutputStream
        //    FileOutputStream fileOutputStream = new FileOutputStream(outputPath);
        //
        //    // Zapisywanie danych z tablicy bitów do pliku
        //    fileOutputStream.write(content.getBytes("UTF-8"));
        //
        //    // Zamykanie strumienia
        //    fileOutputStream.close();
        //
        //    System.out.println("Tablica bitów została zapisana jako znaki w pliku.");
        //
        //} catch (IOException e) {
        //    e.printStackTrace();
        //}

        //znaki
        // Konwersja tablicy bitów na ciąg bajtów
        byte[] bytes = new byte[fileBits.length / 8]; // Dziel przez 8, ponieważ potrzebujemy 8 bitów na bajt
        for (int i = 0; i < fileBits.length / 8; i++) {
            for (int j = 0; j < 8; j++) {
                bytes[i] |= (byte) ((fileBits[i * 8 + j] & 1) << (7 - j)); // Przekształć 8 bitów na bajt
            }
        }

        try {
            // Tworzenie strumienia FileOutputStream
            FileOutputStream fileOutputStream = new FileOutputStream(outputPath);

            // Zapisywanie danych z tablicy bitów do pliku jako znaki w kodowaniu UTF-8
            fileOutputStream.write(bytes);

            // Zamykanie strumienia
            fileOutputStream.close();

            System.out.println("Tablica bitów została zapisana jako znaki w pliku.");

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {

        //System.out.println(Arrays.toString(readFile("input.txt")));
        //createFile(readFile("input.txt"));
        int[] bits = readFile("input.txt");
        System.out.println("Tekst oryg: " + Arrays.toString(bits));

        ArrayList<Integer> encoded = oneBit.encode(bits);
        System.out.println("Zakodowane: " + encoded);
    }


}