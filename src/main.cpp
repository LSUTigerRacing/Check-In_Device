#include <WiFi.h>
#include <WebServer.h>
#include "LSUWiFi.h"
#include "SupabaseWrapper.h"

const char* ssid = "eduroam";

#define EAP_IDENTITY "@lsu.edu"
#define EAP_USERNAME "@lsu.edu"
#define EAP_PASSWORD "pass"

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

    setupRootHandler(server);
    server.begin();
    Serial.println("Web server started successfully.");
  } else {
    Serial.println("\n\u2717 FAILED TO CONNECT");
    Serial.print("Final Status Code: ");
    WiFiPrintStatus(WiFiStatus());
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
      int code = supabaseAddUser("asd"); // will be changed later
      Serial.println(code);
      supabaseResetQuery();
    } else {
      Serial.print("Status: ");
      WiFiPrintStatus(WiFiStatus());
    }
    lastPrint = millis();
  }
}
