#include <KerbalSimpit.h>
#include <LiquidCrystal.h>

// Digital pins
const int sasPin = 22;
const int stagePin = 23;
const int gearPin = 24;
const int abortPin = 25;
const int rcsPin = 26;
const int brakesPin = 27;
const int lightPin = 28;
const int customPin = 29;
const int switchDisplay1Pin = 30;
const int switchDisplay2Pin = 31;
const int mapPin = 44;

// Display 1 (D0–D3)
const int display1D0Pin = 32;
const int display1D1Pin = 33;
const int display1D2Pin = 34;
const int display1D3Pin = 35;

const int display1RPin = 36;
const int display1EPin = 37;

LiquidCrystal lcd1(display1RPin,display1EPin,display1D0Pin,display1D1Pin,display1D2Pin,display1D3Pin);

// Display 2 (D0–D3)
const int display2D0Pin = 38;
const int display2D1Pin = 39;
const int display2D2Pin = 40;
const int display2D3Pin = 41;

const int display2RPin = 42;
const int display2EPin = 43;

LiquidCrystal lcd2(display2RegisterPin, display2EnablePin, display2D0Pin, display2D1Pin, display2D2Pin, display2D3Pin);

const int inputPins[] = {sasPin, stagePin, gearPin, abortPin, rcsPin, brakesPin, lightPin, customPin, switchDisplay1Pin, switchDisplay2Pin, mapPin};

// Analog pins (joysticks)
const int joystick1XPin = 0;
const int joystick1YPin = 1;

const int joystick2XPin = 2;
const int joystick2YPin = 3;

KerbalSimpit simpit(Serial);

void setup(){
    Serial.begin(115200);

    for(int i=0, i<sizeof(inputPins),i++){
        pinMode(inputPins[i],INPUT);
        pinMode(inputPins[i],INPUT_PULLUP);
    }
    for(int i=0, i<sizeof(inputPins),i++){
        pinMode(inputPins[i],OUTPUT);
        pinMode(inputPins[i],INPUT_PULLUP);
    }

    lcd1.begin(16,2);
    lcd2,begin(16,2);
}