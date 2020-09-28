/*

  Test code by Kevin to play musical tones

*/


// helper functions
#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0])

// overall control
bool botOn = false;
int ledState = LOW;

// Pin constants
const int buttonPin = 12; // D12 pin connecting to the button
const int speakersPin[] = {8, 9, 7}; // D10, D4, D7 pins
const int ledPin =  LED_BUILTIN;// the number of the LED pin

const int ldrLPin = A6;
const int ldrRPin = A7;

const int musicLedPin = 11; // D11 pin connecting to the pin for music

const int rMotEnaPin = 6; // D6 pin connecting to right motor
const int rMotEncBacPin = A1; // A1 pin connecting to the back encoder of the right motor
const int rMotEncFrtPin = A2; // A2 pin connecting to the front encoder of the right motor
const int rMotYBacPin = A0; // A0 pin connecting to the back Y pin of the right motor
const int rMotYFrtPin = A3; // A3 pin connecting to the front Y pin of the right motor

const int fanEnaPin = 5; // D5 pin connecting to fan 
const int fanYClockPin = A4; // A4 pin connecting to clockwise Y pin of fan
const int fanYAntiCPin = A5; // A5 pin connecting to anticlockwise Y pin of fan


// Bot constants
const int rMotPwmFreq = 255;
const long turnsPerRotation = 5148; 
const long minTimePerRotation = 12000000; // in microseconds, at full speed, ie pwm 255
 
const int fanPwmFreq = 100;
// Music related constants and variables
const float musicFreq[] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88};
const float musicSharpFreq[] = {277.18, 311.13, 0, 369.99, 415.3, 466.16, 0};

long speakersBeatLength[] = {minTimePerRotation / 192, minTimePerRotation / 192, minTimePerRotation / 192};
int speakersOctave[] = {2, 4, 4};
const char *GoTSheet[] = { 
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", 
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "El2", "F", "Gl3", "Cl3", "El2", "F", 
                            "Gl3", "Cl3", "El2", "F", "Gl3", "Cl3", "El2", "F", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18", 
                            "Fl9", "A#o-l9", "D#l2", "D", "Fl6", "l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18", "Fl9", "l9", "D#l2", "D", "Fl6", "l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18",
                            "Fl9", "l9", "Dl5", "D#l4", "Dl5", "l4", "Cl18", "l18",  
                            "Co+l18", "A#l18", "Cl18", "Gl18", "D#l18", "D#l9", "Fl9", "Gl18", "l18",
                            "Co+l18", "A#l18", "Cl18", "Gl18", "D#l18", "D#l9", "Dl9", 
                            "Co+l9", "Co+l9", "Co+l9",
                            "Co+l18",  
                            "Co++l9", "Co++l9", "Co++l18"
                          }; 
   /*                       
const char *GoTSheetR[] = { 
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "D#l2", "F", 
                            "Gl3", "Cl3", "D#l2", "F", "Gl3", "Cl3", "El2", "F", "Gl3", "Cl3", "El2", "F", 
                            "Gl3", "Cl3", "El2", "F", "Gl3", "Cl3", "El2", "F", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18", 
                            "l9", "A#o-l9", "l9", "A#o-l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18", "l9", "A#o-l9", "D#l2", "D", "Fl6", "A#o-l9", "D#l2", "D", "Cl33", 
                            "Gl9", "Cl9", "D#l2", "F", "Gl6", "Cl6", "D#l2", "F", "Dl18", "l18",
                            "Fl9", "A#o-l9", "Dl5", "D#l4", "l5", "A#o-l4", "Cl18", "l18",  
                            "D#l18", "D#l18", "G#o-l18", "Cl18", "Cl18", "Cl9", "Dl9", "D#l18", "l18", 
                            "D#l18", "D#l18", "G#o-l18", "Cl18", "Cl18", "Cl9", "Ao-Dl9", 
                            "l3", "Gl3", "G#l2", "A#", "l3", "Gl3", "G#l2", "A#", "l3", "Gl3", "G#l2", "A#",
                            "l3", "Gl3", "G#l2", "A#", "l6", "Go+l2", "Ao+",
                            "l3", "Gl3", "G#l2", "A#", "l3", "Gl3", "G#l2", "A#", "l3", "Gl3", "G#l2", "A#", "l3" 
                            "Co+l3", "Gl3", "G#l2", "A#l7", "G#o+l2", "A#o+", 
                            "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#", "Co+l3", "Gl3", "G#l2", "A#l5",
                            "Co++l3", "Go+l3", "G#o+l2", "A#o+", "Co++l3", "Go+l3", "G#o+l2", "A#o+", "Co++l3", "Go+l3", "G#o+l2", "A#o+l11"                        
                          };
const String GoTBeat = "Cl9";
*/
bool rMotMoveFwd = true;
bool fanAntiCFwd = true;


