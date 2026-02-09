#include "MySD.h"

//Helper function to create a new file inside year folder when initializing 
void fileCreate(char *path, String month){
    for(int i = 0; i < month.length(); i++){
        path[6+i] = month.charAt(i);
    }
    File file = SD.open(path,FILE_WRITE);
    file.printf("%s","Day,Time,User\n");
    file.close();
}

void YearFolder_init(int year){
    char year_path[5];
    itoa(year+1900,year_path,10);
    if(SD.exists(year_path)){
        Serial.println("Year Folder already exists");
        return;
    }
    SD.mkdir(year_path);
    char month_fp[13];
    std::copy(year_path,year_path + 5, month_fp);
    strcat(month_fp, "/");
    fileCreate(month_fp, "Jan.csv");
    fileCreate(month_fp, "Feb.csv");  
    fileCreate(month_fp, "Mar.csv");
    fileCreate(month_fp, "Apr.csv");
    fileCreate(month_fp, "May.csv");
    fileCreate(month_fp, "Jun.csv");
    fileCreate(month_fp, "Jul.csv");
    fileCreate(month_fp, "Aug.csv");
    fileCreate(month_fp, "Sep.csv");
    fileCreate(month_fp, "Oct.csv");
    fileCreate(month_fp, "Nov.csv");
    fileCreate(month_fp, "Dec.csv");
}


void addTimestamp(String user,struct tm* timestamp){
    char month_filepath[13];
    itoa(timestamp->tm_year+1900,month_filepath,10);
    switch(timestamp->tm_mon){
        case 0:
            strcat(month_filepath,"/Jan.csv");
            break;
        case 1:
            strcat(month_filepath,"/Feb.csv");
            break;
        case 2:
            strcat(month_filepath,"/Mar.csv");
            break;
        case 3:
            strcat(month_filepath,"/Apr.csv");
            break;
        case 4:
            strcat(month_filepath,"/May.csv");
            break;
        case 5:
            strcat(month_filepath,"/Jun.csv");
            break;
        case 6:
            strcat(month_filepath,"/Jul.csv");
            break;
        case 7:
            strcat(month_filepath,"/Aug.csv");
            break;
        case 8:
            strcat(month_filepath,"/Sep.csv");
            break;
        case 9:
            strcat(month_filepath,"/Oct.csv");
            break;
        case 10:
            strcat(month_filepath,"/Nov.csv");
            break;
        case 11:
            strcat(month_filepath,"/Dec.csv");
            break;
    }
    File month_file = SD.open(month_filepath, FILE_WRITE);
    if(month_file){
        Serial.print("Failed to open ");
        Serial.println(month_filepath);
    }
//    JsonDocument doc;
//    serializeJson(doc,month_file);
    String entry = timestamp->tm_mday + ',' + timestamp->tm_hour + ':' + timestamp->tm_min + ',' + user + "\n";
    month_file.printf("%s",entry.c_str());
    month_file.close();
}
