#include <SD.h>
#include <XBee.h>
#include <avr/pgmspace.h>


File recordings;
File analysis;
File execute;
const unsigned char chipSelect = 10;
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
Rx16Response rx16 = Rx16Response();

const unsigned char motor1F = 3;
const unsigned char motor1B = 5;
const unsigned char motor2F = 6;
const unsigned char motor2B = 9;
const unsigned char speed = 100;
unsigned int turnTime = 200; //motor turning time; subject to wheel specifications
//direction signals, depending on the controller setting
const unsigned char go_Forward = 1;
const unsigned char go_Backward = 4;
const unsigned char go_Left = 2;
const unsigned char go_Right = 3;
const unsigned char go_Stop = 6;

//unsigned long previous_Millis = 0; //initialize timer initial state
//unsigned long current_Millis = millis(); //initialize timer current state
//const unsigned char interval = 50; //set checking interval in millisecond

bool analyzed = false; //don't re-analyze previously analyzed data
const double modifier = 16 / 26.5; //modifier to correct the discrepancy between first run and recorded run due to acceleration and friction

void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

  //SD Card Setup Begin//
  Serial.print(F("Initializing SD card..."));
  pinMode(chipSelect, OUTPUT);

  while (!SD.begin(chipSelect)) { //keep trying to connect to SD card until success
    Serial.println(F("SD card initialization failed!"));
    delay(2000);
  }
  //if there exists an old analyzed file (analyzed.txt), skip the recording; otherwise,
  //remove the old recordings and start a new one.
  if (!SD.exists("analysis.txt")) {
    SD.remove("record.txt");
    Serial.println(F("Removed old record. Setting up recording environment."));
    recordings = SD.open("record.txt", FILE_WRITE);//having recordings open will lead the main program into Record() function
  }
  else {
    analyzed = true;
    Serial.println(F("analysis.txt exists."));
    Serial.println(F("Skip Recording and Analyze. Going to Execute."));
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

  Serial.println(F("Initialization done. Ready!"));
}

//Function to extract values from the optimized analysis.txt. Takes the direction and duration
//values separated by delimiter ',', and put the values in an array for execution
bool readLine(File &f, char* line, size_t maxLen) {
  for (size_t n = 0; n < maxLen; n++) {
    int c = f.read();
    if ( c < 0 && n == 0) return false;  // EOF
    if (c < 0 || c == '\n') {
      line[n] = 0;
      return true;
    }
    line[n] = c;
  }
  return false; // line too long
}

bool readVals(unsigned char* v1, unsigned char* v2) {
  char line[40], *ptr, *str;
  if (!readLine(execute, line, sizeof(line))) {
    return false;  // EOF or too long
  }
  *v1 = strtol(line, &ptr, 10);
  if (ptr == line) return false;  // bad number if equal
  while (*ptr) {
    if (*ptr++ == ',') break;
  }
  *v2 = strtol(ptr, &str, 10);
  return str != ptr;  // true if number found
}

//Function ends


//Move functions
void Forward(unsigned int duration) {
  analogWrite(motor1F, speed);
  analogWrite(motor2F, speed + 1);
  delay(duration);
  analogWrite(motor1F, LOW);
  analogWrite(motor2F, LOW);
  delay(500);
}

void Backward(unsigned int duration) {
  analogWrite(motor1B, speed);
  analogWrite(motor2B, speed + 1);
  delay(duration);
  analogWrite(motor1B, LOW);
  analogWrite(motor2B, LOW);
  delay(500);
}

void Left(unsigned int duration) {
  analogWrite(motor1F, speed);
  analogWrite(motor2B, speed + 1);
  delay(duration);
  analogWrite(motor1F, LOW);
  analogWrite(motor2B, LOW);
  delay(500);
}

void Right(unsigned int duration) {
  analogWrite(motor1B, speed);
  analogWrite(motor2F, speed + 1);
  delay(duration);
  analogWrite(motor1B, LOW);
  analogWrite(motor2F, LOW);
  delay(500);
}

void pushZerosToEnd(char arr[], int n)
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
        Forward(turnTime);
        recordings.write('1');  //*Match me*//
      }
      else if (cmd == go_Left) {
        Serial.println(cmd);
        Left(turnTime);
        recordings.write('2');  //*Match me*//

      }
      else if (cmd == go_Right) {
        Serial.println(cmd);
        Right(turnTime);
        recordings.write('3');  //*Match me*//

      }
      else if (cmd == go_Backward) {
        Serial.println(cmd);
        Backward(turnTime);
        recordings.write('4');  //*Match me*//
      }
      else if (cmd == go_Stop) {
        recordings.close(); //close file after it's done; avoid re-entering Record() function.
        Serial.println(F("DONE RECORDING!"));
      }
    }
  }
}

