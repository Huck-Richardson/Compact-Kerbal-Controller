#include <KerbalSimpit.h>
#include <LiquidCrystal.h>
#include <ezButton.h>

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
ezButton buttons[] = {
  ezButton(sasPin), 
  ezButton(stagePin), 
  ezButton(gearPin),
  ezButton(abortPin), 
  ezButton(rcsPin), 
  ezButton(brakesPin),
  ezButton(lightPin),
  ezButton(customPin),
  ezButton(switchDisplay1Pin),
  ezButton(switchDisplay2Pin),
  ezButton(mapPin)
};
const byte buttonActions[] = {SAS_ACTION,STAGE_ACTION,GEAR_ACTION,ABORT_ACTION,RCS_ACTION,BRAKES_ACTION,LIGHT_ACTION,};
// Analog pins (joysticks)
const int joystick1XPin = 0;
const int joystick1YPin = 1;

const int joystick2XPin = 2;
const int joystick2YPin = 3;

const byte channels[] = {ACTIONSTATUS_MESSAGE,
  ATMO_CONDITIONS_MESSAGE,
  DELTAV_MESSAGE,
  ELECTRIC_MESSAGE,
  VELOCITY_MESSAGE,
  ALTITUDE_MESSAGE,
  AIRSPEED_MESSAGE,
  APSIDESTIME_MESSAGE,
};

int numPages = 6;
LiquidCrystal* lcds[] = { &lcd1, &lcd2 };
int currentPages[] = {0,3};

float deltaV = 0;
float eCharge = 0;
float velocity = 0;
float altitude = 0;
float airspeed = 0;
float apTime = 0;
bool inAtmosphere = true;

KerbalSimpit simpit(Serial);

void setup(){
    Serial.begin(115200);

    for (int i = 0; i < sizeof(lcds); i++) {
    lcds[i]->begin(16, 2);
  }

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

  //Toggles the buttons that simpit has action groups for
    for(int i = 0;i<sizeof(buttonActions);i++){
      if(buttons[i].isPressed()){
        simpit.toggleAction(buttonActions[i]);
      }
    }
    if(buttons[7].isPressed()){
      simpit.toggleCAG(1);
    }
    if(buttons[8].isPressed()){
      currentPages[0]++;
      if(currentPages[0] >= pages){
        currentPages[0] = 0;
      }
    }if(buttons[9].isPressed()){
      currentPages[1]++;
      if(currentPages[1] >= pages){
        currentPages[1] = 0;
      }
    }

}
void printToLcd(LiquidCrystal &lcd, const char* label, float value) {
  lcds[index]->setCursor(0, 0);
  lcds[index]->print(label);
  lcds[index]->setCursor(0, 1);
  lcds[index]->print(value);
}

void updateDisplays(){
  for(int i=0;i<sizeof(lcds);i++){
    if(currentPages[i]==0){
      printToLcd(lcds[i],"Delta-V",deltaV);
    }
  }
}

void messageHandler(byte messageType, byte msg[], byte msgSize) {
  switch(messageType) {
  case DELTAV_MESSAGE:
    if(msgSize == sizeof(deltaVMessage)){
      deltaVMessage myDeltaV = parseMessage<deltaVMessage>(msg);
      deltaV = myDeltaV.stageDeltaV;
    }
    break;
  case ELECTRIC_MESSAGE:
    if(msgSize == sizeof(resourceMessage)){
      resourceMessage myCharge = parseMessage<resourceMessage>(msg);
      eCharge = myCharge.total;
    }
    break;
  case ATMO_CONDITIONS_MESSAGE:
    if(msgSize == sizeof(atmoConditionsMessage)){
      atmoConditionsMessage atmo = parseMessage<atmoConditionsMessage>(msg);
      inAtmosphere = atmo.isVesselInAtmosphere();
    }
  case VELOCITY_MESSAGE:
    if(msgSize == sizeof(velocityMessage)){
      velocityMessage myVelocity = parseMessage<velocityMessage>(msg);
      if(inAtmosphere){
        velocity = myVelocity.vertical;
      }else{
        velocity = myVelocity.orbital;
      }
    }
    break;
  case ALTITUDE_MESSAGE:
    if(msgSize == sizeof(altitudeMessage)){
      altitudeMessage alt = parseMessage<altitudeMessage>(msg);
      altitude = alt.sealevel;
    }
    break;
  case AIRSPEED_MESSAGE:
    if(msgSize == sizeof(airspeedMessage)){
      airspeedMessage air = parseMessage<airspeedMessage>(msg);
      airspeed = air.mach;
    }
    break;
  case APSIDESTIME_MESSAGE:
    if(msgSize == sizeof(apsidesTimeMessage)){
      apsidesTimeMessage time = parseMessage<apsidesTimeMessage>(msg);
      if(time.apoapsis < time.periapsis || time.apoapsis == time.periapsis){
        apTime = time.apoapsis;
      }else{
        apTime = time.periapsis;
      }
    }
    break;
  }
}
