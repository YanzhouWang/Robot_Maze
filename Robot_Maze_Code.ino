#include <SD.h>
#include <XBee.h>

File myFile;
const int chipSelect = 10;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
uint8_t dir{};                              //Direction data


int motor1F = 3;
int motor1B = 5;
int motor2F = 6;
int motor2B = 9;
double speed = 255;
double turnTime = 1000; //motor turning time; subject to wheel specifications


void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  //SD Card Setup Begin//
  Serial.print("Initializing SD card...");
  pinMode(chipSelect, OUTPUT);

  if (!SD.begin(chipSelect)) {
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

  //Motor Setup Begin//
  pinMode(motor1F, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2F, OUTPUT);
  pinMode(motor2B, OUTPUT);
  //Motor Setup End//

  Serial.println("initialization done.");
  Serial.println("Ready!");
}

void Forward() {
  analogWrite(motor1F, speed);
  analogWrite(motor2F, speed);
  delay(turnTime);
  analogWrite(motor1F, LOW);
  analogWrite(motor2F, LOW);
  delay(500);
}

void Backward() {
  analogWrite(motor1B, speed);
  analogWrite(motor2B, speed);
  delay(turnTime);
  analogWrite(motor1B, LOW);
  analogWrite(motor2B, LOW);
  delay(500);
}

void Left() {
  analogWrite(motor1F, speed);
  analogWrite(motor2B, speed);
  delay(turnTime);
  analogWrite(motor1F, LOW);
  analogWrite(motor2B, LOW);
  delay(500);
}

void Right() {
  analogWrite(motor1B, speed);
  analogWrite(motor2F, speed);
  delay(turnTime);
  analogWrite(motor1B, LOW);
  analogWrite(motor2F, LOW);
  delay(500);
}

void Record() {
  //only runs when the file is open and ready to write
  while (myFile) {
    xbee.readPacket(1000); //wait 1s for response
    // reply only when you receive data:
    if (xbee.getResponse().isAvailable() &&  xbee.getResponse().getApiId() == RX_16_RESPONSE) {

      xbee.getResponse().getRx16Response(rx16);
      dir = rx16.getData(0);
      if (dir == 1) {
        Serial.println(dir);
        Forward();
        myFile.write("1");
      }
      else if (dir == 3) {
        Serial.println(dir);
        Left();
        myFile.write("3");

      }
      else if (dir == 5) {
        Serial.println(dir);
        Right();
        myFile.write("5");

      }
      else if (dir == 7) {
        Serial.println(dir);
        Backward();
        myFile.write("7");
      }
      else if (dir == 4) {
        myFile.close(); //close file after it's done; avoid re-entering Record function.
        Serial.println("DONE RECORDING!");
      }
    }
  }
}




void loop() {
  Record();
  Serial.println("Oops");

}
