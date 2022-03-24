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
	LOG("Starting Free db\n");
	if(db==NULL){
		ERROR("ERROR!!! FREEd before\n");
		return ;
	}
	db->capacity = 0; db->current = 0;
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
		memset(db->buf+db->current, 0, len);
	}
	memcpy(db->buf+db->current, buf, len);
	db->current += len;
}

void add_dynamic_buffer(dynamic_buffer *db, size_t len){
	if(db->current + len > db->capacity){
		int req_len = db->current + len;
		int cap = db->capacity;
		while(req_len > cap) cap += DEFAULT_CAPACITY;
		db->buf = (char *) realloc(db->buf, cap);
		db->capacity = cap;
		memset(db->buf+db->current, 0, len);
	}
}

void print_dynamic_buffer(dynamic_buffer *db){
	if(db==NULL){
		ERROR("DB NULL\n");
		return ;
	}
	if(db->buf==NULL){
		ERROR("DB->BUF NULL\n");
		return ;
	}
	LOG("PRINT Dynamic Buffer %ld\n%s" ,db->current,db->buf);
}

void reset_dynamic_buffer(dynamic_buffer *db){
	db->capacity = DEFAULT_CAPACITY;
	memset_dynamic_buffer(db);
}
