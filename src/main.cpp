#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include "led_functions.h"

#define RST_PIN         5           
#define SS_PIN          10          
#define MISO            13
#define MOSI            11
#define SCLK            12
   
MFRC522 mfrc522(SS_PIN,RST_PIN);
//Starts communication with RFID reader and computer, prepares everything, and tells user to scan card
void setup() {
  Serial.begin(115200);
  while(!Serial);                       
  SPI.begin(SCLK,MISO,MOSI,SS_PIN);
  delay(200);                                           
  mfrc522.PCD_Init();                                             
  mfrc522.PCD_DumpVersionToSerial();
}

void loop() {

  //LED staying off early in case of LED glitches
  setColor(0, 0, 0);

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
  Serial.print(F("Name: ")); 
  delay(1000);
  setColor(0, 0, 0);

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