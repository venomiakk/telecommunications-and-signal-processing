module com.example.telekomuna {
    requires javafx.controls;
    requires javafx.fxml;


    opens com.example.telekomuna to javafx.fxml;
    exports com.example.telekomuna;
}