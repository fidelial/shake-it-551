#include <NewhavenLCDSerial.h>

#include <Adafruit_NeoPixel.h>
#include <SM.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6

Adafruit_NeoPixel strip = Adafruit_NeoPixel(10, PIN, NEO_GRB + NEO_KHZ800);
NewhavenLCDSerial lcd(12); // initialize LCD display using pin 12
SM gameState(S1);

int z1, z2;
int buttonState1 = 0;
int buttonState2 = 0;
bool fire1 = true;
bool fire2 = true;
int button1Count = 0;
int button2Count = 0;
int p1Count = 0;
int p2Count = 0;
int lightState1 = 0; // <= 5
int lightState2 = 10; // >= 5

int p1Score = 0;
int p2Score = 0;

int printCount = 0;
const int buttonPin1 = 2;
const int buttonPin2 = 4;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

  strip.begin();
  strip.show();
}

void loop() {
  EXEC(gameState);
}

State S1() {
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);  
//  Serial.print(buttonState1);
//  Serial.print(buttonState2);
//  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
//    for (int q=0; q < 3; q++) {
//      for (int i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
//      }
//      strip.show();
//
//      delay(50);
//
//      for (int i=0; i < strip.numPixels(); i=i+3) {
//        strip.setPixelColor(i+q, 0);        //turn every third pixel off
//      }
//    }
//  }
  if (buttonState1 == HIGH || buttonState2 == HIGH) {
    button1Count++;
    button2Count++;
  }
  if (button1Count != 0 || button2Count != 0) {
    gameState.Set(S2H);
    lcd.clear();
    button1Count = 0;
    button2Count = 0;
  }
}

State S2H() {
   for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0, 0, 255);
   }
    strip.show();
    delay(500);
   for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
   }
    strip.show();

   gameState.Set(S2B);
    
}

State S2B() {
    buttonState1 = digitalRead(buttonPin1);
    buttonState2 = digitalRead(buttonPin2);  
    z1 = map(analogRead(0), 0, 1023, 0, 255);
    z2 = map(analogRead(2), 0, 1023, 0, 255); 

//    Serial.print("PlayerOne: ");
//    Serial.println(z1);
//    Serial.print(" ");
//    Serial.println(buttonState1);
//    Serial.println();
//    Serial.print("PlayerTwo: ");
//    Serial.println(z2);
//    Serial.print(" ");
//    Serial.println(buttonState2);
//    Serial.println();
    delay(100);
  
  if (z1 >= 160) {
    p1Count++;
//    Serial.print("p1Count is: ");
//    Serial.println(p1Count);
  }

  if (p1Count == 5) {
    lightState1++;
    p1Count = 0;
  }
  
  if (lightState1 < 6) {
    for (int i = 0; i < lightState1; i++) {
      strip.setPixelColor(i, strip.Color(0, 150, 0));
      strip.show();
    }
  }

  if (lightState1 == 5 && buttonState1 == HIGH) {
    for (int i = 11; i >= lightState2; i--) { 
      strip.setPixelColor(i, 0);
    }
    lightState2 = 10;
    for (int i = 0; i < lightState1; i++) {
      strip.setPixelColor(i, 0);
      strip.show();
    }
    lightState1 = 0;
    p1Score++;
  }
  if (z2 >= 160) {
     p2Count++;
//     Serial.print("p2Count is: ");
//     Serial.println(p2Count);
  }

  if (p2Count == 5) {
    lightState2--;
    p2Count = 0;
    Serial.print("lightState2 is: ");
    Serial.println(lightState2);
  } 
  if (lightState2 >= 5) {
    for (int i = 10; i >= lightState2; i--) {
      strip.setPixelColor(i, strip.Color(230, 25, 55));
      strip.show();
    }
  }
  if (lightState2 <= 5 && buttonState2 == HIGH) {
    for (int i = 0; i <= lightState1; i++) { 
      strip.setPixelColor(i, 0);
    }
    lightState1 = 0;
    for (int i = 10; i >= lightState2; i--) {
      strip.setPixelColor(i, 0);
      strip.show();
    }
    lightState2 = 10;
    p2Score++;
  }
//  Serial.print("GameTime is: ");
//  Serial.println(gameState.Statetime());

  if (gameState.Statetime() >= 60000) {
   for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 255, 0, 0);
   }
    strip.show();
    delay(500);
   for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
   }
    strip.show();

    gameState.Set(S3);
  }
}

State S3(){
  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  if (buttonState1 == HIGH || buttonState2 == HIGH) {
    gameState.Set(S1);
  }
//  Serial.println(buttonState1);
//  Serial.println(buttonState2);

    if (p1Score > p2Score) {
      //Serial.println("P1 WINS!");
      lcd.setCursor(0, 3);
      lcd.print("FINAL SCORE:");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.print("P1: ");
      lcd.setCursor(0, 7);
      lcd.print(p1Score);
      lcd.setCursor(0, 9);
      lcd.print("P2: ");
      lcd.setCursor(0, 12);
      lcd.print(p2Score);
      lcd.setCursor(1, 3);
      lcd.print("P1 WINS!");
    }
    else if (p2Score > p1Score) {
      //Serial.println("P2 WINS!");
      lcd.setCursor(0, 2);
      lcd.print("FINAL SCORE:");
      delay(1500);
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.print("P1: ");
      lcd.setCursor(0, 7);
      lcd.print(p1Score);
      lcd.setCursor(0, 9);
      lcd.print("P2: ");
      lcd.setCursor(0, 12);
      lcd.print(p2Score);
      lcd.setCursor(1, 3);
      lcd.print("P2 WINS!");
    }
    else if (p1Score == p2Score) {
      //Serial.print("IT'S A TIE!");
      lcd.setCursor(0, 2);
      lcd.print("FINAL SCORE:");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 3);
      lcd.print("P1: ");
      lcd.setCursor(0, 7);
      lcd.print(p1Score);
      lcd.setCursor(0, 9);
      lcd.print("P2: ");
      lcd.setCursor(0, 12);
      lcd.print(p2Score);
      lcd.setCursor(1, 3);
      lcd.print("IT'S A TIE!");
    }
  if (p1Score > p2Score) {
    for(int j=0; j<256; j++) {
      for(int i=0; i < 5; i++) {
        strip.setPixelColor(i, Wheel((i+j) & 255));
      }
      strip.show();
      delay(50);
    }
  }
  else if (p2Score > p1Score) {
     for(int j=0; j<256; j++) {
        for(int i=10; i >= 5; i--) {
          strip.setPixelColor(i, Wheel((i+j) & 255));
        }
        for (int i = 10; i <= 5; i--) { 
          strip.setPixelColor(i, 0);
        }
        strip.show();
        delay(50);
      }
  }
  else if (p1Score == p2Score) {
      for(int j=0; j<256; j++) {
        for(int i=0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, Wheel((i+j) & 255));
         }
        strip.show();
        delay(50);
      }
   }
   if (gameState.Statetime() >= 15000) {
    for(int i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
      strip.show();
      lcd.clear();
   }
   gameState.Set(S1);
   
}
}


uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
