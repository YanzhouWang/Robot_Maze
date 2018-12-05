#include <XBee.h>
//set "xbee" as a XBee module
XBee xbee = XBee();
//arduino will send 0 at all times, 1 for left turn, 3 for right turn, 5 for go and 7 for back. 9 is for utility. 
//initialize the payload
uint8_t payload[] = {1};

//put payload into a package, set the address of receiving XBee module, FFFF for broadcast mode
Tx16Request tx16 = Tx16Request(0xFFFF, payload, sizeof(payload));

int flexSensorPinindex = A0;
int flexSensorPinmiddle = A1;

int buttonGo = 7;
int buttonUtil = 9;
int ledPin=12;

//initial state for the "GO" button and "Utility" Button
int GobuttonState = 0;
int UtbuttonState = 0;
int Gowrite = 0;
int Utwrite = 0;


int thresholdi = 870;
int thresholdm = 970;


int Xbeepackage =0;

void setup(){
  Serial.begin(9600);
xbee.setSerial(Serial);
pinMode(GobuttonState, INPUT);
pinMode(UtbuttonState, INPUT);

pinMode(flexSensorPinindex, INPUT);
pinMode(flexSensorPinmiddle, INPUT);
pinMode(ledPin,OUTPUT);

 
}

  void loop() {
// Read the values
int flexS1 = analogRead(flexSensorPinindex);
int flexS2 = analogRead(flexSensorPinmiddle);

// Read the values for Go button
GobuttonState = digitalRead(buttonGo);
if (GobuttonState == HIGH) {Gowrite =1; }
else {Gowrite =0;}
// Read the values for Utility button
UtbuttonState = digitalRead(buttonUtil);
if (UtbuttonState == HIGH) {Utwrite =1; }
else {Utwrite =0;}
  



 // Mapping all the sensor values down between 0-1000
  int flex_1_0to100 = map(flexS1, 1000, 0, 0, 1000);
  int flex_2_0to100 = map(flexS2, 1000, 0, 0, 1000);

//flex sensor 1 and 2 are both above threshold, then it signals go back words  
  if (flex_1_0to100 > thresholdi && flex_2_0to100 > thresholdm) {
  Xbeepackage = 7;
   digitalWrite(ledPin,LOW);
  payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
      
Serial.print(Xbeepackage);
}
// only index finger flex sensor is above threshold, turn left 
  else if (flex_1_0to100 > thresholdi && flex_2_0to100 < thresholdm) {
  Xbeepackage = 1;
   digitalWrite(ledPin,LOW);
  payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
      
  Serial.print(Xbeepackage);
}
// only middle finger flex sensor is above threshold, turn right 
  else if (flex_1_0to100 < thresholdi && flex_2_0to100 > thresholdm) {
  Xbeepackage = 3;
   digitalWrite(ledPin,LOW);
  payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
     
  Serial.print(Xbeepackage);
}
// there was an input of pushing the Go button
  else if (Gowrite ==1) {
  Xbeepackage = 5;
   digitalWrite(ledPin,LOW);
  payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
       
  Serial.print(Xbeepackage);
  
  // Reset the write value to 0 
  Gowrite =0;
}
// there was an input of pushing the Utility button
  else if (Utwrite ==1){
  Xbeepackage = 9;
   digitalWrite(ledPin,LOW);
  payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
      
  Serial.print(Xbeepackage);
  // Reset the write value to 0
  Utwrite =0;
} 
// no input is given at the moment, simply blinks and send 0 to xbee 
    else {
    digitalWrite(ledPin,HIGH);
    Xbeepackage = 0;
    payload[0] = Xbeepackage;
      xbee.send( tx16 );
      delay(100);
    }
       

// delay to block unintentional inputs 
delay(20);
}
