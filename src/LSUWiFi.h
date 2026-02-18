#pragma once

#include <WiFi.h>
#include <WebServer.h>

// Initialize WPA2 Enterprise WiFi and start connection
void LSUWiFiSetup(const char* ssid, const char* identity, const char* username, const char* password);

// Wait for connection, Returns true if connected.
bool WiFiWaitForConnection(int timeoutSeconds);

// Check connection status
bool WiFiIsConnected();
int WiFiStatus();

// Print WiFi status
void WiFiPrintStatus(int status);

// Setup HTTP server handler for root path
void setupRootHandler(WebServer& server);
