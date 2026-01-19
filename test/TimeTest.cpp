#include <Arduino.h>
#include "time.h"
#include <WiFi.h>
#include "esp_twai.h"
const char* ssid = " ";
const char* ntp_server = "pool.ntp.org";
//This puts the time to GMT-6 which correlates to CST
const long gmtOffset_sec = -21600;
const long daylightOffset_sec = 3600;
struct tm time_info;
int main(){

}