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

// Define the array of leds
CRGB ledsNorth[NUM_LEDS_N];
CRGB ledsSouth[NUM_LEDS_S];

cppQueue commandQueue(sizeof(int) * 10, MAX_QUEUE_COMMANDS, QUEUE_IMPLEMENTATION);

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
  unsigned long previousMillis = millis();
  
  if (commandQueue.isEmpty())
  {
    delay(20);
  } 
  else
  {
    //Pop first item in queue
    int commandRecord = -1;
    bool success = commandQueue.pop(&commandRecord);
    
    Serial.print("Queue pop succesfully gathered? ");
    Serial.print(success ? "YES" : "NO");
    Serial.print(" on command record received (-1 default value) ");
    Serial.println(commandRecord);
    
    executeCommand(commandRecord);

    
      
    
  }
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
      animateLedStrip(ledsNorth, 4, 4);
      break;      
    case 2:
      animateLedStrip(ledsNorth, 8, 4);
      //fill_solid(ledsNorth + 8, 4, CRGB::Yellow);
      //FastLED.show();
      break;
    case 3:
      animateLedStrip(ledsNorth, 12, 4);
      //fill_solid(ledsNorth + 12, 4, CRGB::Yellow);
      //FastLED.show();
      break;
      //South Route cities
    case 4:
      animateLedStrip(ledsSouth, 4, 4);
      //fill_solid(ledsSouth + 4, 4, CRGB::Yellow);
      //FastLED.show();
      break;
    case 5:
      animateLedStrip(ledsSouth, 8, 4);
      //fill_solid(ledsSouth + 8, 4, CRGB::Yellow);
      //FastLED.show();
      break;
    case 6:
      animateLedStrip(ledsSouth, 12, 3);
      //fill_solid(ledsSouth + 12, 3, CRGB::Yellow);
      //FastLED.show();
      break;
    //Turn off cities north 2-4   
    case 14:
      fill_solid(ledsNorth + 4, 12, CRGB::Black);
      FastLED.show();
      break;
    //Turn off cities south 2-4
    case 15:    
      fill_solid(ledsSouth + 4, 11, CRGB::Black);
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

void animateLedStrip(CRGB leds[], int offset, int numLeds) {
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
