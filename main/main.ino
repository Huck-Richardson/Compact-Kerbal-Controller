#include <KerbalSimpit.h>
#include <LiquidCrystal.h>
#include <

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

LiquidCrystal lcd2(display2RPin, display2EPin, display2D0Pin, display2D1Pin, display2D2Pin, display2D3Pin);

const int inputPins[] = {sasPin, stagePin, gearPin, abortPin, rcsPin, brakesPin, lightPin, customPin, switchDisplay1Pin, switchDisplay2Pin, mapPin};

// Analog pins (joysticks)
const int joystick1XPin = 0;
const int joystick1YPin = 1;

const int joystick2XPin = 2;
const int joystick2YPin = 3;

byte currentActionStatus = 0;
const byte channels[] = {LF_STAGE_MESSAGE,
SF_STAGE_MESSAGE,
XENON_GAS_STAGE_MESSAGE,
ELECTRIC_MESSAGE,
VELOCITY_MESSAGE,
ALTITUDE_MESSAGE,
AIRSPEED_MESSAGE,
APSIDESTIME_MESSAGE,
ACTIONSTATUS_MESSAGE};
KerbalSimpit simpit(Serial);

void setup(){
    Serial.begin(115200);

    for(int i=0; i<sizeof(inputPins);i++){
        pinMode(inputPins[i],INPUT);
        pinMode(inputPins[i],INPUT_PULLUP);
    }

    lcd1.begin(16,2);
    lcd2.begin(16,2);

    while (!simpit.init()) {
    delay(100);
    }

    simpit.printToKSP("Connected", PRINT_TO_SCREEN);
    simpit.inboundHandler(messageHandler);
    for(int i = 0;i<sizeof(channels);i++){
        simpit.registerChannel(channels[i]);
    }
}
void loop(){
    simpit.update();
}
void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
  case ACTIONSTATUS_MESSAGE:
    // Checking if the message is the size we expect is a very basic
    // way to confirm if the message was received properly.
    if (msgSize == 1) {
      currentActionStatus = msg[0];

      //Let the LED_BUILIN match the current SAS state
      if(currentActionStatus & SAS_ACTION){
        digitalWrite(LED_BUILTIN, HIGH);
      } else {
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
    break;
  }
}
