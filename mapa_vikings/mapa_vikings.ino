
//      ******************************************************************
//      *                                                                *
//      *     Example of running two or more motors at the same time     *
//      *                                                                *
//      *            S. Reifel & Co.                6/24/2018            *
//      *                                                                *
//      ******************************************************************


// This example shows how to run two motors at the same time.  The previous 
// examples used function calls that were "blocking", meaning that they did 
// not return until the motion was complete.  This example show how to use
// a "polling" method instead, thus allowing you to do two or more things 
// at once.
//
// A possible area of concern is that running multiple motors simultaneously  
// may limit the top speed of each.  This library can generate a maximum of  
// about 12,500 steps per second using an Arduino Uno.  Running just one  
// motor in full step mode, with a 200 steps per rotation motor, the maximum  
// speed is about 62 RPS or 3750 RPM (most stepper motor can't go this fast). 
// Driving one motor in half step mode, a maximum speed of 31 RPS or 1875 RPM  
// can be reached.  In quarter step mode about 15 RPS or 937 RPM.  Running  
// multiple motors at the same time will reduce the maximum speed, for example  
// running two motors will reduce the maximum step rate by half or more.
//  
//
// Documentation at:
//    https://github.com/Stan-Reifel/SpeedyStepper
//
//
// The motor must be connected to the Arduino with a driver board having a 
// "Step and Direction" interface.  It's VERY important that you set the 
// motor current first!  Read the driver board's documentation to learn how.

// ***********************************************************************

/* Script that drives the two map motors, as well as sending messages to the
 *  driver for the lights
 * 
 * Deps: 
 * · SpeedyStepper -> Motor library
 *   Wire -> I2C library
 */


#include <SpeedyStepper.h>
#include <Wire.h>

//
// pin assignments
//
const int MOTOR_X_STEP_PIN = 15;
const int MOTOR_X_DIRECTION_PIN = 21;
const int MOTORS_ENABLE = 14;
const int MOTOR_Y_STEP_PIN = 22;
const int MOTOR_Y_DIRECTION_PIN = 23;

const int LED_DRIVER_ADDRESS = 1;

int Steps_up_route[] = {1000,1050,600};
int Leds_up_route[] = {0x00, 0x01, 0x02};

int Steps_down_route[] = {600,1500,700};
int Leds_down_route[] = {0x11, 0x12, 0x13};

unsigned long Up_route_start_stop = 0;
unsigned long Down_route_start_stop = 0;
unsigned long Number_of_millis_to_stop = 10000;

const int Stepper_movement_per_round = 50;

int Stepper_x_total_movement = 0;
int Stepper_y_total_movement = 0;

bool upCityArrived = false;
int upCityArrivedId = 0;
bool downCityArrived = false;
int downCityArrivedId = 0;
bool upRouteEnded = false;
bool downRouteEnded = false;
bool upWayRecentChanged = true;
bool downWayRecentChanged = true;

bool homing_enabled = true;
int number_of_loops_until_homing = 10;
int x_number_of_loops_without_homing = 0;
int y_number_of_loops_without_homing = 0;

//
// create two stepper motor objects, one for each motor
//
SpeedyStepper stepperX;
SpeedyStepper stepperY;

void setup() 
{
  // Serial debugging
  Serial.begin(115200);
  
  // Init I2C
  Wire.begin();

  pinMode(MOTORS_ENABLE, OUTPUT);
  digitalWrite(MOTORS_ENABLE, LOW);
  //
  // connect and configure the stepper motors to their IO pins
  //
  stepperX.connectToPins(MOTOR_X_STEP_PIN, MOTOR_X_DIRECTION_PIN);
  stepperY.connectToPins(MOTOR_Y_STEP_PIN, MOTOR_Y_DIRECTION_PIN);

  Serial.println("Route to start in 5 seconds...");
  delay(5000);
}


