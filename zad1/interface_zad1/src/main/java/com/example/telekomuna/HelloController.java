package com.example.telekomuna;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.*;
import java.net.URL;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.ResourceBundle;

public class HelloController implements Initializable {
    @FXML
    private Button chooseFile;

    @FXML
    private Button saveFile;

    @FXML
    private Button loadFile;

    @FXML
    private RadioButton encode;

    @FXML
    private RadioButton decode;

    @FXML
    private TextArea area1;

    @FXML
    private TextArea area2;

    @FXML
    private TextField fileName;

    @FXML Button execute;

    private Stage stage;

    private Path path;

    int[] fileBytes;

    ArrayList<Integer> encoded;



    @Override
    public void initialize(URL url, ResourceBundle resourceBundle) {
        ToggleGroup group = new ToggleGroup();
        encode.setToggleGroup(group);
        encode.setSelected(true);
        decode.setToggleGroup(group);
    }




    @FXML
    public void setChooseFile() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("Open Resource File");
        File file = fileChooser.showOpenDialog(stage);
        if (file != null) {
            path = file.toPath();
            fileName.setText(path.toString());
        }
    }

    @FXML
    public void load()
    {
        area1.setText("");
        area2.setText("");
        File file = new File(path.toUri());

        // Tworzenie tablicy bitów do przechowywania danych
        fileBytes = new int[(int) (file.length() * 8)]; // *8 dla każdego bajtu

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
            if(encode.isSelected())
            {
                for(int i=0;i<fileBytes.length;i++)
                {
                    if((i + 1) % 8 == 0)
                    {
                        area1.setText(area1.getText()+fileBytes[i]+"\n");
                    }
                    else
                    {
                        area1.setText(area1.getText()+fileBytes[i]);
                    }

                }
            }
            else
            {
                for(int i=0;i<fileBytes.length;i++)
                {
                    if((i + 1) % 16 == 0)
                    {
                        area1.setText(area1.getText()+fileBytes[i]+"\n");
                    }
                    else if((i + 1) % 8 == 0)
                    {
                        area1.setText(area1.getText()+fileBytes[i]+" ");
                    }
                    else
                    {
                        area1.setText(area1.getText()+fileBytes[i]);
                    }

                }
            }

            // Zamykanie strumienia
            fileInputStream.close();

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @FXML
    public void onExecuteClick()
    {
        if(encode.isSelected())
        {
            encoded = twoBit.encode(fileBytes);
            for(int i=0;i<encoded.size();i++)
            {
                if((i + 1) % 16 == 0)
                {
                    area2.setText(area2.getText()+encoded.get(i)+"\n");
                }
                else if((i + 1) % 8 == 0)
                {
                    area2.setText(area2.getText()+encoded.get(i)+" ");
                }
                else
                {
                    area2.setText(area2.getText()+encoded.get(i));
                }

            }

        }
        else
        {
            encoded = twoBit.decode(fileBytes);
            for(int i=0;i<encoded.size();i++)
            {
                if((i + 1) % 8 == 0)
                {
                    area2.setText(area2.getText()+fileBytes[i]+"\n");
                }
                else
                {
                    area2.setText(area2.getText()+fileBytes[i]);
                }
            }
        }
    }

    @FXML
    public void onSaveClick()
    {
        String outputPath = "output.txt";
        int[] interim = new int[encoded.size()];
        for(int i =0; i< encoded.size();i++)
        {
            interim[i] = encoded.get(i);
        }
        byte[] bytes = new byte[interim.length / 8]; // Dziel przez 8, ponieważ potrzebujemy 8 bitów na bajt
        for (int i = 0; i < interim.length / 8; i++) {
            for (int j = 0; j < 8; j++) {
                bytes[i] |= (byte) ((interim[i * 8 + j] & 1) << (7 - j)); // Przekształć 8 bitów na bajt
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
}