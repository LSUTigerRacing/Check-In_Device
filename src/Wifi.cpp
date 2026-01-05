
#include "LSUWiFi.h"
#include <esp_wifi.h>
#include <esp_wpa2.h>
#include <cstring>

static const char* g_ssid = nullptr;

void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("--- IP ADDRESS OBTAINED --- IP: ");
      Serial.println(WiFi.localIP());
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("--- DISCONNECTED --- Attempting to reconnect...");
      if (g_ssid) WiFi.begin(g_ssid);
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

static WebServer* g_server = nullptr;

void handleRootWrapper() {
  if (g_server) {
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
    g_server->send(200, "text/html", html);
  }
}

void setupRootHandler(WebServer& server) {
  g_server = &server;
  server.on("/", handleRootWrapper);
}

void LSUWiFiSetup(const char* ssid, const char* identity, const char* username, const char* password) {
  g_ssid = ssid;

  WiFi.onEvent(WiFiEvent);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  delay(1000);

  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.println("Configuring WPA2 Enterprise...");

  esp_wifi_sta_wpa2_ent_set_identity((uint8_t *)identity, strlen(identity));
  esp_wifi_sta_wpa2_ent_set_username((uint8_t *)username, strlen(username));
  esp_wifi_sta_wpa2_ent_set_password((uint8_t *)password, strlen(password));
  esp_wifi_sta_wpa2_ent_clear_ca_cert();
  esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_MSCHAPV2);
  esp_wifi_sta_wpa2_ent_enable();

  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid);
}

bool WiFiWaitForConnection(int timeoutSeconds) {
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < timeoutSeconds) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  return WiFi.status() == WL_CONNECTED;
}

bool WiFiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

int WiFiStatus() {
  return WiFi.status();
}

void WiFiPrintStatus(int status) {
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