void loop() 
{
  int xStepperLoopMovement = 0;
  int yStepperLoopMovement = 0;

  if (!upRouteEnded)
  {
    if (upWayRecentChanged)
    {
      Serial.println("---- Upper route to start ---- ");
      upWayRecentChanged = false;
    }

    if (upCityArrived)
    {
      upCityArrived = false; 
      Serial.print("[UpRoute] City with id ");
      Serial.print(upCityArrivedId);
      Serial.println(" has been arrived. War starts :) ");

      /* I2C TRansmission */
      /*byte cityArriveByte;
      cityArriveByte = static_cast<byte>(upCityArrivedId);
      Serial.print("CITY ID BYTE --> ");
      Serial.println(cityArriveByte);
      Wire.beginTransmission(LED_DRIVER_ADDRESS);
      Wire.write(cityArriveByte);
      Wire.endTransmission();*/
      /*EOT */
      
      Up_route_start_stop = millis();

      //Leds to be shown by using Leds_up_route
    }
    
    if (millis() - Up_route_start_stop > Number_of_millis_to_stop)
    {
      Up_route_start_stop = 0;
      int numberStopsUpRoute = sizeof(Steps_up_route)/sizeof(Steps_up_route[0]);
      
      upRouteEnded = getNeededSteps(Stepper_x_total_movement, Stepper_movement_per_round, Steps_up_route, numberStopsUpRoute, xStepperLoopMovement, upCityArrived, upCityArrivedId);
    }
    
    Stepper_x_total_movement += xStepperLoopMovement;
    
    if (upRouteEnded)
    {
      Stepper_x_total_movement = 0;
      upWayRecentChanged = true;
    }
  }
  else
  {
    if (upWayRecentChanged)
    {
      Serial.println("---- Upper route has ended ---- ");
      upWayRecentChanged = false;
    }

    int numberStopsUpRoute = sizeof(Steps_up_route)/sizeof(Steps_up_route[0]);
    int backTotalSteps = getTotalStepsOfRoute(Steps_up_route, numberStopsUpRoute);
    
    bool backwardsEnded = getNeededSteps(backTotalSteps, Stepper_x_total_movement, Stepper_movement_per_round, xStepperLoopMovement);
    Stepper_x_total_movement += xStepperLoopMovement;
    xStepperLoopMovement *= -1;
    
    if (backwardsEnded)
    {
      Stepper_x_total_movement = 0;
      upWayRecentChanged = true;
      upRouteEnded = false;
      ++x_number_of_loops_without_homing;
    }
  }

  if (!downRouteEnded)
  {
    if (downWayRecentChanged)
    {
      Serial.println("---- Down route to start ---- ");
      downWayRecentChanged = false;
      /*Wire.beginTransmission(LED_DRIVER_ADDRESS);
      Wire.write(0x03);
      Wire.endTransmission();*/
    }

    if (downCityArrived)
    {
      downCityArrived = false; 
      Serial.print("[DownRoute] City with id ");
      Serial.print(downCityArrivedId);
      Serial.println(" has been arrived. War starts :) ");
      Down_route_start_stop = millis();

      //Leds to be shown by using Leds_down_route
    }

    if (millis() - Down_route_start_stop > Number_of_millis_to_stop)
    {
      Down_route_start_stop = 0;
      int numberStopsDownRoute = sizeof(Steps_down_route)/sizeof(Steps_down_route[0]);
      
      downRouteEnded = getNeededSteps(Stepper_y_total_movement, Stepper_movement_per_round, Steps_down_route, numberStopsDownRoute, yStepperLoopMovement, downCityArrived, downCityArrivedId);
    }
    
    Stepper_y_total_movement += yStepperLoopMovement;
    
    if (downRouteEnded)
    {
      Stepper_y_total_movement = 0;
      downWayRecentChanged = true;
      /*Wire.beginTransmission(LED_DRIVER_ADDRESS);
      Wire.write(0x04);
      Wire.endTransmission();*/
    }
  }
  else
  {
    if (downWayRecentChanged)
    {
      Serial.println("---- Down route has ended ---- ");
      downWayRecentChanged = false;
    }

    int numberStopsDownRoute = sizeof(Steps_down_route)/sizeof(Steps_down_route[0]);
    int backTotalSteps = getTotalStepsOfRoute(Steps_down_route, numberStopsDownRoute);
    
    bool backwardsEnded = getNeededSteps(backTotalSteps, Stepper_y_total_movement, Stepper_movement_per_round, yStepperLoopMovement);
    Stepper_y_total_movement += yStepperLoopMovement;
    yStepperLoopMovement *= -1;
    
    if (backwardsEnded)
    {
      Stepper_y_total_movement = 0;
      downWayRecentChanged = true;
      downRouteEnded = false;
      ++y_number_of_loops_without_homing;
    }
  }

  if (xStepperLoopMovement != 0)
  {
    Serial.print("StepperX to move ");
    Serial.println(xStepperLoopMovement);
  }

  if (yStepperLoopMovement != 0)
  {
    Serial.print("StepperY to move ");
    Serial.println(yStepperLoopMovement);
  }
  
  moveStepper(&stepperX, xStepperLoopMovement, &stepperY, yStepperLoopMovement);
  //moveStepper(&stepperX, xStepperLoopMovement, NULL, 0);

  if (homing_enabled)
  {
    int x_steps_to_home = 0;
    int y_steps_to_home = 0;

    if (x_number_of_loops_without_homing == number_of_loops_until_homing)
    {
      x_number_of_loops_without_homing = 0;
      x_steps_to_home = 50;
    }
    
    if (y_number_of_loops_without_homing == number_of_loops_until_homing)
    {
      y_number_of_loops_without_homing = 0;
      y_steps_to_home = 50;
    }

    moveStepper(&stepperX, xStepperLoopMovement, &stepperY, yStepperLoopMovement, 20);
  }
  
}

