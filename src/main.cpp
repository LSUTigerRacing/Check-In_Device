#include "MySD.h"
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
    String userList;
    userSDInit(userList);
  }
  else{
    Serial.println("SD card module not detected");
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
 
  //If status falled for MIFARE_Write, then the write fails and blinks red to indicate so 
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Write failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    flashColor(255, 0, 0, 3, 100, 100);
    return;
  }

  Serial.println(F("Write success"));

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
  char *userID = (char*) buffer1;
  userID[17] = '\0';
  bool check_in = checkForPresense(userID,true,false);
  getLocalTime(&time_info);
  addTimestamp(userID,&time_info,check_in);
  //Blinking green LED indicates the card being read was a success 
  flashColor(0, 255, 0, 3, 100, 100);
  //If the card being read was a success then it goes to each byte from the card and prints the bytes to the serial monitor
  for (uint8_t i = 0; i < 16; i++) {
    Serial.write(buffer1[i] );
  }

  Serial.println(F("\n**End Reading**\n")); 

  //Delays for 1 second and stops everything else 
  delay(1000); 
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
 