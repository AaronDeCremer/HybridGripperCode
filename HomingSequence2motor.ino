#include <AccelStepper.h>

// AccelStepper Setup
#define motor1_dir_pin 2 // Motor 1 pins
#define motor1_step_pin 3
#define endStopPin1 19 // End switches motor 1
#define endStopPin2 18
AccelStepper stepper1 = AccelStepper(1, motor1_step_pin, motor1_dir_pin);
                                  
#define motor2_dir_pin 4 // Motor 2 pins
#define motor2_step_pin 5
#define endStopPin3 21 // End switches motor 2
#define endStopPin4 20                                  
AccelStepper stepper2 = AccelStepper(1, motor2_step_pin, motor2_dir_pin);
bool positionSet1 = false;
bool homed1 = false;
bool positionSet2 = false;
bool homed2 = false;
void setup() {
  Serial.begin(9600); // initialize serial communication at 9600 baud rate
  stepper1.setMaxSpeed(200);
  stepper1.setAcceleration(200);
  stepper1.setSpeed(0);
  stepper2.setMaxSpeed(200);
  stepper2.setAcceleration(200);
  stepper2.setSpeed(0);
  pinMode(endStopPin1, INPUT_PULLUP);
  pinMode(endStopPin2, INPUT_PULLUP);
  pinMode(endStopPin3, INPUT_PULLUP);
  pinMode(endStopPin4, INPUT_PULLUP);
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);

}

void loop() {

  if((digitalRead(endStopPin1) == LOW) && (homed1 == false)){
    //Let motor run until endstop is reached at slow speed
    stepper1.setSpeed(-200);
    stepper1.run();
  }
  else{
    // If the endstop is reached, the motor will move away for a few steps for safety and set the current position to 0
    stepper1.setCurrentPosition(0);
    if(positionSet1 == false){
      stepper1.setSpeed(100);
      stepper1.moveTo(700);
      delay(1000);
      stepper1.runToPosition();
      stepper1.setCurrentPosition(0);
      Serial.println("Homed motor1");
      homed1 = true;
      positionSet1 = true;
    }
  }
  if(homed1 == true){
    // repeat as before but fot second motor
    if((digitalRead(endStopPin4) == LOW) && (homed2 == false) && (homed1 == true)){
    stepper2.setSpeed(-200);
    stepper2.run();
    }
    else{
      stepper2.setCurrentPosition(0);
      if(positionSet2 == false){
        stepper2.setSpeed(100);
        stepper2.moveTo(700);
        delay(1000);
        stepper2.runToPosition();
        stepper2.setCurrentPosition(0);
        Serial.println("Homed motor2");
        homed2 = true;
        positionSet2 = true;
      }
    }
  }
}