bool getNeededSteps (int totalMovement, int maxStepsPerRound, int arrayOfMovements[], int numberStopsUpRoute, int & stepsToExecute, bool & cityArrived, int & cityId)
{
  int sumOfStepsUpRoute = 0;
  int i = 0;
  bool ended = false;
  cityArrived = false;

  while (totalMovement >= sumOfStepsUpRoute && !ended)
  {    
    if (i < numberStopsUpRoute)
    {
      sumOfStepsUpRoute += arrayOfMovements[i];
      ++i;
    }
    else
    {
      ended = true;
    }
  }

  int xStepperLoopMovement = 0;
  if (!ended)
  {
    int stepsUntilNextStop = (sumOfStepsUpRoute - totalMovement);
    xStepperLoopMovement = stepsUntilNextStop > maxStepsPerRound ? maxStepsPerRound : stepsUntilNextStop;
  }
  
  stepsToExecute = xStepperLoopMovement;

  if (totalMovement + stepsToExecute == sumOfStepsUpRoute)
  {
    Serial.println("City arrived on getNeededSteps");
    cityArrived = true;
    cityId = i - 1;
  }

  if (ended)
  {
    Serial.println("---- Route has ended ----");
  }

  return ended;
}

bool getNeededSteps (int totalMovement, int alreadyMoved, int maxStepsPerRound, int & stepsToExecute)
{
  bool ended = false;

  if (totalMovement > alreadyMoved)
  {
    int pendingToMove = totalMovement - alreadyMoved;
    stepsToExecute = pendingToMove > maxStepsPerRound ? maxStepsPerRound : pendingToMove;
  }
  else
  {
    ended = true;
  }
  
  return ended;
}

int getTotalStepsOfRoute (int arrayOfMovements[], int numberStopsUpRoute)
{
  int sumOfStepsUpRoute = 0;
  int i = 0;

  for (int i = 0; i < numberStopsUpRoute; ++i)
  {
    sumOfStepsUpRoute += arrayOfMovements[i];
  }

  return sumOfStepsUpRoute;
}

void moveStepper (SpeedyStepper * stepper1, int stepsToMove1, SpeedyStepper * stepper2, int stepsToMove2)
{
  return moveStepper(stepper1, stepsToMove1, stepper2, stepsToMove2, 50);
}

void moveStepper (SpeedyStepper * stepper1, int stepsToMove1, SpeedyStepper * stepper2, int stepsToMove2, int stepsPerSecond)
{
  //
  // setup the speed, acceleration and number of steps to move for the 
  // X motor, note: these commands do not start moving yet
  //
  if (stepper1)
  {
    stepper1->setSpeedInStepsPerSecond(stepsPerSecond * 16);
    stepper1->setAccelerationInStepsPerSecondPerSecond(600 * 16);
    stepper1->setupRelativeMoveInSteps(stepsToMove1 * 16);
  }
  
  if (stepper2)
  {
    stepper2->setSpeedInStepsPerSecond(50 * 16);
    stepper2->setAccelerationInStepsPerSecondPerSecond(600 * 16);
    stepper2->setupRelativeMoveInSteps(stepsToMove2 * 16);
  }
  
  // Don´t change the parameter order!!!
  while( (stepper1 && !stepper1->motionComplete()) || (stepper2 && !stepper2->motionComplete()) )
  {
    if (stepper1)
    {
      stepper1->processMovement();
    }

    if (stepper2)
    {
      stepper2->processMovement();
    }
  }
}
