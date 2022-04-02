/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.h
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：Response.h and Response.c Both deal with Request Recieved. 
*
*****************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<sys/stat.h>
#include<ctype.h>
#include<sys/mman.h>
#include "buffer.h"
#include "parse.h"
#include "util.h"
#include "logger.h"

typedef enum Return_value{
	CORRECT,
	ERROR,
	CLOSE,
	CLOSE_FROM_CLIENT,
	PERSISTENT,
	NOT_COMPLETE,
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

typedef enum TYPE{
	HTML,
	CSS,
	PNG,
	JPEG,
	GIF,
	ICO,
	NONE
}TYPE;

int handle_request(int, int, dynamic_buffer*, struct sockaddr_in, dynamic_buffer*);
void helper_head(Request*, dynamic_buffer*, struct sockaddr_in);

int handle_get(Request*, dynamic_buffer*, struct sockaddr_in, int);
int handle_head(Request*, dynamic_buffer*, struct sockaddr_in, int);
void handle_post(Request*, dynamic_buffer*, struct sockaddr_in, int, dynamic_buffer*);

void handle_400(dynamic_buffer*, struct sockaddr_in);
void handle_404(dynamic_buffer*, struct sockaddr_in);
void handle_501(dynamic_buffer*, struct sockaddr_in);
void handle_505(dynamic_buffer*, struct sockaddr_in);

METHODS method_switch(char *);
TYPE get_TYPE(char *);

char *get_header_value(Request*, char*);
void free_request(Request*);

void set_response(dynamic_buffer*, char*, char*);
void set_header(dynamic_buffer*, char*, char*);
void set_msg(dynamic_buffer*, char*, int);

void get_time(char*,size_t);
void get_last_modified(struct stat, char*, size_t);
int get_file_content(dynamic_buffer*, char*);
TYPE get_file_type(char*, char*);

/***********************  CGI ISSUE  *****************************/

#define FILENAME "FileName"
/* note: null terminated arrays (null pointer) */

int handle_cgi_get(Request*, dynamic_buffer*, struct sockaddr_in,  dynamic_buffer*, int);
int handle_cgi_post(Request*, dynamic_buffer*, struct sockaddr_in,  dynamic_buffer*);

char *get_header_value(Request*, char*);


