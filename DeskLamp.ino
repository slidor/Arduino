#include <Adafruit_NeoPixel.h>
#include <ezButton.h>
#include <RotaryEncoder.h>
#include <EEPROM.h>


#define ROT_PIN_IN1 2
#define ROT_PIN_IN2 3
#define ROT_BTN_PIN 4
#define PWR_BTN_PIN 5
#define LED_PIN 10

// How many NeoPixels are attached to the Arduino?
#define LED_COUNT 3

// constants

const int SHORT_PRESS_TIME = 1000; // 1000 milliseconds
const int LONG_PRESS_TIME  = 2000; // 1000 milliseconds

// global variables
bool stripON = true;
int red = 0;
int green = 0;
int blue = 0;
int white = 25;
int mode = 0;
int dir = 6;
int maxBrightness = 50;

//HSV TEST
uint16_t hue = 0;

unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);

ezButton powerButton(PWR_BTN_PIN);
ezButton rotaryButton(ROT_BTN_PIN); 

RotaryEncoder *encoder = nullptr;

void ledOn() {
  Serial.print("Set LEDcolor: (R,G,B,W) (");
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.print(blue);
  Serial.print(", ");
  Serial.print(white);
  Serial.println(")");
  switch (dir) {
    case 0:
      strip.fill(strip.Color(red,green,blue,white), 0, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 1:
      strip.fill(strip.Color(0,0,0,0), 0, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 2:
      strip.fill(strip.Color(0,0,0,0), 0, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 3:
      strip.fill(strip.Color(red,green,blue,white), 0, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 4:
      strip.fill(strip.Color(red,green,blue,white), 0, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(0,0,0,0), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 5:
      strip.fill(strip.Color(0,0,0,0), 0, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3, LED_COUNT/3);
      strip.fill(strip.Color(red,green,blue,white), LED_COUNT/3*2, LED_COUNT/3);
      strip.show();
      break;
    case 6:
      strip.fill(strip.Color(red,green,blue,white), 0, LED_COUNT);
      strip.show();
      break;
  }
}

void ledOff() {
  strip.fill(strip.Color(0,0,0,0), 0, LED_COUNT);
  strip.show();
}


void toggleOnOff() {
  if (stripON) {
    ledOff();
    stripON = false;
    Serial.println("LEDs off");
  }
  else {
    ledOn();
    stripON = true;
    Serial.println("LEDs on");
  }
}

void memorize() {
  Serial.println("memorize");
  EEPROM.update(0, 1);
  EEPROM.update(1, red); // Red
  EEPROM.update(2, green); // Green
  EEPROM.update(3, blue); // Blue
  EEPROM.update(4, white); // White
  EEPROM.update(5, dir); // Direction 
}

void readFromEEPROM() {
  if (EEPROM.read(0) == 1) {
    Serial.println("Read values from EEPROM");
    red = EEPROM.read(1); // Red
    green = EEPROM.read(2); // Green
    blue = EEPROM.read(3); // Blue
    white = EEPROM.read(4); // White
    Serial.print("White: ");
    Serial.println(white);
    dir = EEPROM.read(5); // Direction    
  }
}

void changeMode() {
  mode = (mode + 1) % 4;
  Serial.print("mode: ");
  Serial.print(mode);
  Serial.print(" ");
  switch (mode) {
    case 0:
      Serial.println("brightness");
      break;
    case 1:
      Serial.println("direction");
      break;
    case 2:
      Serial.println("color");
      break;
    case 3:
      Serial.println("white");
      break;
    default:
      Serial.println("undefined");
      break;
  }
}

void setLightDirection(int delta) {
  if (stripON) {
    dir = dir + delta;
    if (dir > 6) {
      dir = 0;
    }
    if (dir < 0) {
      dir = 6;
    }
  }
  ledOn();
  Serial.print("direction:");
  Serial.println(dir);
}

void changeBrightness(int delta) {
  int brightnessChange = delta * 10;
  if (stripON) {
    if (red != 0) {
      red = red + brightnessChange;
      if (red > 255) { red = 255; }
      if (red < 15)  { red =  15; }
    }
    if (green != 0) {
      green = green + brightnessChange;
      if (green > 255) { green = 255; }
      if (green < 15)  { green =  15; }
    }
    if (blue != 0) {
      blue = blue + brightnessChange;
      if (blue > 255) { blue = 255; }
      if (blue < 15)  { blue =  15; }
    }
    if (white != 0) {
      white = white + brightnessChange;
      if (white > 255) { white = 255; }
      if (white < 15)  { white =  15; }
    }
    ledOn();
  }
}

void checkPosition()
{
  encoder->tick(); // just call tick() to check the state.
}

void setWhite() {
  red = 0;
  green = 0;
  blue = 0;
  white = 125;
  ledOn();
  Serial.println("reset to white");
}

void setColor(int delta) {
  hue = hue + delta * 1024;
  Serial.print("HUE Value: ");
  Serial.println(hue);
  union {
    uint32_t rgbcolor;
    uint8_t temp_array[4];
  } u;
  u.rgbcolor = strip.ColorHSV(hue);
  red = u.temp_array[2];
  green = u.temp_array[1];
  blue = u.temp_array[0];
  white = u.temp_array[3];
  ledOn();
}

int updatePosition(int input) {
  int pos = input;
  int newPos = encoder->getPosition();
  if (pos != newPos) {
    switch (mode) {
      case 0:
        changeBrightness(newPos - pos);
        break;
      case 1:
        setLightDirection(newPos - pos);
        break;
      case 2:
        setColor(newPos - pos);
        break;
      case 3:
        setWhite();
        break;
    }
    pos = newPos;
  }
  return pos;
}

void checkPowerButton() {
  if(powerButton.isPressed())
    pressedTime = millis();

  if(powerButton.isReleased()) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if( pressDuration < SHORT_PRESS_TIME )
      toggleOnOff();

    if( pressDuration > LONG_PRESS_TIME )
      memorize();
  }
}

void checkRotaryButton() {
  if(rotaryButton.isPressed()) {
    changeMode();
  }
}

// setup() function -- runs once at startup --------------------------------

void setup() {

  Serial.begin(115200);
  while (!Serial)
    ;
  Serial.println("DeskLamp started");

  readFromEEPROM();
  
  encoder = new RotaryEncoder(ROT_PIN_IN1, ROT_PIN_IN2, RotaryEncoder::LatchMode::FOUR3);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_IN1), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ROT_PIN_IN2), checkPosition, CHANGE);

  powerButton.setDebounceTime(50);
  rotaryButton.setDebounceTime(50);
    
  strip.begin();           
  strip.show();            
  strip.setBrightness(maxBrightness);
  
  ledOn();
}

// loop() function -- runs repeatedly as long as board is on ---------------

void loop() {
  static int pos = 0;
  powerButton.loop();
  rotaryButton.loop();
  pos = updatePosition(pos);
  checkPowerButton();
  checkRotaryButton();
}
