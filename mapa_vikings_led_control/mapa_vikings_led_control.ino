#include <FastLED.h>
#include <Wire.h>

// How many leds in your strip?
#define NUM_LEDS_N 16
#define NUM_LEDS_S 16
#define DATA_PIN_N 2
#define DATA_PIN_S 3

// Define the array of leds
CRGB ledsNorth[NUM_LEDS_N];
CRGB ledsSouth[NUM_LEDS_S];

void setup() { 
  // Uncomment/edit one of the following lines for your leds arrangement.
  // ## Clockless types ##
  FastLED.addLeds<WS2812B, DATA_PIN_N, GRB>(ledsNorth, NUM_LEDS_N);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, DATA_PIN_S, GRB>(ledsSouth, NUM_LEDS_S);  // GRB ordering is typical

  FastLED.setBrightness(50);
  FastLED.clear();
  FastLED.show();
}

void loop() { 
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Yellow;
    FastLED.show();
    delay(100);
  }
  for (int i = 0; i < NUM_LEDS_S; i++) {
    ledsSouth[i] = CRGB::Yellow;
    FastLED.show();
    delay(100);
  }
  delay(2000);
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Red;
    FastLED.show();
    delay(100);
  }
  for (int i = 0; i < NUM_LEDS_S; i++) {
    ledsSouth[i] = CRGB::Red;
    FastLED.show();
    delay(100);
  }
  delay(2000);
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Black;
    FastLED.show();
    delay(100);
  }
  for (int i = 0; i < NUM_LEDS_S; i++) {
    ledsSouth[i] = CRGB::Black;
    FastLED.show();
    delay(100);
  }
  delay(2000);
}
