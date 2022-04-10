/*****************************************************************
 *   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
 *   
 *   文件名称：utils.c
 *   创 建 者：Christopher Liu  1051666563@qq.com
 *   创建日期：2022年03月15日
 *   描    述：log 宏定义
 *
 *****************************************************************/

//#define SUCCESS(format, ...) printf("\33[1;33m" format "\33[0m",## __VA_ARGS__)
#include<stdio.h>
#include<string.h>
#include "parse.h"
#define ERROR(format, ...) \
	do {\
	time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1]='\0';\
	printf("\33[1;31m[%s] [DEBUG ERROR] " format "\33[0m",Time, ## __VA_ARGS__);}while(0)
#define ERRORLOG(format, ...) printf("\33[1;35m" format "\33[0m", ## __VA_ARGS__)
#define ACCESSLOG(format, ...) printf("\33[1;36m" format "\33[0m", ## __VA_ARGS__)
#define LEAVELOG(format, ...) printf("\33[1;34m" format "\33[0m", ## __VA_ARGS__)
#define ARRIVALLOG(format, ...) printf("\33[1;33m" format "\33[0m", ## __VA_ARGS__)

#define LOG2(format, ...) \
	do{\
	time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1]='\0';\
	printf("\33[1;35m[%s] [DEBUG LOG] " format "\33[0m",Time, ## __VA_ARGS__);}while(0)

#define LOG(format, ...) \
	do{\
	time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1]='\0';\
	printf("\33[1;36m[%s] [DEBUG LOG] " format "\33[0m",Time, ## __VA_ARGS__);}while(0)




#define PRINT(format, ...) printf("\33[1;37m" format "\33[0m", ## __VA_ARGS__)

#define PRINT3(format, ...) printf("\33[1;4m" format "\33[0m", ## __VA_ARGS__)

#define PRINT4(format, ...) printf("\33[1;32m" format "\33[0m", ## __VA_ARGS__)
 
#define PRINT2(format, ...) printf("\33[1;34m" format "\33[0m", ## __VA_ARGS__)

void helper(int, char);
void PRINTHEAD(int, int ,int);

