#include <Arduino.h>
#include "time.h"
#include <WiFi.h>
#include "MySD.h"
#define SD_CS_PIN 9
const char* ssid = " ";
const char* ntp_server = "pool.ntp.org";
//This puts the time to GMT-6 which correlates to CST
const long gmtOffset_sec = -21600;
const long daylightOffset_sec = 3600;
struct tm time_info;
int main(){
    Serial.begin(115200);
      if(SD.begin(SD_CS_PIN)){
    Serial.println("SD card module detected");
  }
  else{
    Serial.println("SD card module not detected");
  }
    File fyle = SD.open("pp.txt",FILE_WRITE);
    fyle.printf("%s","test");
}