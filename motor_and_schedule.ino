//This code was written by Danny W., Ranvir, Adrian, and by Todd Treece// Copyright (c) 2016 Adafruit Industries

#include <Stepper.h>
#include "time.h"
#include "config.h"


//initialize global variables for storing user input schedule
int hours1;
int minutes1;
int hours2;
int minutes2;

//initializes global variables for actual time according to the Global Time Server
int ahours;
int aminutes;


//Time variables
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600*-8; //multiply this by an integer based on time zone (ex. GMT -8, our time, do -8*3600)
const int   daylightOffset_sec = 3600;

bool change; //boolean to check if a user wants to change schedule
int indexx = 1; //indexx for different times of the day

bool toggle = false; //checks whether the device is "on" or "off"
//const int   MAX_SERVING = 200; // make max serving corresponsd to the size of average bowl?
const int stepsPerRevolution = 48;  //specification of motor (found by 360/(stride angle of motor)
int serving_size = 0;  //good practice to initialize non-constant global variables to a reasonable value


Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33); //set up stepper motor

//set up all feeds for Adafruit IO
AdafruitIO_Feed *serving_size_feed = io.feed("serving-size"); 
AdafruitIO_Feed *toggle_feed = io.feed("toggle");
AdafruitIO_Feed *schedule_reset_feed = io.feed("schedule-reset");
AdafruitIO_Feed *schedule_input_feed = io.feed("schedule-input");
AdafruitIO_Feed *display_feed = io.feed("display");

/***********************************SETUP***********************************************/

void setup() {
   
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
   
   myStepper.setSpeed(10);  // set the speed at 60 rpm:
  
   Serial.begin(115200);   // start the serial connection

   while(! Serial);   // wait for serial monitor to open
  
  Serial.print("Connecting to Adafruit IO");
  io.connect(); // connect to io.adafruit.com

  //runs functions based on what feeds receive data
  schedule_reset_feed->onMessage(changeSchedule);
  schedule_input_feed->onMessage(handleInput);
  toggle_feed->onMessage(toggleControl);
  serving_size_feed->onMessage(handleServingSize);

   
  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
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
  }

  if(ScheduleCheck()) {    
      StepperControl(serving_size);
   }
   
  // FOR TESTING  
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println(aminutes);
  Serial.println(ahours);
  // END TESTING
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

/**************************************Stepper Control********************************/

void StepperControl(int serving_size_x) {
  
  if(toggle == true) {
    myStepper.step(serving_size_x);  //need to scale serving_size_x or serving_size to motor speed etc. 
  }

}

/****************************************ON/OFF switch*********************************/

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
}

/**************************************Schedule Check*****************************/

bool ScheduleCheck(){
  
 if ( (hours1==ahours1) && (minutes1==aminutes1) && (hours2==ahours2) && (minutes2==aminutes2)) 
    return true;
   else
        return false;
   
}
