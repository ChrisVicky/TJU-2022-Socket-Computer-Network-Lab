
/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述: Logger
*
*****************************************************************/

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<arpa/inet.h>
#include"util.h"

void ErrorLog(char *, struct sockaddr_in);
void AccessLog(char*,struct sockaddr_in,char*, int);

