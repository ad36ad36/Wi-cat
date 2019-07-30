#include <Stepper.h>

//This code was written by Danny W., Ranvir, Adrian, 
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.


/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME   "inukealot"
#define IO_KEY        "0d8142ca7e074d50adf5781f991ea0fd"

#define WIFI_SSID   "dw123"
#define WIFI_PASS   "1998meborn"

#include "AdafruitIO_WiFi.h"

#if defined(USE_AIRLIFT) || defined(ADAFRUIT_METRO_M4_AIRLIFT_LITE)
  // Configure the pins used for the ESP32 connection
  #if !defined(SPIWIFI_SS) // if the wifi definition isnt in the board variant
    // Don't change the names of these #define's! they match the variant ones
    #define SPIWIFI SPI
    #define SPIWIFI_SS 10  // Chip select pin
    #define SPIWIFI_ACK 9  // a.k.a BUSY or READY pin
    #define ESP32_RESETN 6 // Reset pin
    #define ESP32_GPIO0 -1 // Not connected
  #endif
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS, SPIWIFI_SS, SPIWIFI_ACK, ESP32_RESETN, ESP32_GPIO0, &SPIWIFI);
#else
  AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
#endif

//initialize all variables
int hours1;
int minutes1;
int hours2;
int minutes2;
bool change; //boolean to check if a user wants to change schedule
int indexx = 1; //indexx for different times of the day
const int   MAX_SERVING = 200; // make max serving corresponsd to the size of average bowl?
const int stepsPerRevolution = 48;  //specification of motor (found by 360/(stride angle of motor)
bool toggle = false; //checks whether the device is "on" or "off"

Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33); //set up stepper motor

//set up all feeds for Adafruit IO
AdafruitIO_Feed *serving_size_feed = io.feed("serving-size"); 
AdafruitIO_Feed *toggle_feed = io.feed("toggle");
AdafruitIO_Feed *schedule_reset_feed = io.feed("schedule-reset");
AdafruitIO_Feed *schedule_input_feed = io.feed("schedule-input");
AdafruitIO_Feed *display_feed = io.feed("display");


void setup() {
   myStepper.setSpeed(10);  // set the speed at 60 rpm:
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  io.connect(); // connect to io.adafruit.com

  //runs functions based on what feeds receive data
  schedule_reset_feed->onMessage(changeSchedule);
  schedule_input_feed->onMessage(handleInput);
  toggle_feed->onMessage(toggleControl);
  serving_size_feed->onMessage(StepperControl);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  serving_size_feed->get();
}


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
    display_feed->save("saved");
    delay(500);
    
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
    delay(500);
    display_feed->save("saved");
  }

}


//checks if user has pressed the button indicating a change in schedule
void changeSchedule (AdafruitIO_Data *data) {
  change = true;
}


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
      Serial.println(hours1);
      Serial.println(minutes1);
    }
    //stores time for evening schedule
    else if (indexx == 2) {
      hours2 = atoi(strings[1]);
      minutes2 = atoi(strings[2]);
      Serial.println(hours2);
      Serial.println(minutes2);
    }
    if ( ((hours1 >= 0) || (hours2 >= 0)) && ((hours1 <= 24) || (hours2 <= 24)) && 
         ((minutes1 >= 0) || (minutes2 >=0)) && ((minutes1 >= 59) || (minutes2 <= 59)) ) {
      change = false;
    }
    else {
      display_feed->save("Invalid Input");
    }
  }
  
}


void StepperControl(AdafruitIO_Data *data) {
  
  if(toggle == true) {
    
    int serving = data->toInt();      // convert the data to intege
   
    if(serving < 0)
        serving = 0;
      else if(serving > MAX_SERVING)
        serving = MAX_SERVING;
    myStepper .step(serving);
  }

}


//ON/OFF function of device ; determine whether device will run
void toggleControl(AdafruitIO_Data *data) {  
  
  // handle the toggle on the Adafruit IO Dashboard
  String toggleString = data->toString();
  if(toggleString == String("ON")) {
    Serial.println("ON");
    toggle = true;
  } 
  else {
    Serial.println("OFF");
    toggle = false;
  }
  
}
