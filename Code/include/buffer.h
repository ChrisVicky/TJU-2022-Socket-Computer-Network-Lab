/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：buffer.h
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：创建动态的缓冲区，避免用户请求过长导致的缓冲区溢出问题
*
*****************************************************************/

#ifndef BUFFER_H
#define BUFFER_H
#include<string.h>
#include<stdlib.h>
#include "util.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
//#define DEFAULT_CAPACITY 1024
#define DEFAULT_CAPACITY 10
// #define DEBUG
typedef struct dynamic_buffer{
	char *buf;
	size_t capacity;
	size_t current; // current end
	size_t access_end; // The end of current access_end --> in parse
}dynamic_buffer;

void init_dynamic_buffer(dynamic_buffer *);
void memset_dynamic_buffer(dynamic_buffer *);
void free_buffer_dynamic_buffer(dynamic_buffer*);
void free_dynamic_buffer(dynamic_buffer *);
void append_dynamic_buffer(dynamic_buffer *, char *, size_t);
void add_dynamic_buffer(dynamic_buffer*, size_t); 
void print_dynamic_buffer(dynamic_buffer*);
void reset_dynamic_buffer(dynamic_buffer*);
void update_dynamic_buffer(dynamic_buffer*);
void catpart_dynamic_buffer(dynamic_buffer*, dynamic_buffer*, int, int);


#endif
