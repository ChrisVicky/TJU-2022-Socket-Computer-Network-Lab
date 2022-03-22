/*****************************************************************
*   Copyright (C) 2022 TJU Liu Jinfan. All rights reserved.
*   
*   文件名称：response.c
*   创 建 者：Christopher Liu  1051666563@qq.com
*   创建日期：2022年03月21日
*   描    述：
*
*****************************************************************/

#include "response.h"

char *my_http_version = "HTTP/1.1";
char *space = " ";
char *crlf = "\r\n";
char *colon = ":";




int handle_request(int client_sock, int sock, dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	Request *request = parse(dbuf->buf, dbuf->current, client_sock);
	
	// 400 Error Parsing
	if(request == NULL){
		handle_400(dbuf);
		return CLOSE;
	}
	
	// check connection:close
	Return_value return_value = PERSISTENT;
	Connection_status connection_status = KeepAlive;
	char *connection_value = get_header_value(request, "Connection");
	if(!strcmp(connection_value, "Close")){
		return_value = CLOSE;
		connection_status = Close;
	}
	
	// 505 Http version not supported
	if(strcmp(my_http_version, request->http_version)){
		handle_505(dbuf);
		free_request(request);
		return CLOSE;
	}
	
	// Methods, 501 Method not supported.
	switch(method_switch(request->http_method)){
		case GET:
			handle_get(request, dbuf);
			break;
		case HEAD:
			handle_head(request, dbuf);
			break;
		case POST:
			handle_post(request, dbuf);
			break;
		default:
			handle_501(dbuf);
			free_request(request);
			return CLOSE;
	}
	return return_value;
} 

// Handler --> Get, Post, Head
void handle_get(Request *request, dynamic_buffer *buf){
	return ;
}

void handle_head(Request *request, dynamic_buffer *buf){
	return ;
}

void handle_post(Request *request, dynamic_buffer *buf){
	return ;
}

// Error Number Handler
void handle_400(dynamic_buffer *dbuf){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "400", "Bad request");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
}

void handle_404(dynamic_buffer *dbuf){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "404", "Not Found");
	set_msg(dbuf, crlf);
}

void handle_501(dynamic_buffer *dbuf){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "501", "Not Implemented");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
}

void handle_505(dynamic_buffer *dbuf){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "505", "HTTP Version not supported");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
}

// utility: get header value
char *get_header_value(Request *request, char *header_name){
	int i;
	for(i=0;i<request->header_count;i++){
		if(!strcmp(request->headers[i].header_name, header_name))
			return request->headers[i].header_value;
	}
	return "";
}

// switch methods
METHODS method_switch(char *method){
	if(!strcmp(method, "GET")) return GET;
	if(!strcmp(method, "HEAD")) return HEAD;
	if(!strcmp(method, "POST")) return POST;
	return NOT_SUPPORTED;
}

// Free Request
void free_request(Request * request){
	if(request==NULL) return;
	free(request->headers);
	free(request);
	return ;
}


// Set Response, Header, msg
void set_response(dynamic_buffer *dbuf, char *code, char *description){
	append_dynamic_buffer(dbuf, my_http_version, strlen(my_http_version));
	append_dynamic_buffer(dbuf, space, strlen(space));
	append_dynamic_buffer(dbuf, code, strlen(code));
	append_dynamic_buffer(dbuf, space, strlen(space));
	append_dynamic_buffer(dbuf, description, strlen(description));
	append_dynamic_buffer(dbuf, crlf, strlen(crlf));
	return ;
}

void set_header(dynamic_buffer *dbuf, char *key, char *value){
	append_dynamic_buffer(dbuf, key, strlen(key));
	append_dynamic_buffer(dbuf, colon, strlen(colon));
	append_dynamic_buffer(dbuf, space, strlen(space));
	append_dynamic_buffer(dbuf, value, strlen(value));
	append_dynamic_buffer(dbuf, crlf, strlen(crlf));
	return ;
}

void set_msg(dynamic_buffer *dbuf, char* msg){
	append_dynamic_buffer(dbuf, msg, strlen(msg));
}