void Analyze() {
  analysis = SD.open("record.txt", FILE_READ);
  char cmds{};                     //a variable to temporarily store one direction value from the recordings file
  uint8_t numcmds{0};                //number of commands recorded
  if (analysis) {
    Serial.println(F("Start Analyzing..."));
    while ((cmds = analysis.read()) != -1) {
      Serial.print(cmds);                     //or Serial.println((int)cmds-48), or better, Serial.println(cmds-'0')
      numcmds++;                              //getting the number of commands recorded
    }
    analysis.close();                         //for some reason we need to close the file immediately before we perform any other tasks

    Serial.println("");
    Serial.print(F("Number of commands: "));
    Serial.println(numcmds);
    //size and populate the direction and distance arrays with raw data
    signed char cmd_list[numcmds] {};
    unsigned char dura_list[numcmds] {};
    uint8_t counter{0};
    //reopen this file for further processing
    analysis = SD.open("record.txt", FILE_READ);
    if (analysis) {
      Serial.println(F("Stuffing arrays..."));

      while ((cmds = analysis.read()) != -1) {
        cmd_list[counter] = cmds - '0';
        dura_list[counter] = 1; //this is essentially a multiplier, which will multiply turnTime later
        counter += 1;
      }
    }
    analysis.close();//done with file record.txt


    //changing the recorded values to +1(Forward),-1(Backward),+2(Left),-2(Right)
    //to recognize "opposite" direction commands
    for (int i = 0; i < counter; i++) {
      if (cmd_list[i] == go_Forward) {
        cmd_list[i] = +1;
      }
      else if (cmd_list[i] == go_Left) {
        cmd_list[i] = +2;
      }
      else if (cmd_list[i] == go_Right) {
        cmd_list[i] = -2;
      }
      else if (cmd_list[i] == go_Backward) {
        cmd_list[i] = -1;
      }
    }

    //CODE FOR OPTIMIZATION//
    Serial.println(F("Optimizing......"));
    bool finish{false};
    while (!finish) {
      //Same-direction optimization

      for (int i = 0; i < counter - 1; ++i) {
        if (cmd_list[i] == cmd_list[i + 1] && cmd_list[i] != 0) {
          finish = false;
          cmd_list[i + 1] = 0;
          dura_list[i] += dura_list[i + 1];
          dura_list[i + 1] = 0;
          pushZerosToEnd(cmd_list, counter);
          pushZerosToEnd(dura_list, counter);
          i--;
        }
        else {
          finish = true;
        }
      }
      //Opposite-direction optimization
      for (int j = 0; j < counter - 1; ++j) {
        while (cmd_list[j] == -cmd_list[j + 1] && cmd_list[j] != 0) {
          finish = false;
          if (dura_list[j] > dura_list[j + 1]) {
            dura_list[j] = dura_list[j] - dura_list[j + 1];
            cmd_list[j + 1] = 0;
            dura_list[j + 1] = 0;
            pushZerosToEnd(cmd_list, counter);
            pushZerosToEnd(dura_list, counter);
            j--;
          }
          else if (dura_list[j] < dura_list[j + 1]) {
            dura_list[j + 1] = dura_list[j + 1] - dura_list[j];
            cmd_list[j] = 0;
            dura_list[j] = 0;
            pushZerosToEnd(cmd_list, counter);
            pushZerosToEnd(dura_list, counter);
            j--;
          }
          else if (dura_list[j] == dura_list[j + 1]) {
            cmd_list[j] = 0;
            cmd_list[j + 1] = 0;
            dura_list[j] = 0;
            dura_list[j + 1] = 0;
            pushZerosToEnd(cmd_list, counter);
            pushZerosToEnd(dura_list, counter);
            j--;
          }

        }
        //else {
        finish = true;
        // }
      }
      if (counter == 1) {
        finish = true;
      }
    }
    //End of Optimization//

    //converting the direction commands back to original in order to write again
    for (int k = 0; k < counter; k++) {
      if (cmd_list[k] == +1) {
        cmd_list[k] = 1;//*Match me*//
      }
      else if (cmd_list[k] == -1) {
        cmd_list[k] = 4;//*Match me*//
      }
      else if (cmd_list[k] == +2) {
        cmd_list[k] = 2;//*Match me*//
      }
      else if (cmd_list[k] == -2) {
        cmd_list[k] = 3;//*Match me*//
      }
    }

    analysis = SD.open("analysis.txt", FILE_WRITE);
    if (analysis && !analyzed) {
      Serial.println(F("WRITING analysis.txt...."));
      for (int i = 0; i < numcmds && cmd_list[i] != 0; i++) {
        //write an cmd_list item followed immediately by its value
        //ignoring 0 values
        analysis.print(cmd_list[i]);
        analysis.print(',');
        analysis.println(dura_list[i]);
      }
      analysis.close();
      Serial.println(F("DONE!"));
      analyzed = true;
      delay(1000);
    }
  }
}

void Execute() {
  //modified from File_Read Arduino code//
  unsigned char x, y;
  execute = SD.open("analysis.txt", FILE_READ);
  if (execute) {
    while (readVals(&x, &y)) {
      if (x == go_Forward) {
        Serial.print(F("Forward for "));
        Serial.println(y * turnTime);
        Forward(y * turnTime * modifier);
      }
      else if (x == go_Backward) {

        Serial.print(F("Backward for "));
        Serial.println(y * turnTime);
        Backward(y * turnTime * modifier);
      }
      else if (x == go_Left) {

        Serial.print(F("Left for "));
        Serial.println(y * turnTime);
        Left(y * turnTime * modifier);
      }
      else if (x == go_Right) {
        Serial.print(F("Right for "));
        Serial.println(y * turnTime);
        Right(y * turnTime * modifier);
      }
    }
  }
  execute.close();
  Serial.println(F("DONE!!"));
}

void loop() {
  while (!analyzed) {
    Record();
    Analyze();
    delay(10000);
  }
  Execute();
  Serial.println(F(".................end of line................."));
  delay(10000);

}
