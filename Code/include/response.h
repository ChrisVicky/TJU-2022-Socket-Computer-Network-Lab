/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.h
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：
*
*****************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/stat.h>
#include "buffer.h"
#include "parse.h"
#include "util.h"
#include "logger.h"

typedef enum Return_value{
	ERROR,
	CLOSE,
	PERSISTENT
}Return_value;

typedef enum Connection_status{
	KeepAlive,
	Close
}Connection_status;

typedef enum METHODS{
	GET,
	HEAD,
	POST,
	NOT_SUPPORTED
}METHODS;

int handle_request(int, int, dynamic_buffer*, struct sockaddr_in);
void helper_head(Request*, dynamic_buffer*, struct sockaddr_in);

void handle_get(Request*, dynamic_buffer*, struct sockaddr_in, int);
void handle_head(Request*, dynamic_buffer*, struct sockaddr_in, int);
void handle_post(Request*, dynamic_buffer*, struct sockaddr_in, int);

void handle_400(dynamic_buffer*, struct sockaddr_in);
void handle_404(dynamic_buffer*, struct sockaddr_in);
void handle_501(dynamic_buffer*, struct sockaddr_in);
void handle_505(dynamic_buffer*, struct sockaddr_in);

METHODS method_switch(char *);


char *get_header_value(Request*, char*);
void free_request(Request*);

void set_response(dynamic_buffer*, char*, char*);
void set_header(dynamic_buffer*, char*, char*);
void set_msg(dynamic_buffer*, char*);

void get_time(char*,size_t);
void get_last_modified(struct stat, char*, size_t);
void get_file_content(char*, char*, int);

