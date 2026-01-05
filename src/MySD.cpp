#include "MySD.hpp"

void createMonthFolder(){

}

void createYearFolder(){

}

void addTimeStamp(int userID, String day){
    day += ".json";
    char file_name[day.length() + 6];
    strcpy(file_name, day.c_str()); 
    SD.open(file_name, FILE_WRITE);
}