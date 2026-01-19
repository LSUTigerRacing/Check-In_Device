#include "MySD.h"
#include <WiFi.h>
#include <WebServer.h>
#include "LSUWiFi.h"
#include "SupabaseWrapper.h"

#define SD_CS_PIN 9

const char* ssid = "eduroam";

#define EAP_IDENTITY "@lsu.edu"
#define EAP_USERNAME "@lsu.edu"
#define EAP_PASSWORD "pass"

const char* ntp_server = "pool.ntp.org";
//This puts the time to GMT-6 which correlates to CST
const long gmtOffset_sec = -21600;
const long daylightOffset_sec = 3600;
struct tm time_info;
WebServer server(80);

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n\n=== ESP32 WPA2 Enterprise Web Server ===");

  LSUWiFiSetup(ssid, EAP_IDENTITY, EAP_USERNAME, EAP_PASSWORD);
  
  Serial.print("Waiting for connection");
  bool connected = WiFiWaitForConnection(60);
  Serial.println();

  if (connected) {
    Serial.println("\n\u2713 WPA2 ENTERPRISE SUCCESS!");
    Serial.print("Server IP: ");
    Serial.println(WiFi.localIP());

    // initialize supabase
    supabaseBegin("SupabaseURL", "SupabaseKey");
    configTime(gmtOffset_sec,daylightOffset_sec,ntp_server);
    setupRootHandler(server);
    server.begin();
    Serial.println("Web server started successfully.");
  } else {
    Serial.println("\n\u2717 FAILED TO CONNECT");
    Serial.print("Final Status Code: ");
    WiFiPrintStatus(WiFiStatus());
  }

  if(SD.begin(SD_CS_PIN)){
    Serial.println("SD card module detected");
    getLocalTime(&time_info);
    YearFolder_init(time_info.tm_hour);
  }
  else{
    Serial.println("SD card module not detected");
  }

}

void loop() {
  if (WiFiIsConnected()) {
    server.handleClient();
  }
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 10000) {
    if (WiFiIsConnected()) {
      Serial.printf("Status: CONNECTED | IP: %s | RSSI: %d\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
      AddUserSupabase("asd"); // will be changed later
      supabaseResetQuery();
    } else {
      Serial.print("Status: ");
      WiFiPrintStatus(WiFiStatus());
    }
    lastPrint = millis();
  }

  bool check_in = true;
  if(check_in){
    getLocalTime(&time_info);
    addTimestamp("Bobby Hill",&time_info); //Replace Bobby Hill with name variable
  }
}
 