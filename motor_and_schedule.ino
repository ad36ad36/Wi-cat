//This code was written by Danny W, Ranvir, Adrian, and Todd Treece for Adafruit Industries


#include <Stepper.h>


int hours1;
int minutes1;
int hours2;
int minutes2;

bool change; //boolean to check if a user wants to change schedule
int indexx = 1; //indexx for different times of the day

const int   MAX_SERVING = 200; // make max serving corresponsd to the size of average bowl?

const int stepsPerRevolution = 48;  // change this to fit the number of steps per revolution
// for your motor

bool toggle = false;

Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33); 

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


  schedule_reset_feed->onMessage(changeSchedule);
  schedule_input_feed->onMessage(handleschedule_input_feed);
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
//  Serial.println(sum);
//  delay(500);
  io.run();
  if (change == true) {
    display_feed->save("What time to feed in the morning?");
    while (change == true) {
      io.run();
      Serial.println(".");
      if (change == false) {
        break;
      }
    }
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
  delay(500);
  display_feed->save("saved");
  }
  
}

//checks if user has pressed schedule_reset_feed indicating a change in schedule
void changeSchedule (AdafruitIO_Data *data) {
  
  change = true;
}

//function to handle schedule_input_feed input and stores values
void handleschedule_input_feed (AdafruitIO_Data *data) {
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
    change = false;
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

//https://www.arduino.cc/reference/en/language/structure/sketch/loop/
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
