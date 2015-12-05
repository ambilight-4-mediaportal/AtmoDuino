// AtmoduinoV2 by NevCairiel,
// based on RickDB Atmoduino V1
// UCS2912 support by Lightning303

#include <FastLED.h>

// Set the number of leds in the strip.
#define NUM_LEDS 60

// Set the data pin
#define DATA_PIN 6

// Choose if you want to use white correction
// 1 = enabled
// 0 = disabled
#define WHITE_CORRECTION 1
// Percentage of color that should be added to white
// Example:
// -> Red = Red + RED_CORRECTION * White / 100
// -> Green = Gre + GREEN_CORRECTION * White / 100
// -> Blue = Blue + BLUE_CORRECTION * White / 100
#define RED_CORRECTION 0
#define GREEN_CORRECTION 40
#define BLUE_CORRECTION 50

CRGB leds[NUM_LEDS + (NUM_LEDS / 3)];
byte ledBuffer[12];
byte ledCounter = 0;
int currentLed = 0;

void setup()
{
  Serial.begin(576000);

  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS + (NUM_LEDS / 3));

  clearLeds();
}

int readByte()
{
  while (Serial.available() == 0){ /* wait for data */ }
  return Serial.read();
}

void clearLeds()
{
  for(int dot = 0; dot < NUM_LEDS + (NUM_LEDS / 3); dot++)
  {
    leds[dot] = CRGB::Black; 
  }
  FastLED.show();
}


void loop()
{ 
  byte rgb[4];
  if(readByte() == 0xD7){
    if(readByte() == 0xEE){
      if(readByte() == 0x23){
        currentLed = 0;
        ledCounter = 0;
        int channels = readByte() + 1;
        for(int dot = 0; dot < channels; dot++){          
          rgb[0] = readByte();
          rgb[1] = readByte();
          rgb[2] = readByte();

          rgb[3] = min(rgb[0], rgb[1]);
          rgb[3] = min(rgb[3], rgb[2]);
          if (WHITE_CORRECTION == 1)
          {
            AddColor(rgb[0] - rgb[3] + (rgb[3] * RED_CORRECTION / 100));
            AddColor(rgb[1] - rgb[3] + (rgb[3] * GREEN_CORRECTION / 100));
            AddColor(rgb[2] - rgb[3] + (rgb[3] * BLUE_CORRECTION / 100));
          }
          else
          {
            AddColor(rgb[0] - rgb[3]);
            AddColor(rgb[1] - rgb[3]);
            AddColor(rgb[2] - rgb[3]);
          }
          AddColor(rgb[3]);
        }
        FastLED.show();
      }
    }
  }
}

void AddColor(byte color)
{
  ledBuffer[ledCounter++] = color;
  if (ledCounter == 12)
  {
    ledCounter = 0;
    leds[currentLed++] = CRGB(ledBuffer[0], ledBuffer[1], ledBuffer[2]);
    leds[currentLed++] = CRGB(ledBuffer[3], ledBuffer[4], ledBuffer[5]);
    leds[currentLed++] = CRGB(ledBuffer[8], ledBuffer[7], ledBuffer[6]);
    leds[currentLed++] = CRGB(ledBuffer[11], ledBuffer[10], ledBuffer[9]);
  }
}
