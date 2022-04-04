/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：buffer.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：动态缓冲区具体实现
*
*****************************************************************/
#define DEBUG

#include "buffer.h"
void init_dynamic_buffer(dynamic_buffer * db){
	db->buf = (char *) malloc(DEFAULT_CAPACITY*sizeof(char*));
	db->capacity = DEFAULT_CAPACITY;
	db->current = 0;
	db->access_end = 0;
	memset(db->buf, 0, DEFAULT_CAPACITY*sizeof(char));
}

void memset_dynamic_buffer(dynamic_buffer * db){
	memset(db->buf, 0, db->capacity);
	db->current = 0;
}
void free_buffer_dynamic_buffer(dynamic_buffer *db){
	if(db==NULL){
		ERROR("Not exists!\n");
		return;
	}
	if(db->buf==NULL){
		ERROR("buffer IS NULL\n");
		return ;
	}
	free(db->buf);
}
void free_dynamic_buffer(dynamic_buffer * db){

#ifdef DEBUG
	LOG("Starting Free db\n");
#endif
	if(db==NULL){
		ERROR("ERROR!!! FREEd before\n");
		return ;
	}
	db->capacity = 0; db->current = 0;
	if(db->buf==NULL){
		ERROR("buf not initialized\n");
		return ;
	}
	if(strlen(db->buf))
		free(db->buf);
	free(db);
}

void append_dynamic_buffer(dynamic_buffer * db, char * buf, size_t len){
//	PRINT4("GGGGGGGG\n");
//	PRINT4("Cureent Len: %ld vs %ld, buf: %s\n" ,db->capacity,db->current+len,buf);
	add_dynamic_buffer(db, len);
	strncpy(db->buf + db->current, buf, len);
	db->current += len;
	db->buf[db->current] = '\0';
//	print_dynamic_buffer(db);
//	LOG("End Append\n");
}

void add_dynamic_buffer(dynamic_buffer *db, size_t len){
	if(db->current + len > db->capacity){
		int req_len = db->current + len;
		int cap = db->capacity;
		while(req_len > cap) cap += DEFAULT_CAPACITY;
	//	PRINT4("from %ld --> %d, Strlen:%ld\n" ,db->capacity, cap, strlen(db->buf));
		db->buf = (char *) realloc(db->buf, cap*sizeof(char*));
		memset(db->buf+db->capacity, 0, cap - db->capacity);
		db->capacity = cap;
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
	helper(30, '-');printf("\n");
	PRINT("PRINT Dynamic Buffer, size: %ld:\n>>%s<<\n" ,db->current,db->buf);
	helper(30, '-');printf("\n");
}

void reset_dynamic_buffer(dynamic_buffer *db){
	//free_buffer_dynamic_buffer(db);
	init_dynamic_buffer(db);
}


/**
 * @brief Change the Head of a db to its access_end
 *
 * @param db
 */
void update_dynamic_buffer(dynamic_buffer *db){
//	LOG("Updating!\n");
//	print_dynamic_buffer(db);
	strcpy(db->buf, db->buf+db->access_end);
	db->current -= db->access_end;
//	db->capacity -= db->access_end;
	db->access_end = 0;
//	LOG("END Updating\n");
//	print_dynamic_buffer(db);
	/*
	print_dynamic_buffer(db);
	dynamic_buffer * tmp = (dynamic_buffer*) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(tmp);
	append_dynamic_buffer(tmp, front, strlen(front));
	init_dynamic_buffer(db);
	append_dynamic_buffer(db, tmp->buf, tmp->current);
	free_dynamic_buffer(tmp);*/
}

/**
 * @brief From L to R, Copy msg
 *
 * @param dest
 * @param src
 * @param l
 * @param len [l,l+len] (use strncpy)
 */
void catpart_dynamic_buffer(dynamic_buffer *dest, dynamic_buffer *src, int l, int len){
	add_dynamic_buffer(dest, len);
	strncpy(dest->buf, src->buf+l, len);
}
