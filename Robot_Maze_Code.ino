#include <SD.h>
#include <XBee.h>


File recordings;
File analysis;
const int chipSelect = 10;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();

int motor1F = 3;
int motor1B = 5;
int motor2F = 6;
int motor2B = 9;
int speed = 255;
int turnTime = 1000; //motor turning time; subject to wheel specifications
//direction signals, depending on the controller setting
uint8_t go_Forward=5;
uint8_t go_Backward=1;
uint8_t go_Left=3;
uint8_t go_Right=7;


bool overall = true; //for debugging

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

  Serial.println("Initialization done. Ready!");
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

void pushZerosToEnd(int arr[], int n)
{
  int count = 0;  // Count of non-zero elements

  // Traverse the array. If element encountered is non-
  // zero, then replace the element at index 'count'
  // with this element
  for (int i = 0; i < n; i++)
    if (arr[i] != 0)
      arr[count++] = arr[i]; // here count is
  // incremented

  // Now all non-zero elements have been shifted to
  // front and  'count' is set as index of first 0.
  // Make all elements 0 from count to end.
  while (count < n)
    arr[count++] = 0;
}

void Record() {
  //only runs when the file is open and ready to write
  while (recordings) {
    uint8_t cmd{};                              //Direction data
    xbee.readPacket(1000); //wait 1s for response
    // reply only when you receive data:
    if (xbee.getResponse().isAvailable() &&  xbee.getResponse().getApiId() == RX_16_RESPONSE) {

      xbee.getResponse().getRx16Response(rx16);
      cmd = rx16.getData(0);
      if (cmd == go_Forward) {
        Serial.println(cmd);
        Forward();
        recordings.write("5");  //*Match me*//
      }
      else if (cmd == go_Left) {
        Serial.println(cmd);
        Left();
        recordings.write("3");  //*Match me*//

      }
      else if (cmd == go_Right) {
        Serial.println(cmd);
        Right();
        recordings.write("7");  //*Match me*//

      }
      else if (cmd == go_Backward) {
        Serial.println(cmd);
        Backward();
        recordings.write("1");  //*Match me*//
      }
      else if (cmd == 4) {
        recordings.close(); //close file after it's done; avoid re-entering Record function.
        Serial.println("DONE RECORDING!");
      }
    }
  }
}

void Analyze() {
  analysis = SD.open("record.txt", FILE_READ);
  char cmds{};                     //a variable to temporarily store one direction value from the recordings file
  uint8_t numcmds{0};                //number of commands recorded
  if (analysis) {
    Serial.println("Start Analyzing...");
    while ((cmds = analysis.read()) != -1) {
      Serial.print(cmds);                     //or Serial.println((int)cmds-48), or better, Serial.println(cmds-'0')
      numcmds++;                              //getting the number of commands recorded
    }
    analysis.close();                         //for some reason we need to close the file immediately before we perform any other tasks

    Serial.println("");
    Serial.print("Number of commands: ");
    Serial.println(numcmds);
    //size and populate the direction and distance arrays with raw data
    int cmd_list[numcmds] {};
    int dist_list[numcmds] {};
    uint8_t counter = 0;

    analysis = SD.open("record.txt", FILE_READ);    //reopen this file for further processing
    if (analysis) {
      Serial.println("Stuffing arrays...");

      while ((cmds = analysis.read()) != -1) {
        cmd_list[counter] = cmds - '0';
        //Serial.print(cmd_list[counter]);
        dist_list[counter] = turnTime;
        //Serial.println(dist_list[counter]);
        counter += 1;
      }
    }
    analysis.close();//done with file record.txt

    //CODE FOR OPTIMIZATION//
    Serial.println("Optimizing");
    //changing the recorded values to +1(Forward),-1(Backward),+2(Left),-2(Right) to match my C++ code
    for(int i=0;i<counter;i++){
      if(cmd_list[i]==go_Forward){
        cmd_list[i]=+1;}
        else if(cmd_list[i]==go_Left){
          cmd_list[i]=+2;}
          else if (cmd_list[i]==go_Right){
            cmd_list[i]=-2;}
            else if(cmd_list[i]==go_Backward){
              cmd_list[i]=-1;}
      }
    
    bool finish{false};
    while (!finish) {
      for (int i = 0; i < counter - 1; ++i) {
        if (cmd_list[i] == cmd_list[i + 1] && cmd_list[i] != 0) {
          finish = false;
          cmd_list[i + 1] = 0;
          dist_list[i] += dist_list[i + 1];
          dist_list[i + 1] = 0;
          pushZerosToEnd(cmd_list, counter);
          pushZerosToEnd(dist_list, counter);
          i--;
        }
        else {
          finish = true;
        }
      }
    }
    //xxxxxxxxxxxxxxxxxxxxx//

    analysis = SD.open("analysis.txt", FILE_WRITE);
    if (analysis) {
      Serial.println("WRITING analysis.txt");
      for (int i = 0; i < numcmds; i++) {
        analysis.print(cmd_list[i]);
        analysis.print("   "); //for debugging
        analysis.println(dist_list[i]);
      }
      analysis.close();
      Serial.println("DONE!");
      overall = false;
      delay(10000);
    }
  }
}





void loop() {
  while (overall) {
    Record();
    Analyze();
  }
  Serial.println(".................end of line.................");
  delay(5000);

}
