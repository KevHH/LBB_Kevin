// pin constants
const int sysLEDPin =  LED_BUILTIN;
const int redLEDPin = A0;
const int blueLEDPin = A1;
const int rSwitchPin = A2;
const int rBwdAPin = A3;
const int rFwdAPin = A4;
const int lSwitchPin = A5;
const int lLDRPin = A6;
const int rLDRPin = A7;
const int greenLEDPin = 12;
const int yellowLEDPin = 11;
const int lEnaPin = 10;
const int fanPin = 9;
const int lBwdAPin = 8;
const int lFwdAPin = 7;
const int rBwdSPin = 6;
const int rEnaPin = 5;
const int rFwdSPin = 4;
const int lBwdSPin = 3;
const int lFwdSPin = 2;

byte lSwitch = LOW;
byte rSwitch = LOW;

// memory
char serialMemArray[100];
long serialMemPointer = 0;

int lMotOp = 0;
int lMotFreq = 0;
int lMotPrevTime = 0;
int lMotInterval = 0;

int rMotOp = 0;
int rMotFreq = 0;
int rMotPrevTime = 0;
int rMotInterval = 0;

void setup() {
  pinMode(sysLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
  pinMode(rBwdAPin, OUTPUT);
  pinMode(rFwdAPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(yellowLEDPin, OUTPUT);
  pinMode(lEnaPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(lBwdAPin, OUTPUT);
  pinMode(lFwdAPin, OUTPUT);
  pinMode(rEnaPin, OUTPUT);
  Serial.begin(19200);
}

void loop() {
  
  
  // listen to Serial from Python
  if (Serial.available() > 0) {    
    char newChar = Serial.read();
    if (newChar != '\n'){
      // continue reading
      serialMemPointer += 1;
      serialMemArray[serialMemPointer-1] = newChar;
    Serial.println("ard&GOT&" + String(serialMemArray) + " " + String(serialMemPointer));
    } else {
      // finish reading
      String msgType = "";
      String pyMsg = "";
      String pyData = "";
      String pyTime = "";
      byte stage = 0; // 0: reading type; 1: reading message; 2: reading data; 3: reading time
      bool validMsg = true;

      for (long i=0; i<serialMemPointer; i++){    
  Serial.println("ard&PROCESS&(" + String(msgType) + "," + String(pyMsg) + "," + String(pyData) + "," + String(pyTime) + ")");    
        char tmpChar = serialMemArray[i];
        if (tmpChar == '&'){
          switch (stage){
            case 0:
              if (!msgType.equals("py")){
                validMsg = false;
              }
            case 1:
            case 2:
              stage += 1;
              break;
          }
        } else {
          switch (stage){
            case 0:
              msgType = msgType + tmpChar;
              break;
            case 1:
              pyMsg = pyMsg + tmpChar; 
              break;           
            case 2:
              pyData = pyData + tmpChar; 
              break;           
            case 3:
              pyTime = pyTime + tmpChar; 
              break;    
          }
        }

        if (!validMsg){
          break;
        }
      }
      
      // process message and data
      if (validMsg){
        int pyTimeInt = 0;        
        if (!pyTime.equals("")){
          pyTimeInt = pyTime.toInt();
        }
        pyProcessing(pyMsg, pyData, pyTimeInt);
      }
      serialMemPointer = 0;
    }
  }
/*
  // print status to serial
  byte newLSwitch = digitalRead(lSwitchPin);
  if (newLSwitch != lSwitch){
    lSwitch = newLSwitch;
    Serial.println("ard&lSwitch&" + String(newLSwitch));
  }
  byte newRSwitch = digitalRead(rSwitchPin);
  if (newRSwitch != rSwitch){
    rSwitch = newRSwitch;
    Serial.println("ard&rSwitch&" + String(newRSwitch));
  }

  byte lFwdS = digitalRead(lFwdSPin);
  byte lBwdS = digitalRead(lBwdSPin);
  Serial.println("ard&lMot&" + String(lFwdS) + "," + String(lBwdS));
  
  byte rFwdS = digitalRead(rFwdSPin);
  byte rBwdS = digitalRead(rBwdSPin);
  Serial.println("ard&rMot&" + String(rFwdS) + "," + String(rBwdS));

  int lLDR = analogRead(lLDRPin);
  Serial.println("ard&lLDR&" + String(lLDR));
  int rLDR = digitalRead(rLDRPin);
  Serial.println("ard&rLDR&" + String(rLDR));*/

  delay(200);
}

void pyProcessing(String msg, String data, int timeInt){
  Serial.println("ard&PROCESS&(" + String(msg) + "," + String(data) + "," + String(timeInt) + ")");
  // process Python messages
  if (msg.equals("nudge")){
     rWheel(-1, 150);
     lWheel(1, 150);
     delay(200);
     rWheel(1, 150);
     lWheel(-1, 150);
     delay(200);
     rWheel(-1, 150);
     lWheel(1, 150);
     delay(200);
     rWheel(1, 150);
     lWheel(-1, 150);
     delay(200);
     rWheel(0, 0);
     lWheel(0, 0);
  } else if (msg.equals("whisper")){
     rWheel(0, 0);
     lWheel(0, 0);
     digitalWrite(redLEDPin, HIGH);
     digitalWrite(yellowLEDPin, HIGH);
     digitalWrite(blueLEDPin, LOW);
     digitalWrite(greenLEDPin, LOW);
     delay(200);
     digitalWrite(redLEDPin, LOW);
     digitalWrite(yellowLEDPin, LOW);
     digitalWrite(blueLEDPin, HIGH);
     digitalWrite(greenLEDPin, HIGH);
     delay(200);
     digitalWrite(redLEDPin, HIGH);
     digitalWrite(yellowLEDPin, HIGH);
     digitalWrite(blueLEDPin, LOW);
     digitalWrite(greenLEDPin, LOW);
     delay(200);
     digitalWrite(redLEDPin, LOW);
     digitalWrite(yellowLEDPin, LOW);
     digitalWrite(blueLEDPin, HIGH);
     digitalWrite(greenLEDPin, HIGH);
     delay(200);
     digitalWrite(redLEDPin, LOW);
     digitalWrite(yellowLEDPin, LOW);
     digitalWrite(blueLEDPin,  LOW);
     digitalWrite(greenLEDPin, LOW);
  } else if (msg.equals("fan")){
    analogWrite(fanPin, data.toInt());
  } else if (msg.equals("wheel")) {
    int op = 0;
    if (data[1] == 'f') {
      op = 1;
    } else if (data[1] == 'b') {
      op = -1;
    }
    if (data[0] == 'l'){
      lWheel(op, data.substring(2).toInt());
    } else if (data[0] == 'r'){
      rWheel(op, data.substring(2).toInt());
    }
  } else if (msg.equals("LED")) {
    int state = -1;
    if (data[0] == 'L'){
      state = LOW;
    } else if (data[0] == 'H'){
      state = HIGH;
    }
    if (state != -1){
      if (data[1] == 'r') {
        digitalWrite(redLEDPin, state);
      } else if (data[1] == 'b') {
        digitalWrite(blueLEDPin, state);
      } else if (data[1] == 'y') {
        digitalWrite(yellowLEDPin, state);
      } else if (data[1] == 'g') {
        digitalWrite(greenLEDPin, state);
      }
    }
  }
}

void rWheel(int op, int freq){
  switch (op){
    case -1:
      analogWrite(rEnaPin, freq);
      digitalWrite(rFwdAPin, LOW); 
      digitalWrite(rBwdAPin, HIGH);
      break;
    case 0:
      analogWrite(rEnaPin, 0);
      digitalWrite(rFwdAPin, LOW); 
      digitalWrite(rBwdAPin, LOW);
      break;
    case 1:
      analogWrite(rEnaPin, freq);
      digitalWrite(rFwdAPin, HIGH); 
      digitalWrite(rBwdAPin, LOW);
      break;
  }
}

void lWheel(int op, int freq){
  switch (op){
    case -1:
      analogWrite(lEnaPin, freq);
      digitalWrite(lFwdAPin, LOW); 
      digitalWrite(lBwdAPin, HIGH);
      break;
    case 0:
      analogWrite(lEnaPin, 0);
      digitalWrite(lFwdAPin, LOW); 
      digitalWrite(lBwdAPin, LOW);
      break;
    case 1:
      analogWrite(lEnaPin, freq);
      digitalWrite(lFwdAPin, HIGH); 
      digitalWrite(lBwdAPin, LOW);
      break;
  }
}
