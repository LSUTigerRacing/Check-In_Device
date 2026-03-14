#include <SPI.h>
#include <SD.h>
#include <CSV_Parser.h>
#include <time.h>

//File Structure will be year/month.json can change later if needed
void YearFolder_init(int year);

//Add to the month's file with the entry of the user and when the user entered
void addTimestamp(String user, struct tm* time_info);

//Creates a csv file for user look up
void userSDInit(String userList);

//Changes value in the UserList.csv file depending on location as well return the user's status after tapping their RFID card
bool checkForPresense(String id, bool office, bool shop);