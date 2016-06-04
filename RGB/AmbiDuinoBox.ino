// AmbiDuinoBox by Kenwonders
// Supports Ambibox and AtmoDuino V1 / V2 all at once.
// For more info see: http://forum.team-mediaportal.com/threads/atmoduinobox-multi-protocol-arduino-code-for-ambient-lights.134483/

#include <FastLED.h>
//-----------------------EDITABLE CONFIGURATION
#define NUM_LEDS 176
#define LED_TYPE WS2812B
#define DATA_PIN 11
//#define CLOCK_PIN 0
#define COLOUR_ORDER RGB
#define SERIAL_SPEED 512000
//-----------------------EDITABLE CONFIGURATION

CRGB leds[NUM_LEDS];
const uint8_t headers[3][4] =  {
  { 0xD7, 0xEE, 0x23, NUM_LEDS - 1}, //atmoduinov2
  { 0xFF, 0x00, 0x00, NUM_LEDS * 3}, //ambibox atmoduino can overflow but the truncation is what ambibox appears to do too
  { 0xFF, 0x00, 0x00, NUM_LEDS} //atmowin atmoduino
};
const uint8_t numHeaders = 3;

void setup() {
  Serial.begin(SERIAL_SPEED);

  //If your LEDs have a clock pin, swap these two below:
//-----------------------EDITABLE CONFIGURATION  
  //FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOUR_ORDER>(leds, NUM_LEDS);
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOUR_ORDER>(leds, NUM_LEDS);
//-----------------------EDITABLE CONFIGURATION  
  clearLeds();
}
void loop() {
  if (findHeader()) {
    loadFrame();
    flushBuffer();
  }
}
bool findHeader() {
  for (uint8_t magicByte = 0; magicByte < 4; magicByte++) {
    uint8_t receivedByte = readByte();
    bool foundMagicByte = false;
    for (uint8_t headerNum = 0; headerNum < numHeaders; headerNum++) {
      if (receivedByte == headers[headerNum][magicByte])
      {
        foundMagicByte = true;
      }
    }
    if (!foundMagicByte) {
      return false;
    }
  }
  return true;
}
void loadFrame() {
  for (uint8_t dot = 0; dot < NUM_LEDS; dot++) {
    leds[dot].g = readByte();
    leds[dot].r = readByte();
    leds[dot].b = readByte();
  }
  FastLED.show();
}
int readByte() {
  while (Serial.available() == 0) {}
  return Serial.read();
}
void flushBuffer() {
  while (Serial.available() > 0) {
    Serial.read();
  }
}
void clearLeds() {
  memset(leds, 0, sizeof(leds));
  FastLED.show();
}
