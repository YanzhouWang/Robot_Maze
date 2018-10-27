#include <SD.h>
#include <XBee.h>

File recordings;
File analysis;
const int chipSelect = 10;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();
uint8_t cmd{};                              //Direction data
char cmds {};                     //Variable length array to store retrieved data from SD card


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

  while (!SD.begin(chipSelect)) { //keep trying to connect to SD card until success
    Serial.println("SD card initialization failed!");
    delay(2000);
  }
  //if there exists an old analyzed file (analyzed.txt), skip the recording; otherwise,
  //remove the old recordings and start a new one.
  if (!SD.exists("analysis.txt")) {
    SD.remove("record.txt");
    Serial.println("Removed old record. Setting up recording environment.");
    recordings = SD.open("record.txt", FILE_WRITE);//having recordings open will lead the main program into Record() function
  }
  else {
    Serial.println("analysis.txt exists. Skip Recording.");
  }
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

  Serial.println("Initialization done.");
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
  while (recordings) {
    xbee.readPacket(1000); //wait 1s for response
    // reply only when you receive data:
    if (xbee.getResponse().isAvailable() &&  xbee.getResponse().getApiId() == RX_16_RESPONSE) {

      xbee.getResponse().getRx16Response(rx16);
      cmd = rx16.getData(0);
      if (cmd == 1) {
        Serial.println(cmd);
        Forward();
        recordings.write("1");
      }
      else if (cmd == 3) {
        Serial.println(cmd);
        Left();
        recordings.write("3");

      }
      else if (cmd == 5) {
        Serial.println(cmd);
        Right();
        recordings.write("5");

      }
      else if (cmd == 7) {
        Serial.println(cmd);
        Backward();
        recordings.write("7");
      }
      else if (cmd == 4) {
        recordings.close(); //close file after it's done; avoid re-entering Record function.
        Serial.println("DONE RECORDING!");
      }
    }
  }
}

void Read() {
  recordings = SD.open("record.txt", FILE_READ);
  if (recordings) {
    Serial.println("Start Reading...");
    int i = 0;
    while ((cmds = recordings.read()) != -1) {
      Serial.println(cmds);
      i++;
    }
    recordings.close();
    Serial.println("DONE READING!");
    Serial.println(sizeof(cmds));
  }
}

void Analyze() {
  Serial.println("START ANALYZING");
  analysis = SD.open("analysis.txt", FILE_WRITE);
  //Read the characters from the file one by one, then counter++, therefore we can know the size of the vector for further analysis.
}



void loop() {
  Record();
  Read();
  Analyze();
  Serial.println("Oops");
  delay(5000);

}