// Speaker controls: 0 - buzzer, 1 - left speaker, 2 - right speaker
unsigned long speakersPrevTime[] = {0, 0, 0};
unsigned long speakersPeriod[] = {0, 0, 0};
long speakersCountsLeft[] = {0, 0, 0};
float speakersLag[] = {0, 0, 0};
byte speakersStatus[] = {0, 0, 0}; // 0 means about to play next tone; 1 means playing next tone; 2 means lag 
unsigned long speakersSheetIndex[] = {0, 0, 0};

// helper variables
String tmpStr="";  
unsigned long currentTime = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  
  pinMode(musicLedPin, OUTPUT);

  pinMode(speakersPin[0], OUTPUT);
  pinMode(speakersPin[1], OUTPUT);
  pinMode(speakersPin[2], OUTPUT);


  pinMode(fanEnaPin, OUTPUT);
  pinMode(fanYClockPin, OUTPUT);
  pinMode(fanYAntiCPin, OUTPUT);

  pinMode(rMotEnaPin, OUTPUT);
  pinMode(rMotYFrtPin, OUTPUT);
  pinMode(rMotYBacPin, OUTPUT);
  Serial.begin(9600);
}

unsigned long getSheetLength(byte s){
  switch (s){
    case 0:
         return sizeof(GoTSheet);
    case 1:
        return sizeof(GoTSheet);
    case 2: 
        return sizeof(GoTSheet);
  }
}

String getSheetNote(byte s, unsigned long k){
  switch (s){
    case 0:
         return GoTSheet[k];
    case 1:
        return GoTSheet[k];
    case 2: 
        return GoTSheet[k];
  }
}

void loop() {
  /*
  int ldrLValue = analogRead(ldrLPin); // do something when it's below 1000
  int ldrRValue = analogRead(ldrRPin);
  */
  
  // switch for the bot
  int button = digitalRead(buttonPin);
  if (button == HIGH){
    // turn relevant variables off
    toggleBool(botOn);
    toggleState(ledState);
    digitalWrite(ledPin, ledState);
    botReset();
  }

  if (botOn){   
    currentTime = micros();
    bool noActiveSpeakers = true;
    for (byte s = 0; s < 3; s++){
      // handles music
      switch (speakersStatus[s]){
        case 0: // starting next tone when sheet is not finished
          if (speakersSheetIndex[s] < getSheetLength(s)){
            noActiveSpeakers = false;

            // render note
            renderToneFromString(getSheetNote(s, speakersSheetIndex[s]), s); 

            speakersPrevTime[s] = micros();
            speakersStatus[s] = 1;

            // motor and fan action
            motAndFanGo();
             
           speakersSheetIndex[s] += 1;
          }

          break;
        case 1: // playing tones;
          noActiveSpeakers = false;          
        
          if (speakersCountsLeft[s] <= 0){
            // change state
            speakersStatus[s] = 2;
            motAndFanPause();
          } else {
            if (currentTime - speakersPrevTime[s] >= speakersPeriod[s]){
              analogWrite(speakersPin[s], 0);
              // reset
              speakersPrevTime[s] = micros();
              speakersCountsLeft[s] -= 1;
            } else if (currentTime - speakersPrevTime[s] >= speakersPeriod[s] / 2) {
              analogWrite(speakersPin[s], 50);
            }            
          }
            
          break;
        case 2: // lag
          noActiveSpeakers = false;
          if (currentTime - speakersPrevTime[s] >= speakersLag[s]){
            // change state
            speakersStatus[s] = 0;
          }
          break;
      }
    }
    
    if (noActiveSpeakers) {
      botOn = false;
      ledState = LOW;
      digitalWrite(ledPin, ledState);
      
      botReset();
    }
  } else {
    botReset();
  } 
}

