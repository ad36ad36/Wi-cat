#include <WiFi.h> //may be included in config.h already
#include "time.h"  
#include "config.h"

#if defined(ARDUINO_ARCH_ESP32)
  // ESP32Servo Library (https://github.com/madhephaestus/ESP32Servo)
  // installation: library manager -> search -> "ESP32Servo"
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

// pin used to control the servo
#define SERVO_PIN 27

const char* ssid       = "";  //may be included in config.h already
const char* password   = "";  //may be included in config.h already

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

int minutes;
int hours;

// create an instance of the servo class
Servo servo;

// set up the 'servo' and 'schedule_input'  and 'serving_size' feeds
//
AdafruitIO_Feed *servo_feed = io.feed("servo"); 
AdafruitIO_Feed *schedule_feed = io.feed("schedule");
AdafruitIO_Feed *serving_feed = io.feed("serving");


void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  minutes = timeinfo.tm_min;  
  hours = timeinfo.tm_hour; 
}

void setup()
{
  Serial.begin(115200); // Starts serial connection
  
  while(! Serial); // Waits for serial monitor to open
  
  servo.attach(SERVO_PIN); // See config.h for SERVO_PIN
 
  /* //THIS CODE IS PROBABLY INCLUDED IN config.h THROUGH AdafruitIO_WiFi io() function or class?
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
   */
  
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();
  
  // set up a message handler for the 'servo' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  toggle_feed->onMessage(toggleControl);
  servo_feed->onMessage(servoControl);
  
    // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
    }
  
    // we are connected
  Serial.println();
  Serial.println(io.statusText());
  servo_feed->get();
  
  }
  // Serial.println(" CONNECTED"); // This is from ECE_212_Time_Server.ino


  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

//https://www.arduino.cc/reference/en/language/structure/sketch/loop/

void loop() //arduino specific function for a continuous input checking loop
{
  Serial.println(minutes);
  Serial.println(hours);
}
