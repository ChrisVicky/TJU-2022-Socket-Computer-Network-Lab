
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
void ErrorLog(char * msg, struct sockaddr_in cli_addr){
    time_t now; time(&now); char *Time = ctime(&now); Time[strlen(Time)-1] = '\0';
    ERRORLOG("[%s] [Error] [pid %5d:tid %16ld] [Client %12s:%-7d] %s\n" ,Time,getpid(),pthread_self(),inet_ntoa(cli_addr.sin_addr),(int) ntohs(cli_addr.sin_port),msg);
}

void AccessLog(char *msg, struct sockaddr_in cli_addr,char *method, int code){
    time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1] = '\0';
  	ACCESSLOG("[%s] [Access] [pid %5d:tid %16ld] [Client %12s:%-7d] %s %d %s\n" ,Time,getpid(),pthread_self(), inet_ntoa(cli_addr.sin_addr), (int) ntohs(cli_addr.sin_port), method,code, msg);
}



