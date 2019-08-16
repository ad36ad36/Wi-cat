//This code was written by Danny W., Ranvir, Adrian, and by Todd Treece// Copyright (c) 2016 Adafruit Industries
#include <Arduino.h>
#include "BasicStepperDriver.h"
//#include <Stepper.h>
#include "time.h"
#include "config.h"

//Pin variables
const int ledPin = 17; // GPIO pin for ON/OFF LED
const int buzzer = 16; // GPIO pin for active buzzer
//Pins 33 (STEP) and 15 (DIR) are used for motor


//Buzzer variables
bool buzzerOn = 1;
bool buzzerState = 0;
unsigned long buzzTimer;


//initialize global variables for storing user input schedule
int hours1, minutes1, hours2, minutes2;

//initializes global variables for actual time according to the Global Time Server
int ahours, aminutes, aseconds;

//Time variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*-8; //multiply this by an integer based on time zone (ex. GMT -8, our time, do -8*3600)
const int   daylightOffset_sec = 3600;

bool change; //boolean to check if a user wants to change schedule
int indexx = 1; //indexx for different times of the day

//Motor Variables
bool toggle = false; //checks whether the device is "on" or "off" ???????????????????
const int stepsPerRevolution = 200;  //specification of motor (found by 360/(stride angle of motor)
int serving_size = 0;  //good practice to initialize non-constant global variables to a reasonable value
//Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33); //set up stepper motor
BasicStepperDriver myStepper(stepsPerRevolution, 15, 33);

//set up all feeds for Adafruit IO
AdafruitIO_Feed *serving_size_feed = io.feed("serving-size"); 
AdafruitIO_Feed *toggle_feed = io.feed("toggle");
AdafruitIO_Feed *schedule_reset_feed = io.feed("schedule-reset");
AdafruitIO_Feed *schedule_input_feed = io.feed("schedule-input");
AdafruitIO_Feed *display_feed = io.feed("display");
AdafruitIO_Feed *manual_motor_feed = io.feed("manual-motor");


/***********************************SETUP***********************************************/

void setup() {
   pinMode(buzzer, OUTPUT); //initialize the buzzer pin as an output
   pinMode(ledPin, OUTPUT); //initialize LED pin as output
   Serial.begin(115200);   // start the serial connection

   //connect to WiFi
   Serial.printf("Connecting to %s ", WIFI_SSID);
   WiFi.begin(WIFI_SSID, WIFI_PASS);
   while (WiFi.status() != WL_CONNECTED) {
      //delay(500);
      Serial.print(".");
   }
   Serial.println(" CONNECTED");
   
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
   printLocalTime();
   

   //myStepper.setSpeed(60);  // set the speed of the motor
   myStepper.begin(20, 1); //set the speed and microstepping respectively // maybe 10?
   

   while(! Serial);   // wait for serial monitor to open
  
  Serial.print("Connecting to Adafruit IO");
  io.connect(); // connect to io.adafruit.com

  //runs functions based on what feeds receive data
  schedule_reset_feed->onMessage(changeSchedule);
  schedule_input_feed->onMessage(handleInput);
  toggle_feed->onMessage(toggleControl);
  serving_size_feed->onMessage(handleServingSize);
  manual_motor_feed->onMessage(motorTest); //

   
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    //delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
 // serving_size_feed->get(); //??????
}

/*************LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOP************/

void loop() {
  
  io.run(); //checks io for any incoming data
  
  //checks if user wants to change schedule
  if (change == true) {
    display_feed->save("What time to feed in the morning?");
    //waits for user to provide an input through the text block on the IO
    while (change == true) {
      io.run();
      Serial.println(".");
      if (change == false) {
        break;
      }
    }
    delay(1000);
    display_feed->save("saved");
    delay(1000);
    
    //asks for input for evening schedule
    indexx++;
    display_feed->save("What time to feed in the evening?");
    change = true;
    while ( change == true ) {
      io.run();
      Serial.println(".");
      if (change == false)
        break;
    }
    delay(1000);
    display_feed->save("saved");
    indexx = 1;
  }

  printLocalTime();
  //Serial.println((String)"Test: ahours: " +ahours+ " aminutes: " +aminutes+ " aseconds: " +aseconds); //check real time
  //Serial.println((String)"Test: hours1: " +hours1+ " minutes1: " +minutes1); //check input one
  //Serial.println((String)"Test: hours2: " +hours2+ " minutes2: " +minutes2); //check input two
  
   if(ScheduleCheck()) {    
         StepperControl(serving_size);
      }

   if(buzzerState) {
      Buzz();
   }
   
}

/**********************************************Change Schedule*****************************************************/

//checks if user has pressed the button indicating a change in schedule
void changeSchedule (AdafruitIO_Data *data) {
  change = true;
}

