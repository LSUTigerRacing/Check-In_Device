#include <SPI.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <time.h>

//File Structure will be year/month.json can change later if needed
void YearFolder_init(int year);

//Add to the month's file with the entry of the user and when the user entered
void addTimestamp(String user, struct tm* time_info);
