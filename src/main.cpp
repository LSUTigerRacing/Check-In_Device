#include "esp_wifi.h"
#include "esp_wpa2.h"  
#include <cstdlib>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPSupabase.h>

const char* ssid = "eduroam";

Supabase db;

#define EAP_IDENTITY "@lsu.edu"
#define EAP_USERNAME "@lsu.edu"
#define EAP_PASSWORD "pass" 

WebServer server(80);

void handleRoot() {
  Serial.println("Serving client request for root path...");
  String html = R"raw(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>ESP32 Hello World</title>
        <style>
            body { font-family: Inter, sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #f3f4f6; }
            .container { text-align: center; background: white; padding: 40px; border-radius: 12px; box-shadow: 0 10px 25px rgba(0, 0, 0, 0.1); }
            h1 { color: #10b981; font-size: 2.5rem; }
            p { color: #6b7280; }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>Hello World from ESP32!</h1>
            <p>Connected via WPA2 Enterprise (eduroam).</p>
        </div>
    </body>
    </html>
  )raw";
  server.send(200, "text/html", html);
}


void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("--- IP ADDRESS OBTAINED --- IP: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("--- DISCONNECTED --- Attempting to reconnect...");
      WiFi.begin(ssid);
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("WiFi station started");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to access point, waiting for IP...");
      break;
    default:
      Serial.printf("[WiFi-event] event: %d\n", event);
      break;
  }
}

void printWiFiStatus(int status) {
  switch(status) {
    case WL_IDLE_STATUS: Serial.println("WL_IDLE_STATUS"); break;
    case WL_NO_SSID_AVAIL: Serial.println("WL_NO_SSID_AVAIL"); break;
    case WL_CONNECTED: Serial.println("WL_CONNECTED"); break;
    case WL_CONNECT_FAILED: Serial.println("WL_CONNECT_FAILED"); break;
    case WL_CONNECTION_LOST: Serial.println("WL_CONNECTION_LOST"); break;
    case WL_DISCONNECTED: Serial.println("WL_DISCONNECTED"); break;
    default: Serial.print("UNKNOWN STATUS: "); Serial.println(status); break;
  }
}


void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n\n=== ESP32 WPA2 Enterprise Web Server ===");

  WiFi.onEvent(WiFiEvent);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.println("Configuring WPA2 Enterprise...");

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_USERNAME, strlen(EAP_USERNAME));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
  esp_wifi_sta_wpa2_ent_clear_ca_cert();
  esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_MSCHAPV2);
  esp_wifi_sta_wpa2_ent_enable();


  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid);

  int attempts = 0;
  Serial.print("Waiting for connection");
  while (WiFi.status() != WL_CONNECTED && attempts < 60) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WPA2 ENTERPRISE SUCCESS!");
    Serial.print("Server IP: ");
    Serial.println(WiFi.localIP());
    db.begin("SupabaseURL","SupabaseKey"); //change this to appropriate values

    server.on("/", handleRoot);
    
    server.begin();
    Serial.println("Web server started successfully.");
    
  } else {
    Serial.println("\n✗ FAILED TO CONNECT");
    Serial.print("Final Status Code: ");
    printWiFiStatus(WiFi.status());
  }
}

void AddUserSupabase(String user){
  String t = "[{\"name\": " + user + ", \"created_at\": \"2025-11-07T01:43:47.178071+00:00\"}]";
  int code  = db.insert("esp",t,false); //(String: table name, String: JSON, Bool: Upsert )
  Serial.println(code);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // IMPORTANT: This line is required to keep the web server responsive
    server.handleClient();
  }
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 10000) { 
    if (WiFi.status() == WL_CONNECTED) {
      Serial.printf("Status: CONNECTED | IP: %s | RSSI: %d\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
      AddUserSupabase("asd"); //this adds user every 10 secs, will be modified soon
      db.urlQuery_reset(); //IMPORTANT: this is required after every supabase exec

    } else {
      Serial.print("Status: ");
      printWiFiStatus(WiFi.status());
    }
    lastPrint = millis();
  }
}
