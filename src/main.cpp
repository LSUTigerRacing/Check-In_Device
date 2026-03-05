#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "LSUWiFi.h"
#include "SupabaseWrapper.h"
#include <SPI.h>
#include <MFRC522.h>
#include "led_functions.h"

#define RST_PIN         5           
#define SS_PIN          10          
#define MISO            13
#define MOSI            11
#define SCLK            12

const char* ssid = "eduroam";

#define EAP_IDENTITY "@lsu.edu"
#define EAP_USERNAME "@lsu.edu"
#define EAP_PASSWORD "pass"

WebServer server(80);
MFRC522 mfrc522(SS_PIN,RST_PIN);

void setup() {
  Serial.begin(115200);
  while(!Serial);                       
  SPI.begin(SCLK,MISO,MOSI,SS_PIN);
  delay(200);                                           
  mfrc522.PCD_Init();                                             
  mfrc522.PCD_DumpVersionToSerial();

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

  //Creates and sets the key to default key
  MFRC522::MIFARE_Key key; 
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF; 

  byte block; 
  byte len; 
  MFRC522::StatusCode status; //Track the status of the RFID 

  //If no card then it restarts the loop
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  //If there is a card then it reads its UID, else then it exits
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  
  //Red light indicating card is detected and then turns off after 1 second
  setColor(255, 0, 0);
  Serial.println(F("**Card Detected:**")); 
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid)); 
  
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
  
  Serial.print(F("Name: ")); 
  delay(1000);
  setColor(0, 0, 0);

  byte trailerBlock = 7;
  block = 4;

  //Sets up the aunthetication
  status = mfrc522.PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A,
    trailerBlock,
    &key,
    &(mfrc522.uid)
  );

  //If status falled for authentication, then authentication fails and blinks red to indicate so
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    flashColor(255, 0, 0, 3, 100, 100);
    return;
  }

  byte buffer1[18]; 
  block = 4; 
  len = 18; 
  status = mfrc522.MIFARE_Read(block, buffer1, &len); //Reads data of MIFARE_Read and stores it into buffer1

  //If the card being read was failed then the code returns to the loop itself until the card was read successfully 
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status)); 
      flashColor(255, 0, 0, 3, 100, 100); //Blinking red LED indicates the reading failed
    return;
  }
  else{
    String uid;
    for(uint8_t i = 0; i < 16; i++){
      uid += buffer1[i];
      Serial.write(buffer1[i]); //For debugging
    }
    //Add SD check for both to see if user has entered
    bool present;

    //Unhighlight based on deployment
    if(present){
      RemoveInOffice(uid);
      //RemoveInShop(uid);
      flashColor(0, 0, 255, 3, 100, 100); //blinks blue 3 times
    }
    else{
      AddInOffice(uid);
      //AddInShop(uid);
      flashColor(0, 255, 0, 3, 100, 100); //blinks green 3 times
    }
  }

  Serial.println(F("\n**End Reading**\n")); 

  //Delays for 1 second and stops everything else 
  delay(1000); 
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
