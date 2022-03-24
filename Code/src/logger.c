
/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：简化日志记录
*   TODO:
*   	直接输出到文件中的宏定义
*
*****************************************************************/

#include "logger.h"

/**
 * @brief  对应 阿帕奇 ERROR LOG
 *
 * @param msg 		: Error Msg
 * @param cli_addr	: Client's Address
 */
void ErrorLog(char * msg, struct sockaddr_in cli_addr){
    time_t now; time(&now); char *Time = ctime(&now); Time[strlen(Time)-1] = '\0';
    ERRORLOG("[%s] [Error] [pid %5d:tid %16ld] [Client %12s:%-7d] %s\n" ,Time,getpid(),pthread_self(),inet_ntoa(cli_addr.sin_addr),(int) ntohs(cli_addr.sin_port),msg);
}

/**
 * @brief 对应 阿帕奇 Access Log
 *
 * @param msg		: Access Msg 
 * @param cli_addr	: Client's Address
 * @param method	: Methods used in the Access Request
 * @param code		: Access Codes ( 200 ETC )
 */
void AccessLog(char *msg, struct sockaddr_in cli_addr,char *method, int code){
    time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1] = '\0';
  	ACCESSLOG("[%s] [Access] [pid %5d:tid %16ld] [Client %12s:%-7d] %s %d %s\n" ,Time,getpid(),pthread_self(), inet_ntoa(cli_addr.sin_addr), (int) ntohs(cli_addr.sin_port), method,code, msg);
}



