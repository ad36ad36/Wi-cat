//This code was written by Danny W. and 
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.

#include "config.h" //config.h contains WiFi networking setup


//set up variables to hold user's schedule
int hours1;
int minutes1;
int hours2;
int minutes2;

bool change; //boolean to check if a user wants to change schedule
int indexx = 1; //indexx for different times of the day

// set up all feeds
AdafruitIO_Feed *button = io.feed("button");
AdafruitIO_Feed *test = io.feed("test");
AdafruitIO_Feed *text = io.feed("text");

void setup() {
  // start the serial connection
  Serial.begin(115200);

  // wait for serial monitor to open
  while(! Serial);

  Serial.print("Connecting to Adafruit IO");

  io.connect(); // connect to io.adafruit.com


  button->onMessage(changeSchedule);
  text->onMessage(handleText);

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  
  io.run(); //necessary to run A

  int sum = hours1 + minutes1;   //verifying that inputs have been given and saved correctly

  Serial.println(sum);
  
  sum = hours2 + minutes2;
  Serial.println(sum);
  delay(1000);
}

//checks if user has pressed button indicating a change in schedule
void changeSchedule (AdafruitIO_Data *data) {
  //asks for input for morning schedule
  test->save("What time to feed in the morning?");
  change == true;
  while ( change ) { //infinite loop to wait for user input
    if (change == false)
      break;
  }
  test->save("saved");

  //asks for input for evening schedule
  indexx++;
  test->save("What time to feed in the evening?");
  change == true;
  while ( change ) {
    if (change == false)
      break;
  }
  test->save("saved");
}

//function to handle text input and stores values
void handleText (AdafruitIO_Data *data) {
  if (change == true) { //makes sure if any accidental input from text box doesn't change schedule
    //converts data into a string
    String text1 = data->toString();
    // Serial.println(text1);
    //int number = text1.toInt();
    
    //convert string into char array
    char text2[text1.length()+1];
    text1.toCharArray(text2, text1.length()+1);
    char *strings[2];
    char *ptr = NULL;

    // Serial.println(text2);
    //test->save("what time to feed");
    
    //separates input into hours and minutes
    ptr = strtok(text2 , ":");
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
    change == false;
  }
}
