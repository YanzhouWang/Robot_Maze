#include <SD.h>
#include <XBee.h>

File myFile;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();                                         
uint8_t dir{};                              //Direction data


void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  //SD Card Setup Begin//
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
  //SD Card Setup End//


  //XBee Setup Begin//
  xbee.setSerial(Serial);
  //XBee Setup End//

  Serial.println("initialization done.");
  Serial.println("Ready!");
}

void Record() {
  xbee.readPacket(1000);
  // reply only when you receive data:
  if (xbee.getResponse().isAvailable() &&  xbee.getResponse().getApiId() == RX_16_RESPONSE) {
    xbee.getResponse().getRx16Response(rx16);
    dir = rx16.getData(0);

    if (dir == 1) {
      Serial.println(dir);
      myFile.write("1");
    }
    else if (dir == 3) {
      Serial.println(dir);
      myFile.write("3");

    }
    else if (dir == 5) {
      Serial.println(dir);
      myFile.write("5");

    }
    else if (dir == 7) {
      Serial.println(dir);
      myFile.write("7");
    }
    else if (dir == 4) {
      myFile.close();
      Serial.println("DONE!");
    }
  }
}




void loop() {

  Record();

}
