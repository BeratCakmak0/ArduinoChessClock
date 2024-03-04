#include <Arduino.h>
#include <TM1637Display.h>
#include <Debounce.h>

#define CLK1 5
#define DIO1 3
#define CLK2 9
#define DIO2 12

Debounce StartB(8,70);
Debounce RightB(7,40);
Debounce LeftB(6,40);
Debounce ModB(2,70);
Debounce DecreaseB(10,70);
Debounce IncreaseB(11,70);

#define LedR A2
#define LedL 4

int timer1,timer2 = 600;
int increasement = 3;
int mod;
unsigned long previousMillis;
bool blinkState,pressed;
  
unsigned long lastDebounceTime;
bool currentState,lastFlickerableState,lastSteadyState,debounceVal;

int counter,counter2,timerCounter;
int increasementSpeed = 300;
bool started,stateStartstate,state;
int BRIGHTNESS =5;

TM1637Display display1(CLK1, DIO1);
TM1637Display display2(CLK2, DIO2);

const uint8_t SEG_PLAY[] = {
  SEG_A | SEG_B | SEG_E | SEG_F | SEG_G,           // P
  SEG_D | SEG_E | SEG_F,                           // L
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G,   // A
  SEG_B | SEG_C | SEG_F | SEG_G                    // Y
  };
const uint8_t SEG_GG[] = {
  SEG_D,                                           // _
  SEG_A | SEG_C | SEG_D | SEG_E | SEG_F,           // G
  SEG_A | SEG_C | SEG_D | SEG_E | SEG_F,           // G
  SEG_D                                            // _
  };

const uint8_t INC[] = {
  SEG_B | SEG_C,                                   // I
  SEG_A | SEG_B | SEG_C | SEG_E | SEG_F,           // N
  SEG_A | SEG_F | SEG_E | SEG_D,                   // C
  SEG_G | SEG_D                                    // =
  };
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };
  
  const int ButtonP[6] = {8, 7, 6, 2, 10, 11};
  const int LedP[5] = {A2, 4, A4, A3, A5};
  
void setup(){
 Serial.begin(9600);
  for(int i=0; i<sizeof(ButtonP)/sizeof(ButtonP[0]); i++){
    pinMode(ButtonP[i], INPUT);
  }
  for(int i=0; i<sizeof(LedP)/sizeof(LedP[0]); i++){
    pinMode(LedP[i], OUTPUT);
  }
}
void loop() { 
  display1.setBrightness(BRIGHTNESS, true);  
  display2.setBrightness(BRIGHTNESS, true); 
  if(StartB.read()){
     started = 0;
  }
  while(StartB.read() == 0 && started == 0){
    if(millis() - previousMillis >= 700){
      previousMillis = millis();
      counter = 1;
      started = 1;
    }
  }
  while(counter == 1){
    Playing();
  }
  if(counter == 0){
    Setup();
  }
}
void Playing(){
      if(StartB.read()){
        started=0;
       }
      if(StartB.read() == 0 && started == 0){
          started = 1;
          while(1){
            if(StartB.read()){
             started=0;
            }
            if(StartB.read() == 0 && started == 0){
             started=1;
             break;
            }
          }
        }

   if(!RightB.read()==1){
        if(state!=0){
         timer2=timer2+increasement;
        }
      state = 0;
    }
    if(!LeftB.read()==1){
       if(state!=1){
         timer1=timer1+increasement;
        }
     state = 1;
    }
  
   if(state == 0 ){
      if(millis() - previousMillis >= 1000){
       previousMillis = millis();
       timer1 = timer1 - 1;
      }
       digitalWrite(LedL,HIGH);
       digitalWrite(LedR,LOW);
       display1.showNumberDecEx(Converter(timer1),0b01000000,true);
       display2.showNumberDecEx(Converter(timer2),0b01000000,true);
    }
    if(state==1){
     if(millis() - previousMillis >= 1000){
       previousMillis = millis();
       timer2 = timer2 - 1;
     }
       digitalWrite(LedR,HIGH);
       digitalWrite(LedL,LOW);
       display1.showNumberDecEx(Converter(timer1),0b01000000,true);
       display2.showNumberDecEx(Converter(timer2),0b01000000,true);
    }
    if(timer1 <= 0 || timer2 <= 0){
      while(1){
        if(millis() - previousMillis >= 700){
         previousMillis = millis();
         display1.setBrightness(BRIGHTNESS, blinkState);
         display2.setBrightness(BRIGHTNESS, blinkState);
         display1.setSegments(SEG_GG);
         display2.setSegments(SEG_GG);
         blinkState = !blinkState;
        }
      }
      
    }
  }
void Setup(){
  display1.setBrightness(BRIGHTNESS, true);  
  display2.setBrightness(BRIGHTNESS, true); 
    if(!ModB.read())
  {
    pressed = true;   
  }
  else if(pressed){
       mod = mod + 1;
        if(mod > 3)
        {
          mod = 1;
        }
      pressed = false;
  }
        switch (mod) {
  case 1://Timer Settings   
      if(!IncreaseB.read() && millis() - previousMillis >= increasementSpeed && timer1<9900){
          previousMillis = millis();
          timer1 += 5;
          if(timerCounter>=5){
            increasementSpeed = 70;
            Serial.println(timerCounter);
          }
          timerCounter++;
      }
      if(!DecreaseB.read() && millis() - previousMillis >= increasementSpeed && timer1>0){
          previousMillis = millis();
          timer1 -= 5;
          if(timerCounter>=5){
            increasementSpeed = 70;
            Serial.println(timerCounter);
          }
          timerCounter++;
      }
      if(DecreaseB.read()==1&&IncreaseB.read()==1){
        timerCounter=0;
        increasementSpeed=300;
      }
      timer2 = timer1;
      display1.showNumberDecEx(Converter(timer1),0b01000000,true);
      display2.showNumberDecEx(Converter(timer2),0b01000000,true);
    break;
  case 2://Increasement Settings
      if(!IncreaseB.read() && millis() - previousMillis >= 500 &&increasement<9999){
        previousMillis = millis();
        increasement++;
        display2.showNumberDec(increasement,true);
      }
      if(!DecreaseB.read() && millis() - previousMillis >= 500 && increasement>0){
        previousMillis = millis();
        increasement--;
        display2.showNumberDec(increasement,true);
      }
      display1.setSegments(INC);
      display2.showNumberDec(increasement,true);
    break;
   case 3: //Brightness Settings 
      if(!IncreaseB.read() && millis() - previousMillis >= 500 &&BRIGHTNESS<7){
        previousMillis = millis();
        BRIGHTNESS++;
      }
      if(!DecreaseB.read() && millis() - previousMillis >= 500 && BRIGHTNESS>0){
        previousMillis = millis();
        BRIGHTNESS--;
      }
    display1.setBrightness(BRIGHTNESS,true);
    display1.setSegments(data);
    display2.setBrightness(BRIGHTNESS,true);
    display2.showNumberDec(BRIGHTNESS,false,4);
   break;
  default:
      if(millis() - previousMillis >= 700){
      previousMillis = millis();
      display1.setBrightness(BRIGHTNESS, blinkState);
      display2.setBrightness(BRIGHTNESS, blinkState);
      display1.setSegments(SEG_PLAY);
      display2.setSegments(SEG_PLAY);
      blinkState = !blinkState;
    }
    break;
  }  
}
int Converter(int time_)
{ 
  int seconds = time_ % 60;
  int minutes = time_ / 60;
  int convertedMinutes = minutes * 100;
  return convertedMinutes + seconds;
}
