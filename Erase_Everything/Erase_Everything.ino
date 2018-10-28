//This code deletes "analysis.txt" file in the SD card
//so that the SD card can remain in the breakout board during operation
#include <SD.h>
File myFile;
const int chip = 10;

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  //SD Card Setup Begin//
  Serial.print("Initializing SD card...");
  pinMode(chip, OUTPUT);

  while (!SD.begin(chip)) { //keep trying to connect to SD card until success
    Serial.println("SD card initialization failed!");
    delay(2000);
  }
  if (SD.exists("analysis.txt") && SD.exists("record.txt")) {
    SD.remove("analysis.txt");
    Serial.println("Erased analysis only....");
    return;

  }
  if (!SD.exists("analysis.txt")) {
    SD.remove("record.txt");
    Serial.println("Erased everything....");
    return;
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
