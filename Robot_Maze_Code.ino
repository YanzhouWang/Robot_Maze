#include <SD.h>
int dir;
File myFile;
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    return;
  }
  if (SD.exists("a.txt")) {
    SD.remove("a.txt");
  }
  myFile = SD.open("a.txt", FILE_WRITE);


  
  Serial.println("initialization done.");
  Serial.println("Ready!");
}

void Record() {
  Serial.println("In Record:")
  // reply only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    dir = Serial.parseInt();
    if (dir == 1) {
      Serial.println(dir);
      myFile.write("1");
    }
    else if (dir == 2) {
      Serial.println(dir);
      myFile.write("2");

    }
    else if (dir == 3) {
      Serial.println(dir);
      myFile.write("3");

    }
    else if (dir == 4) {
      Serial.println(dir);
      myFile.write("4");
    }
    else if (dir == 10) {
      myFile.close();
      Serial.println("DONE!");
    }
  }
}




void loop() {

  Record();

}
