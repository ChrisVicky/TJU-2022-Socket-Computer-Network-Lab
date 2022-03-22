/*****************************************************************
 *   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
 *   
 *   文件名称：utils.c
 *   创 建 者：Christopher Liu  1051666563@qq.com
 *   创建日期：2022年03月15日
 *   描    述：
 *
 *****************************************************************/

//#define SUCCESS(format, ...) printf("\33[1;33m" format "\33[0m",## __VA_ARGS__)
#define ERROR(format, ...) \
	do {\
	time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1]='\0';\
	printf("\33[1;31m[%s] [DEBUG ERROR] " format "\33[0m",Time, ## __VA_ARGS__);}while(0)
#define ERRORLOG(format, ...) printf("\33[1;31m" format "\33[0m", ## __VA_ARGS__)
#define ACCESSLOG(format, ...) printf("\33[1;36m" format "\33[0m", ## __VA_ARGS__)
#define LOG(format, ...) \
	do{\
	time_t now; time(&now); char *Time=ctime(&now); Time[strlen(Time)-1]='\0';\
	printf("\33[1;36m[%s] [DEBUG LOG] " format "\33[0m",Time, ## __VA_ARGS__);}while(0)
#define PRINT(format, ...) printf("\33[1;37m" format "\33[0m", ## __VA_ARGS__)



