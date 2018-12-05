#include <XBee.h>
#include <avr/pgmspace.h>
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();

uint8_t cmd;                      //content of payload stored here

const unsigned char motor1F = 3;
const unsigned char motor1B = 5;
const unsigned char motor2F = 6;
const unsigned char motor2B = 9;
const unsigned char speed = 200;

const unsigned char go_Forward = 1;
const unsigned char go_Backward = 3;
const unsigned char go_Left = 5;
const unsigned char go_Right = 7;
const unsigned char go_Stop = 0;


void setup() {
  Serial.begin(9600);
  xbee.setSerial(Serial);
  pinMode(motor1F, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2F, OUTPUT);
  pinMode(motor2B, OUTPUT);
  //Motor Setup End//
  Serial.println("Initialization done. Ready!");
}


void Forward() {
  analogWrite(motor1F, speed);
  analogWrite(motor2F, speed + 1);
}

void Backward() {
  analogWrite(motor1B, speed);
  analogWrite(motor2B, speed + 1);
}

void Left() {
  analogWrite(motor1F, speed/2);
  analogWrite(motor2B, (speed + 1)/2);
}

void Right() {
  analogWrite(motor1B, speed/2);
  analogWrite(motor2F, (speed + 1)/2);
}


void Stop() {
  analogWrite(motor1F, LOW);
  analogWrite(motor1B, LOW);
  analogWrite(motor2F, LOW);
  analogWrite(motor2B, LOW);
}


void loop() {
  xbee.readPacket(100);
  if (xbee.getResponse().isAvailable() &&  xbee.getResponse().getApiId() == RX_16_RESPONSE) {
    xbee.getResponse().getRx16Response(rx16);
    cmd = rx16.getData(0);
    if (cmd == go_Forward) {
      Serial.println(cmd);
      Forward();
      Serial.println(F("FORWARD"));

    }
    else if (cmd == go_Left) {
      Serial.println(cmd);
      Left();
      Serial.println(F("LEFT"));

    }
    else if (cmd == go_Right) {
      Serial.println(cmd);
      Right();
      Serial.println(F("RIGHT"));

    }
    else if (cmd == go_Backward) {
      Serial.println(cmd);
      Backward();
      Serial.println(F("BACKWARD"));

    }
    else if (cmd == go_Stop) {
      Stop();
      Serial.println(F("STOP"));
    }
  }
  delay(10);
}
