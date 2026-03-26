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
const int joystickModePin = 44;
unsigned long lastSend = 0;
const int sendInterval = 50;
unsigned long lastDisplay = 0;
const int displayInterval = 200;

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
  ezButton(joystickModePin),
};
const byte buttonActions[] = {SAS_ACTION,STAGE_ACTION,GEAR_ACTION,ABORT_ACTION,RCS_ACTION,BRAKES_ACTION,LIGHT_ACTION,};
// Analog pins (joysticks)
const int joystick1XPin = 0;
const int joystick1YPin = 1;
//0 left right is yaw,1 its roll
int joystickMode = 0;

const int joystick2XPin = 2;
const int joystick2YPin = 3;

const int joystickPins[] = {joystick1XPin,joystick1YPin,joystick2XPin,joystick2YPin};

const int xMin = (511 - 20);
const int xMax = (511 + 20);
const int yMin = (511 - 20);
const int yMax = (511 + 20);
const int zMin = (511 - 20);
const int zMax = (511 + 20);

const int throttlePin = 4;

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
bool connected = false;

KerbalSimpit simpit(Serial);

void setup(){
  
    Serial.begin(115200);

    while (!simpit.init()) {
    delay(100);
    }
    
    simpit.printToKSP("Connected", PRINT_TO_SCREEN);
    simpit.inboundHandler(messageHandler);

      for(int i = 0;i<8;i++){
        simpit.registerChannel(channels[i]);
    }
    
    for (int i = 0; i < 2; i++) {
    lcds[i]->begin(16, 2);
    }

    for(int i = 0;i < 4;i++){
      pinMode(joystickPins[i],INPUT);
    }

}
void loop(){
    simpit.update();
    updateDisplays();

    for(int i=0;i<11;i++){
      buttons[i].loop();
    }

  //Toggles the buttons that simpit has action groups for
    for(int i = 0;i<7;i++){
      if(buttons[i].isPressed()){
        simpit.toggleAction(buttonActions[i]);
      }
    }
    if(buttons[7].isPressed()){
      simpit.toggleCAG(1);
    }
    if(buttons[8].isPressed()){
      currentPages[0]++;
      if(currentPages[0] >= numPages){
        currentPages[0] = 0;
      }
    }if(buttons[9].isPressed()){
      currentPages[1]++;
      if(currentPages[1] >= numPages){
        currentPages[1] = 0;
      }
    }if(buttons[10].isPressed()){
      if(joystickMode == 0){
        joystickMode = 1;
      }else{
        joystickMode = 0;
      }
    }
  rotationMessage rot;
  int rotX = analogRead(joystick1XPin);
  int rotY = analogRead(joystick1YPin);
  translationMessage tra;
  int traX = analogRead(joystick2XPin);
  int traY = analogRead(joystick2YPin);

  int rotationX = 0;
if (rotX < xMin)
  rotationX = map(rotX, 0, xMin, INT16_MIN, 0);
else if (rotX > xMax)
  rotationX = map(rotX, xMax, 1023, 0, INT16_MAX);

int rotationY = 0;
if (rotY < yMin)
  rotationY = map(rotY, 0, yMin, INT16_MIN, 0);
else if (rotY > yMax)
  rotationY = map(rotY, yMax, 1023, 0, INT16_MAX);

if (joystickMode == 0) {

  rot.setPitch(rotationY);
  rot.setYaw(rotationX);
  rot.setRoll(0);
}
else if (joystickMode == 1) {

  rot.setPitch(rotationY);
  rot.setYaw(0);
  rot.setRoll(rotationX);
}

int translationX = 0;

if (traX < xMin)
  translationX = map(traX, 0, xMin, INT16_MIN, 0);
else if (traX > xMax)
  translationX = map(traX, xMax, 1023, 0, INT16_MAX);

int translationY = 0;

if (traY < yMin)
  translationY = map(traY, 0, yMin, INT16_MIN, 0);
else if (traY > yMax)
  translationY = map(traY, yMax, 1023, 0, INT16_MAX);

tra.setX(translationX);
tra.setY(translationY);
tra.setZ(0);

  throttleMessage throttleMsg;
  int throttleReading = analogRead(throttlePin);
  throttleMsg.throttle = map(throttleReading, 0, 1023, 0, INT16_MAX);

if (millis() - lastDisplay > displayInterval) {
  updateDisplays();
  lastDisplay = millis();
}
  simpit.send(THROTTLE_MESSAGE, throttleMsg);
  simpit.send(ROTATION_MESSAGE, rot);
  simpit.send(TRANSLATION_MESSAGE, tra);
}
void printToLcd(int idx, const char* label, float value) {
  lcds[idx]->setCursor(0, 0);
  lcds[idx]->print(label);
  lcds[idx]->setCursor(0, 1);
  lcds[idx]->print(value);
  lcds[idx]->print("     ");
}

void updateDisplays(){
  for(int i=0;i<sizeof(lcds);i++){
    if(currentPages[i]==0){
      printToLcd(i,"Delta-V In Stage",deltaV);
    }else if(currentPages[i]==1){
      printToLcd(i,"Electric Charge",eCharge);
    }else if(currentPages[i]==2){
      printToLcd(i,"Velocity",velocity);
    }else if(currentPages[i]==3){
      printToLcd(i,"Altitude",altitude);
    }else if(currentPages[i]==4){
      printToLcd(i,"Airspeed : Mach",airspeed);
    }else if(currentPages[i]==5){
      printToLcd(i,"Time Til:A/P",apTime);
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
    break;
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