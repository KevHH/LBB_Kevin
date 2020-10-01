// pin constants
const int ledPin =  LED_BUILTIN;// the number of the LED pin
const int musicLedPin = 11;

// memory
char serialMemArray[1000];
long serialMemPointer = 0;


void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(musicLedPin, OUTPUT);
  Serial.begin(19200);
  digitalWrite(ledPin, HIGH);
}

void loop() {
  delay(100);
  // listen to Serial from Python
  if (Serial.available() > 0) {    
    char newChar = Serial.read();
    if (newChar != '\n'){
      // continue reading
      serialMemPointer += 1;
      serialMemArray[serialMemPointer] = newChar;
    } else {
      // finish reading
      String msgType = "";
      String pyMsg = "";
      String pyData = "";
      byte stage = 0; // 0: reading type; 1: reading message; 2: reading data
      bool validMsg = true;
      for (long i=0; i<serialMemPointer; i++){
        char tmpChar = serialMemArray[i];
        if (tmpChar == '&'){
          switch (stage){
            case 0:
              if (!msgType.equals("py")){
     digitalWrite(musicLedPin, HIGH);
     delay(500);
     digitalWrite(musicLedPin, LOW);
                Serial.println("ard&update_age&" + msgType);
                validMsg = false;
                break;
              }
            case 1:
            case 2:
              stage += 1;
              break;
          }
        } else {
          switch (stage){
            case 0:
              msgType += tmpChar;
            case 1:
              pyMsg += tmpChar;            
            case 2:
              pyData += tmpChar;   
          }
        }

        if (!validMsg){
          break;
        }
      }
      
      // process message and data
      if (validMsg){
        
     digitalWrite(musicLedPin, HIGH);
     delay(500);
     digitalWrite(musicLedPin, LOW);
        pyProcessing(pyMsg, pyData);
      }
      serialMemPointer = 0;
      delay(100);
    }
  }
}

void pyProcessing(String msg, String data){
  // process Python messages
  if (msg.equals("nudge")){
     digitalWrite(musicLedPin, HIGH);
     delay(500);
     digitalWrite(musicLedPin, LOW);
  } else if (msg.equals("whisper")){
     digitalWrite(musicLedPin, HIGH);
      delay(500);
     digitalWrite(musicLedPin, LOW);
      delay(500);
     digitalWrite(musicLedPin, HIGH);
      delay(500);
     digitalWrite(musicLedPin, LOW);
  }
}