void motAndFanGo(){
  digitalWrite(musicLedPin, HIGH);

  // rotate wheel      
  analogWrite(rMotEnaPin, rMotPwmFreq);
  if (rMotMoveFwd){
    digitalWrite(rMotYFrtPin, HIGH); 
    digitalWrite(rMotYBacPin, LOW);
  } else {
    digitalWrite(rMotYFrtPin, LOW); 
    digitalWrite(rMotYBacPin, HIGH);
  }
  toggleBool(rMotMoveFwd);

  // rotate fan 
  /*
  analogWrite(fanEnaPin, fanPwmFreq);
  if (fanAntiCFwd){
    digitalWrite(fanYAntiCPin, HIGH); 
    digitalWrite(fanYClockPin, LOW);
  } else {
    digitalWrite(fanYAntiCPin, LOW); 
    digitalWrite(fanYClockPin, HIGH);
  }
  toggleBool(fanAntiCFwd);
  */ 
}

void motAndFanPause(){
  digitalWrite(musicLedPin, LOW);
    
  analogWrite(rMotEnaPin, 0);
  digitalWrite(rMotYFrtPin, LOW);
  digitalWrite(rMotYBacPin, LOW);

  analogWrite(fanEnaPin, 0);
  digitalWrite(fanYAntiCPin, LOW);
  digitalWrite(fanYClockPin, LOW);
    
}

void botReset(){
  motAndFanPause();

  for (byte s=0; s<3; s++){
    speakersPrevTime[s] = 0;
    speakersPeriod[s] = 0;
    speakersCountsLeft[s] = 0;
    speakersLag[s] = 0;
    speakersStatus[s] = 0;
  }

  // buffer time to decrease sensitivity of the button
  delay(200);
  for (byte s=0; s<3; s++){
    speakersPrevTime[s] = micros();
    speakersSheetIndex[s] = 0;
  }
}

void renderToneFromString(String toneString, byte s){
  int musicTone = 0;
  bool sharpOn = false;
  int octave = speakersOctave[s];
  float beatUnits = 1;

  Serial.println(toneString);
  // decoding
  long i=0;
  long startIndex=0;
  while (i < toneString.length()){
    char currentChar = toneString[i];
    switch (currentChar){
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
        musicTone = currentChar - 66;
        break;        
      case 'A':
      case 'B':
        musicTone = currentChar - 59;
        break;
      case '#':
        sharpOn = true;
        break;
      case 'l':
        i++;
        startIndex=i;
        while (i < toneString.length() && toneString[i] != ' '){
          i++;
        }
        beatUnits = toneString.substring(startIndex, i).toFloat(); 
        break;
      case 'o':
        i++;
        startIndex = i;
        while (i < toneString.length() && toneString[i] != ' '){
          if (toneString[i] == '-'){
            octave -= 1;
          } else if (toneString[i] == '+'){
            octave += 1;
          }
          i++;
        }  
        break;
    }
    i++;
  }

  float refFreq = 0;
  if (musicTone >=1 && musicTone <=7){
    // calculate frequency
    if (!sharpOn || musicSharpFreq[musicTone-1] == 0){
      refFreq = musicFreq[musicTone-1] * pow(2, octave - 4);
    } else {
      refFreq = musicSharpFreq[musicTone-1] * pow(2, octave - 4);
    }
    speakersPeriod[s] = round(1000000.0 / refFreq);
    speakersCountsLeft[s] = round(speakersBeatLength[s] * beatUnits) / speakersPeriod[s];
  } else {
    speakersPeriod[s] = round(speakersBeatLength[s] * beatUnits);
    speakersCountsLeft[s] = 1;
  }
  speakersLag[s] = speakersBeatLength[s] * beatUnits * 2 - speakersPeriod[s] * speakersCountsLeft[s]; // include any remainder from playing tone to sync time
}

// ==================
// Helper functions
// ==================


void toggleBool(bool &boolvar){
  if (boolvar){
    boolvar = false;
  } else {
    boolvar = true;
  }
}

void toggleState(int &state){
  if (state == LOW){
    state = HIGH;
  } else {
    state = LOW;
  }
}
