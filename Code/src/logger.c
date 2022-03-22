
/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：
*
*****************************************************************/

#include "logger.h"

char *error_sample = "[Fri Sep 09 10:42:29.902022 2011] [core:error] [pid 35708:tid 4328636416] [client 72.15.99.187] File does not exist: /usr/local/apache2/htdocs/favicon.ico";
void ErrorLog(char * msg, char *cli_addr, int port){
    time_t now;
    time(&now);
    char *Time = ctime(&now); Time[strlen(Time)-1] = '\0';
    printf("[%s] [core:error] [pid %5d:tid%16ld] [client %s:%d] %s\n" ,Time,getpid(),pthread_self(),cli_addr,port,msg);
}

void AccessLog(char *msg, char*cli_addr, int port){
    time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1] = '\0';
}

/*
int main(){
    char input[2000];
    while(1){
        scanf("%s" ,input);
        ErrorLog(input);
        printf("\n");
    }
}

*/

