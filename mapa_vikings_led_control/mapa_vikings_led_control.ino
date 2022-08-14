#include <FastLED.h>
#include <Wire.h>

// How many leds in your strip?
#define NUM_LEDS_N 16
#define NUM_LEDS_S 16
#define DATA_PIN_N 8
#define DATA_PIN_S 9
#define I2C_RECEIVER_ADDRESS 1

// Define the array of leds
CRGB ledsNorth[NUM_LEDS_N];
CRGB ledsSouth[NUM_LEDS_S];

void setup() { 
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, DATA_PIN_N, GRB>(ledsNorth, NUM_LEDS_N);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, DATA_PIN_S, GRB>(ledsSouth, NUM_LEDS_S);  // GRB ordering is typical

  FastLED.setBrightness(50);
  //FastLED.setMaxPowerInVoltsAndMilliamps(5, 300);

  //Test Routine
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Red;
    ledsSouth[i] = CRGB::Red;
    FastLED.show();
    delay(50);
  }
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Green;
    ledsSouth[i] = CRGB::Green;
    FastLED.show();
    delay(50);
  }
  for (int i = 0; i < NUM_LEDS_N; i++) {
    ledsNorth[i] = CRGB::Blue;
    ledsSouth[i] = CRGB::Blue;
    FastLED.show();
    delay(50);
  }
  
  FastLED.clear();
  FastLED.show();

  fill_solid(ledsNorth, 4, CRGB::Yellow);
  fill_solid(ledsSouth, 4, CRGB::Yellow);  
  FastLED.show();

  Wire.begin(I2C_RECEIVER_ADDRESS);  // join i2c bus with address #1
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
    case 0x7:
      FastLED.clear();
      FastLED.show();
      break;
      //North route cities
    case 1:
      fill_solid(ledsNorth + 4, 4, CRGB::Yellow);
      FastLED.show();
      break;      
    case 2:
      fill_solid(ledsNorth + 8, 4, CRGB::Yellow);
      FastLED.show();
      break;
    case 3:
      fill_solid(ledsNorth + 12, 4, CRGB::Yellow);
      FastLED.show();
      break;
      //South Route cities
    case 4:
      fill_solid(ledsSouth + 4, 4, CRGB::Yellow);
      FastLED.show();
      break;
    case 5:
      fill_solid(ledsSouth + 8, 4, CRGB::Yellow);
      FastLED.show();
      break;
    case 6:
      fill_solid(ledsSouth + 12, 4, CRGB::Yellow);
      FastLED.show();
      break;      
    default:
      for (int i = 0; i < NUM_LEDS_N; i++) {
        ledsNorth[i] = CRGB::Blue;
        delay(50);
        FastLED.show();
      }
      for (int i = 0; i < NUM_LEDS_S; i++) {
        ledsSouth[i] = CRGB::Blue;
        delay(50);
        FastLED.show();
      }
      break;
  }
}