/*************************************************Handling Input*******************************************/

//function to handle schedule_input_feed input and stores values
void handleInput (AdafruitIO_Data *data) {
  if (change == true) { //makes sure if any accidental input from schedule_input_feed box doesn't change schedule
    //converts data into a string
    String schedule_input_feed1 = data->toString();
    
    //convert string into char array
    char schedule_input_feed2[schedule_input_feed1.length()+1];
    schedule_input_feed1.toCharArray(schedule_input_feed2, schedule_input_feed1.length()+1);
    char *strings[2];
    char *ptr = NULL;
    
    //separates input into hours and minutes
    ptr = strtok(schedule_input_feed2 , ":");
    strings[1] = ptr;
    ptr = strtok(NULL , "");
    strings[2] = ptr;

    
    //convert char array into integers
    //stores time for morning schedule
    if (indexx == 1) {
      hours1 = atoi(strings[1]);
      minutes1 = atoi(strings[2]);
      Serial.println((String) "Morning schedule: " hours1+":"minutes1);;
    }
    //stores time for evening schedule
    else if (indexx == 2) {
      hours2 = atoi(strings[1]);
      minutes2 = atoi(strings[2]);
      Serial.println((String) "Evening schedule: " hours2+":"minutes2);
    }
    if ( ((hours1 >= 0) && (hours1 <= 24)) && ((hours2 <= 24) && (hours2 >= 0)) && 
         ((minutes1 >= 0) && (minutes1 <=59)) && ((minutes2 <= 59) && (minutes2 >= 0)) ) {
      change = false;
    }
    else {
      delay(1000);
      display_feed->save("Invalid Input");
    }
  }
}

/**************************************************Handle Serving Size******************************/

void handleServingSize(AdafruitIO_Data *data) {
  
  if(toggle == true) {
    
    serving_size = data->toInt();      // convert the data to intege
   
  }

}

/**************************************Stepper Control 2.0********************************/

void StepperControl(int serving_size_x) {
  if(toggle == true) {
    buzzerState = 1;
    buzzTimer = millis() + 10*1000;
    myStepper.move(serving_size_x/4);  //need to scale serving_size_x or serving_size to motor speed etc.
    myStepper.move(-serving_size_x/4));  //need to scale serving_size_x or serving_size to motor speed etc
    myStepper.move(serving_size_x/4));  //need to scale serving_size_x or serving_size to motor speed etc.
    myStepper.move(-serving_size_x/4));  //need to scale serving_size_x or serving_size to motor speed etc.

    Serial.println("step");
  }

}

/**************************************Stepper Control********************************

void StepperControl(int serving_size_x) {
  if(toggle == true) {
    buzzerState = 1;
    buzzTimer = millis() + 10*1000;
    myStepper.step(serving_size_x*stepsPerRevolution);  //need to scale serving_size_x or serving_size to motor speed etc.
    Serial.println("step");
  }

}

****************************************ON/OFF switch*********************************/

//ON/OFF function of device ; determine whether device will run
void toggleControl(AdafruitIO_Data *data) {  
  
  // handle the toggle on the Adafruit IO Dashboard
  String toggleString = data->toString();
  if(toggleString == String("ON")) {
    Serial.println("ON");
    digitalWrite(ledPin, HIGH); //for LED
    toggle = true;
  } 
  else {
    Serial.println("OFF");
    digitalWrite(ledPin, LOW);
    toggle = false;
  }
  
}

/*********************************Print real time*****************************/

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  aminutes = timeinfo.tm_min;
  ahours = timeinfo.tm_hour;
  aseconds=timeinfo.tm_sec;
}

/**************************************Schedule Check*****************************/

bool ScheduleCheck(){
  
 if ( (((hours1==ahours) && (minutes1==aminutes)) || ((hours2==ahours) && (minutes2==aminutes))) && (aseconds==0) ) 
    return true;
   else
    return false;
}

/**************************************Buzz*****************************/

void Buzz(){ //Buzz is called in the StepperControl function
   //output a frequency
   if (millis() < buzzTimer) {
      digitalWrite(buzzer,buzzerOn); 
         if ((millis() % 5) < 1) {      //delay(5);  //wait for 1ms
            buzzerOn = !buzzerOn;      
         }
   }
   else{ 
      buzzerState = 0;
      digitalWrite(buzzer, LOW); 
   }
}

void motorTest(AdafruitIO_Data *data){
   
     // handle the toggle on the Adafruit IO Dashboard
  String toggleString = data->toString();
  if(toggleString == String("ON")) {
  Serial.println("Motor test running!");
  StepperControl(serving_size);
  } 
  else {
    Serial.println("Motor test is not running");
 
  }
  
   
}
