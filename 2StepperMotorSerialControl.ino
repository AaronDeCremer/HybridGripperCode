
#include "AccelStepper.h" 
// Library created by Mike McCauley at http://www.airspayce.com/mikem/arduino/AccelStepper/

// AccelStepper Setup
#define motor1_dir_pin 2 // Motor 1 pins
#define motor1_step_pin 3
const int endStopPin1 = 19; // End switches motor 1
const int endStopPin2 = 18;
AccelStepper stepper1 = AccelStepper(1, motor1_step_pin, motor1_dir_pin);
                                  
#define motor2_dir_pin 4 // Motor 2 pins
#define motor2_step_pin 5
const int endStopPin3 = 21; // End switches motor 2
const int endStopPin4 = 20;                                  
AccelStepper stepper2 = AccelStepper(1, motor2_step_pin, motor2_dir_pin);

// Stepper Travel Variables
long Motor1_distancesteps;  // Used to store the motor1distance in steps value entered in the Serial Monitor
long Motor2_distancesteps;  // Used to store the motor2distance in steps value entered in the Serial Monitor
long Motor1_distancemm;
long Motor2_distancemm;

// driver settings 
const int motor1_steps_per_rev = 200;  // steps per revolution
const int motor1_microsteps = 1;  // microsteps per step
const int motor1_steps_per_mm = (motor1_steps_per_rev * motor1_microsteps) / 8;  // steps per mm

const int motor2_steps_per_rev = 200;  // steps per revolution
const int motor2_microsteps = 1;  // microsteps per step
const int motor2_steps_per_mm = (motor2_steps_per_rev * motor2_microsteps) / 8;  // steps per mm


void setup() {
  //enable the use of the internal pullup resistors
  pinMode(endStopPin1, INPUT_PULLUP);
  pinMode(endStopPin2, INPUT_PULLUP);
  pinMode(endStopPin3, INPUT_PULLUP);
  pinMode(endStopPin4, INPUT_PULLUP);
  // initialise endstops for safety, these will interupt the entire program
  attachInterrupt(digitalPinToInterrupt(18), endswitch, RISING);
  attachInterrupt(digitalPinToInterrupt(19), endswitch, RISING);
  attachInterrupt(digitalPinToInterrupt(20), endswitch, RISING);
  attachInterrupt(digitalPinToInterrupt(21), endswitch, RISING);
  Serial.begin(9600);  // Start the Serial monitor with speed of 9600 Bauds
  
// Print out Instructions on the Serial Monitor at Start
  Serial.println("Enter motor distances seperated by a comma: motor1,motor2 ");
  Serial.print("Enter Move Values Now: ");

//  Set Max Speed and Acceleration of each Steppers
  stepper1.setMaxSpeed(20000.0);      // Set Max Speed of motor1
  stepper1.setAcceleration(10000.0);  // Acceleration of X motor1
  stepper1.setCurrentPosition(0);

  stepper2.setMaxSpeed(20000.0);      // Set Max Speed of motor2
  stepper2.setAcceleration(10000.0);  // Acceleration of motor2
  stepper2.setCurrentPosition(0);

}


void loop() {
int endStopState1 = digitalRead(endStopPin1); // read the state of the end stop switch for motor 1
int endStopState2 = digitalRead(endStopPin2);
int endStopState3 = digitalRead(endStopPin3); // read the state of the end stop switch for motor 2
int endStopState4 = digitalRead(endStopPin4);
bool Neg_motor1 = false;
bool Neg_motor2 = false;  

while ((Serial.available()>0))  { // Check if values are available in the Serial Buffer

  if (Serial.available() > 0) { // Check if there is any serial data available
    String data = Serial.readStringUntil('\n'); // Read the data until a newline character is received
    if (data.length() > 0) { // Check if any data was received
      int commaIndex = data.indexOf(','); // Find the index of the comma separator
      if (commaIndex != -1) { // Check if the comma separator was found
        // Extract the two distances from the received string and store them in separate variables
        String dist1Str = data.substring(0, commaIndex);
        String dist2Str = data.substring(commaIndex + 1);
        Motor1_distancemm = dist1Str.toInt();
        Motor2_distancemm = dist2Str.toInt();

        // Check if the first character of the distance string is a minus sign
        if (dist1Str.charAt(0) == '-') {
          Serial.print("- detected for first motor  -> ");
          //Motor1_distancemm = -Motor1_distancemm; // Negate the distance value
          Neg_motor1 = true;
          Serial.print("Neg_motor1 = true  ");
        }
        else{
          Neg_motor1 = false;
          Serial.print("Neg_motor1 = false  ");
        }
        
        if (dist2Str.charAt(0) == '-') {
          Serial.print("- detected for second motor  -> ");
          //Motor2_distancemm = -Motor2_distancemm; // Negate the distance value
          Neg_motor2 = true;
          Serial.print("Neg_motor2 = true" );
        }
        else{
          Neg_motor2 = false;
          Serial.print("Neg_motor2 = false  ");
        }

        // Print the received distances to the serial monitor
        Serial.print("Received distances: ");
        Serial.print(Motor1_distancemm);
        Serial.print(", ");
        Serial.println(Motor2_distancemm);
      }
    }
  }
  // For some reason the first negation symbol detection doesn't always work, this piece of code fixes it (this way the motor can move backwards if wanted)
  if(Neg_motor1 = true) {
    Motor1_distancesteps = Motor1_distancemm * motor1_steps_per_mm;
    Serial.print(Motor1_distancesteps);
    Serial.print(" motor1 distance in steps , ");
  }
  else{
    Motor1_distancesteps = Motor1_distancemm * motor1_steps_per_mm;
    Serial.print(Motor1_distancesteps);
    Serial.print(" motor1 distance in steps , ");
  }

  if(Neg_motor2 = true){
    Motor2_distancesteps = Motor2_distancemm * motor2_steps_per_mm;
  Serial.print(Motor2_distancesteps);  
  Serial.println(" motor2 distance in steps ");
  }
  else {
    Motor2_distancesteps = Motor2_distancemm * motor2_steps_per_mm;
    Serial.print(Motor2_distancesteps);  
    Serial.println(" motor2 distance in steps ");
  }
  
  
  stepper1.moveTo(Motor1_distancesteps);  // Set new move position for Stepper 1
  stepper2.moveTo(Motor2_distancesteps);  // Set new move position for Stepper 2
  
  Serial.print("Moving Steppers into position...");
  }

  // Check if the Steppers have reached desired position
  if ((stepper1.distanceToGo() != 0) || (stepper2.distanceToGo() !=0)) {
    
    stepper1.run();  // Move Stepper 1 into position
    stepper2.run();  // Move Stepper 2 into position
    
  }
}

void endswitch(){
  // safety if carriage hits an endswitch
  stepper1.stop();
  stepper2.stop();
  Serial.println("Program interrupted because of endswitch");
}