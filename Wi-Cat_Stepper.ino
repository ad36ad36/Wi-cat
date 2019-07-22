/******
Adrian: Hello Wi-Cat team. This is the main source code for our project. 
7/18/2019 This code currently contains a mismash of code from examples, and probably will not compile into anything useful. 
There are a lot of comments about how different lines of code pertain to the different examples. Source code to the examples is
provided in the GitHub.
******/

 
#include "config.h" // Configures WiFi connection. Edit for your Network settings. 

/* For Time Scheduled Servings
#include "time.h" 
const char* ntpServer = "pool.ntp.org"; 
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
*/
const int   MAX_SERVING = 200; // make max serving corresponsd to the size of average bowl?

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution
// for your motor

int minutes;
int hours;
bool toggle = false;


Stepper myStepper(stepsPerRevolution, 14, 15, 32, 33); 

AdafruitIO_Feed *stepper_feed = io.feed("stepper"); 
AdafruitIO_Feed *schedule_feed = io.feed("schedule");
//AdafruitIO_Feed *serving_feed = io.feed("serving");
AdafruitIO_Feed *toggle_feed = io.feed("toggle");

void setup()
{

  myStepper.setSpeed(10);  // set the speed at 60 rpm:
  // Serial.begin(115200); // Starts serial connection //from Servo_ECE112
  Serial.begin(9600);  // initialize the serial port:
  
  while(! Serial); // Waits for serial monitor to open
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'Stepper' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  toggle_feed->onMessage(toggleControl);
  stepper_feed->onMessage(stepperControl);
  
    // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
    }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  stepper_feed->get();
  
  }

void loop() { 

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();
 
}
 
// this function is called whenever a 'stepper' message
// is received from Adafruit IO. it was attached to
// the stepper feed in the setup() function above.

void stepperControl(AdafruitIO_Data *data) {
  
  if(toggle == true) {
    
    int serving = data->toInt();      // convert the data to intege
   
    if(serving < 0)
        serving = 0;
      else if(serving > MAX_SERVING)
        serving = MAX_SERVING;
    myStepper.step(serving);
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
  
  void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  minutes = timeinfo.tm_min;  
  hours = timeinfo.tm_hour; 
}
  
  //init and get the time
 // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  // printLocalTime();

  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true);
  //WiFi.mode(WIFI_OFF);
  


