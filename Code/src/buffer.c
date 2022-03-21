/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：buffer.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：动态缓冲区具体实现
*
*****************************************************************/


#include "buffer.h"
void init_dynamic_buffer(dynamic_buffer * db){
	db->buf = (char *) malloc(sizeof(char) * DEFAULT_CAPACITY);
	db->capacity = DEFAULT_CAPACITY;
	db->current = 0;
	memset(db->buf, 0, DEFAULT_CAPACITY);
}

void memset_dynamic_buffer(dynamic_buffer * db){
	memset(db->buf, 0, db->capacity);
	db->current = 0;
}

void free_dynamic_buffer(dynamic_buffer * db){
	free(db->buf);
	free(db);
}

void append_dynamic_buffer(dynamic_buffer * db, char * buf, size_t len){
	if(db->current + len > db->capacity){
		int req_len = db->current + len;
		int cap = db->capacity;
		while(req_len > cap) cap += DEFAULT_CAPACITY;
		db->buf = (char *) realloc(db->buf, cap);
		db->capacity = cap;
	}
	memcpy(db->buf+db->current, buf, len);
	db->current += len;
}



