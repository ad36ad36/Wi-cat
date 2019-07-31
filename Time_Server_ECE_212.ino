//Wi-Cat (ECE 212 Project)
//Accessing time server and storing values

#include <WiFi.h>
#include "time.h"

const char* ssid       = "dw123";
const char* password   = "1998meborn";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;

int minutes;
int hours;



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
  Serial.begin(115200);
  
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  //WiFi.disconnect(true); //For some reason these couple lines keep the ESP32 from connecting to the internet
  //WiFi.mode(WIFI_OFF);
}

void loop()
{
  Serial.println(minutes);
  Serial.println(hours);
}
