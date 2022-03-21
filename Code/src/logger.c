/*
 * @Description    : Logger.c --> For logging Issues
 * @Auther         : Christopher Liu
 * @LastEditors    : Do note edit
 * @Date           : 2022-03-21 16:58:34
 * @LastEditTime   : 2022-03-21 17:31:11
 * Copyright 2022 Christopher Liu, All Rights Reserved. 
 */

#include "logger.h"

char *error_sample = "[Fri Sep 09 10:42:29.902022 2011] [core:error] [pid 35708:tid 4328636416] [client 72.15.99.187] File does not exist: /usr/local/apache2/htdocs/favicon.ico";
void ErrorLog(char * msg){
    time_t now;
    time(&now);
    char *Time = ctime(&now); Time[strlen(Time)-1] = '\0';
    char *client = "127.0.0.1";
    printf("[%s] [core:error] [pid %5d:tid%16ld] [client %s] %s\n" ,Time,getpid(),pthread_self(),client,msg);
}

void AccessLog(char *msg){
    time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1] = '\0';
    char 
}


int main(){
    char input[2000];
    while(1){
        scanf("%s" ,input);
        ErrorLog(input);
        printf("\n");
    }
}


