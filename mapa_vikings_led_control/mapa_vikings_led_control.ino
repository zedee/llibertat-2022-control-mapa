#include <FastLED.h>
#include <Wire.h>
#include <cppQueue.h>

// Define queue implementation
#define QUEUE_IMPLEMENTATION FIFO

// Leds per route strip
#define NUM_LEDS_N 16
#define NUM_LEDS_S 15

// Data pins for each route
#define DATA_PIN_N 8
#define DATA_PIN_S 9
#define I2C_RECEIVER_ADDRESS 1

// Queue max commands
#define MAX_QUEUE_COMMANDS 10

#define NUM_CITIES_NORTH 4
#define NUM_CITIES_SOUTH 4

// Define the array of leds
CRGB ledsNorth[NUM_LEDS_N];
CRGB ledsSouth[NUM_LEDS_S];

// Define how to implement an animation by steps
const int Number_of_steps_in_animation = 6;
CRGB animation_color_steps[Number_of_steps_in_animation] = {CRGB::Red, CRGB::Yellow, CRGB::Red, CRGB::Yellow, CRGB::Red, CRGB::Yellow};
int animation_duration_steps[Number_of_steps_in_animation] = {300, 300, 300, 300, 300, 300};

unsigned long prevMillis[6] = {0, 0, 0, 0, 0, 0};
bool cityAnimationEnded[6] = {true, true, true, true, true, true};
int cityAnimationStep[6] = {0, 0, 0, 0, 0, 0};
int cityAnimationStartLedArray[6];
int cityAnimationStartFollowerLed[6] = {0, 0, 0, 0, 0, 0};
int cityAnimationNumberOfLeds[6] = {0, 0, 0, 0, 0, 0};

cppQueue commandQueue(sizeof(int) * 10, MAX_QUEUE_COMMANDS, QUEUE_IMPLEMENTATION);

void setup() { 
  Serial.begin(115200);
  
  FastLED.addLeds<WS2812B, DATA_PIN_N, GRB>(ledsNorth, NUM_LEDS_N);  // GRB ordering is typical
  FastLED.addLeds<WS2812B, DATA_PIN_S, GRB>(ledsSouth, NUM_LEDS_S);  // GRB ordering is typical

  FastLED.setBrightness(30);
  //FastLED.setMaxPowerInVoltsAndMilliamps(5, 200);

  //Test Routine
  startupLedTest();

  fill_solid(ledsNorth, 4, CRGB::Yellow);
  fill_solid(ledsSouth, 4, CRGB::Yellow);  
  FastLED.show();

  Wire.begin(I2C_RECEIVER_ADDRESS);  // join i2c bus with address #1
  Wire.onReceive(receiveEvent);  // register event
}

void loop() { 
  /*
  if (commandQueue.isEmpty())
  {
    delay(20);
  } 
  else
  {
    //Pop first item in queue
    int commandRecord = -1;
    bool success = commandQueue.pop(&commandRecord);    
    executeCommand(commandRecord);    
  }
  */

  if (!commandQueue.isEmpty())
  {
    //Pop first item in queue
    int commandRecord = -1;
    bool success = commandQueue.pop(&commandRecord);    
    executeCommand(commandRecord); 
  }
  
  keepAnimationAlive();

  //delay(50);
}

void receiveEvent() {
  int routeMessage = Wire.read();       // receive byte as an integer
  Serial.print("SPI Message received -> ");
  Serial.println(routeMessage);           // print the integer
  commandQueue.push(&routeMessage);
}

void executeCommand(int commandMessage) {
  switch (commandMessage) {
    //North route cities
    case 0:
      FastLED.clear();
      FastLED.show();
      break;         
    case 1:
      if (cityAnimationEnded[0]) {
        animateLedStripBlocker(ledsNorth, 4, 4);
        //animateLedStrip(ledsNorth, 4, 4, 0);
      }
      break;      
    case 2:
      if (cityAnimationEnded[1]) {
        animateLedStripBlocker(ledsNorth, 8, 4);
        //animateLedStrip(ledsNorth, 8, 4, 1);
      }      
      break;
    case 3:
      if (cityAnimationEnded[2]) {
        animateLedStripBlocker(ledsNorth, 12, 4);
        //animateLedStrip(ledsNorth, 12, 4, 2);
      }
      break;
      //South Route cities
    case 4:
      if (cityAnimationEnded[3]) {
        animateLedStripBlocker(ledsSouth, 4, 4);
        //animateLedStrip(ledsSouth, 4, 4, 3);
      }
      break;
    case 5:
      if (cityAnimationEnded[4]) {
        animateLedStripBlocker(ledsSouth, 8, 4);
        //animateLedStrip(ledsSouth, 8, 4, 4);
      }
      break;
    case 6:
      if (cityAnimationEnded[5]) {
        animateLedStripBlocker(ledsSouth, 12, 3);
        //animateLedStrip(ledsSouth, 12, 3, 5);
      }
      break;
    //Turn off cities north 2-4   
    case 14:
      shutdownLedStrip(ledsNorth, 4, 12);
      break;
    //Turn off cities south 2-4
    case 15:    
      shutdownLedStrip(ledsSouth, 4, 11);
      break;
    default:  
      errorLedLights();
      break;
  }
}

void animateLedStripBlocker(CRGB leds[], int offset, int numLeds) {
  for (int i = 0; i < 12; i++) {
    fill_solid(leds + offset, numLeds, CRGB::Red);
    FastLED.show();
    delay(250);
    fill_solid(leds + offset, numLeds, CRGB::Yellow);
    FastLED.show();
  }
  fill_solid(leds + offset, numLeds, CRGB::Yellow);
  FastLED.show();
}

void animateLedStrip(CRGB leds[], int offset, int numLeds, int stripId)
{
  cityAnimationEnded[stripId] = false;  
  prevMillis[stripId] = millis();
  cityAnimationStep[stripId] = 0;

  cityAnimationStartLedArray[stripId] = leds + offset;
  cityAnimationNumberOfLeds[stripId] = numLeds;
  
  fill_solid(
    cityAnimationStartLedArray[stripId], 
    cityAnimationNumberOfLeds[stripId], 
    animation_color_steps[cityAnimationStep[stripId]]
  );
  
  FastLED.show();
}

void keepAnimationAlive()
{
  for (int stripId = 0; stripId < 6; stripId++) {
    if ((unsigned long)millis() - prevMillis[stripId] >= animation_duration_steps[cityAnimationStep[stripId]])
    {
      if (!cityAnimationEnded[stripId]) {
        cityAnimationStep[stripId] = ++cityAnimationStep[stripId];
        prevMillis[stripId] = millis();
                
        fill_solid(
          cityAnimationStartLedArray[stripId], 
          cityAnimationNumberOfLeds[stripId], 
          animation_color_steps[cityAnimationStep[stripId]]
        );
      }     
      FastLED.show();
    }
    if (cityAnimationStep[stripId] == 6) {
      cityAnimationEnded[stripId] = true;
      fill_solid(
        cityAnimationStartLedArray[stripId], 
        cityAnimationNumberOfLeds[stripId], 
        CRGB::Yellow
      );
    }
  }
}

void shutdownLedStrip(CRGB leds[], int offset, int numLeds) {
  fill_solid(leds + offset, numLeds, CRGB::Black);
  FastLED.show();
}

void startupLedTest() {
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
}

void errorLedLights() {
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
}
