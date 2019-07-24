/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME   ""
#define IO_KEY        ""

/******************************* WIFI **************************************/

// the AdafruitIO_WiFi client will work with the following boards:
//   - HUZZAH ESP8266 Breakout -> https://www.adafruit.com/products/2471
//   - Feather HUZZAH ESP8266 -> https://www.adafruit.com/products/2821
//   - Feather HUZZAH ESP32 -> https://www.adafruit.com/product/3405
//   - Feather M0 WiFi -> https://www.adafruit.com/products/3010
//   - Feather WICED -> https://www.adafruit.com/products/3056
//   - Adafruit PyPortal -> https://www.adafruit.com/product/4116
//   - Adafruit Metro M4 Express AirLift Lite -> https://www.adafruit.com/product/4000
//   - Adafruit AirLift Breakout -> https://www.adafruit.com/product/4201

#define WIFI_SSID   "CenturyLink7376_5G"
#define WIFI_PASS   "bf8d8ha73f64f"

// uncomment the following line if you are using airlift
// #define USE_AIRLIFT

// uncomment the following line if you are using winc1500
// #define USE_WINC1500

// comment out the following lines if you are using fona or ethernet
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
/******************************* FONA **************************************/

// the AdafruitIO_FONA client will work with the following boards:
//   - Feather 32u4 FONA -> https://www.adafruit.com/product/3027

// uncomment the following two lines for 32u4 FONA,
// and comment out the AdafruitIO_WiFi client in the WIFI section
// #include "AdafruitIO_FONA.h"
// AdafruitIO_FONA io(IO_USERNAME, IO_KEY);

/**************************** ETHERNET ************************************/

// the AdafruitIO_Ethernet client will work with the following boards:
//   - Ethernet FeatherWing -> https://www.adafruit.com/products/3201

// uncomment the following two lines for ethernet,
// and comment out the AdafruitIO_WiFi client in the WIFI section
// #include "AdafruitIO_Ethernet.h"
// AdafruitIO_Ethernet io(IO_USERNAME, IO_KEY);

 
//#include "config.h" // Configures WiFi connection. Edit for your Network settings. 

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
  

  


