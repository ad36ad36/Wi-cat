//Wi-Cat (ECE 212 Project)
//stepper motor control

#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor

int buttonApin = 21; //assign button GPIO pin

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33);

void setup() {
  pinMode(buttonApin, INPUT_PULLUP); //assign button pin as an input
  
  // set the speed at 60 rpm:
  myStepper.setSpeed(10);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {

  //run only when the button has been pressed
  if (digitalRead(buttonApin) == LOW)
  {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution/4);
  delay(500);
  }
//
//  // step one revolution in the other direction:
//  Serial.println("counterclockwise");
//  myStepper.step(-stepsPerRevolution/2);
//  delay(500);
}
