#include <FastLED.h>
#include <Wire.h>

// How many leds in your strip?
#define NUM_LEDS_N 16
#define NUM_LEDS_S 16
#define DATA_PIN_N 2
#define DATA_PIN_S 3
#define I2C_RECEIVER_ADDRESS 1

// Define the array of leds
CRGB ledsNorth[NUM_LEDS_N];
CRGB ledsSouth[NUM_LEDS_S];

void setup() { 
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, DATA_PIN_N, GRB>(ledsNorth, NUM_LEDS_N);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, DATA_PIN_S, GRB>(ledsSouth, NUM_LEDS_S);  // GRB ordering is typical

  FastLED.setBrightness(50);

  //Test Routine
  for (int i = 0; i < NUM_LEDS_N, i++) {
    ledsNorth[i] = CRGB::Red;
    ledsSouth[i] = CRGB::Red;
    FastLED.show();
    delay(50);
  }
  for (int i = 0; i < NUM_LEDS_N, i++) {
    ledsNorth[i] = CRGB::Green;
    ledsSouth[i] = CRGB::Green;
    FastLED.show();
    delay(50);
  }
  for (int i = 0; i < NUM_LEDS_N, i++) {
    ledsNorth[i] = CRGB::Blue;
    ledsSouth[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
  }
  
  FastLED.clear();
  FastLED.show();

  Wire.begin(RECEIVER_ADDRESS);  // join i2c bus with address #1
  Wire.onReceive(receiveEvent);  // register event
}

void loop() { 
  
}

void receiveEvent() {
  int routeMessage = Wire.read();       // receive byte as an integer
  Serial.print("SPI Message received -> ");
  Serial.println(routeMessage);         // print the integer

  //Decide what to do depending on the message
  switch (routeMessage) {
    case 0x0:
      FastLED.clear();
      FastLED.show();
      break;
    case 0x1:
      leds[0] = CRGB::Red;
      FastLED.show();
      break;
    case 0x2:
      leds[1] = CRGB::Green;
      FastLED.show();
      break;
    case 0x3:
      leds[2] = CRGB::Blue;
      FastLED.show();
      break;
    case 0x11:
      leds[2] = CRGB::Blue;
      FastLED.show();
      break;
    case 0x12:
      leds[2] = CRGB::Blue;
      FastLED.show();
      break;
    case 0x13:
      leds[2] = CRGB::Blue;
      FastLED.show();
      break;
    case 0x14:
      leds[2] = CRGB::Blue;
      FastLED.show();
      break;
    default:
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Blue;
        delay(50);
        FastLED.show();
      }
      break;
}
