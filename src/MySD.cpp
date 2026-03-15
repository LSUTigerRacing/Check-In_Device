#include "MySD.h"

//Helper function to create a new file inside year folder when initializing 
void fileCreate(char *path, String month){
    for(int i = 0; i < month.length(); i++){
        path[6+i] = month.charAt(i);
    }
    File file = SD.open(path,FILE_WRITE);
    file.printf("%s","User,Day,Time,Present\n");
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


void addTimestamp(String user,struct tm* timestamp,bool present){
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
    String entry = user + ',' + timestamp->tm_mday + ',' + timestamp->tm_hour + ':' + timestamp->tm_min + ',' + user + "\n";
    month_file.printf("%s",entry.c_str());
    month_file.close();
}

void userSDInit(String userList){
    if(SD.exists("UserList.csv")){
        SD.remove("UserList.csv");
    }
    File userFile = SD.open("UserList.csv",FILE_WRITE);
    String user; 
    userFile.printf("%s","ID,Name,InOffice,InShop,Ack\n");
    //Replace 50 with actual row length
    for(int i =0; i <user.length(); i + 50 ) {
        user = userList.substring(i, i + i );
        userFile.printf("%s", user);
        userFile.print("\n");
    }
    userFile.close();
}

bool checkForPresence(String id, bool office, bool shop){
    const char* idPtr = id.c_str();
    File userList = SD.open("UserList.csv",FILE_READ);
    char* csv_str = (char *)malloc(strlen("ID,Name,InOffice,InShop,Ack\n") + 1);
    strcpy(csv_str,"ID,Name,InOffice,InShop,Ack\n");
    while(userList.available()){
        char *entry = (char*)malloc(sizeof(char));
        entry[0]= '\0';
        bool newEntry = false;
        while(!newEntry){
            entry = (char*)realloc(entry, strlen(entry) + 2);
            uint8_t charRead[2] = {0,0};
            userList.read(charRead,sizeof(char));
            strcat(entry,(char*) charRead);
            if(charRead[0] == '\n'){
                newEntry = true;
                break;
            }
        }
        csv_str = (char*)realloc(csv_str, strlen(csv_str) + strlen(entry) + 1);
        strcat(csv_str,entry);
        free(entry);
        entry = NULL;
    }
    CSV_Parser csvP(csv_str,"ud-ucuc-");
    free(csv_str);
    csv_str = NULL;
    char **strings = (char**)csvP["ID"];
    uint8_t *officeVal = (uint8_t *)csvP["InOffice"];
    uint8_t *shopVal = (uint8_t *)csvP["InShop"];
    bool present = false;
    for(int i = 0; i < csvP.getRowsCount(); i++){
        if(strcmp(strings[i],idPtr) == 0){
            if(office){
            if(officeVal[i] == 1){
                officeVal[i] = 0;
                present = false;
                }
            else{
                officeVal[i] = 1;
                present = true;
                }
            }
        else if(shop){
            if(shopVal[i] == 1){
                shopVal[i] = 0;
                present = false;
                }
            else{
                shopVal[i] = 1;
                present = true;
                }
            }
        }
    }
    userList.close();
    return present;
}