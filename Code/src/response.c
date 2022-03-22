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

#define DEBUG

char *my_http_version = "HTTP/1.1";
char *space = " ";
char *crlf = "\r\n";
char *colon = ":";

char *server_info = "Liso, the friendly web server";



int handle_request(int client_sock, int sock, dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	Request *request = parse(dbuf->buf, dbuf->current, client_sock);

	// 400 Error Parsing
	if(request == NULL){
		handle_400(dbuf, cli_addr);
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
		handle_505(dbuf, cli_addr);
		free_request(request);
		return CLOSE;
	}

	// Methods, 501 Method not supported.
	switch(method_switch(request->http_method)){
		case GET:
			handle_get(request, dbuf, cli_addr, return_value);
			break;
		case HEAD:
			handle_head(request, dbuf, cli_addr, return_value);
			break;
		case POST:
			handle_post(request, dbuf, cli_addr, return_value);
			break;
		default:
			handle_501(dbuf, cli_addr);
			free_request(request);
			return CLOSE;
	}
	return return_value;
} 

// Handler --> Get, Post, Head
void handle_get(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
	// This should be modified if Get has other uri
	char * html_path = "/home/christopher/Programme/C/Web/webServerStartCodes-new/Code/static_site/index.html";
	
	struct stat file_buffer;
	if(stat(html_path, &file_buffer)){
		// File not Found
#ifdef DEBUG
		ERROR("Path %s\n" ,html_path);
#endif
		handle_404(dbuf, cli_addr);
		free_request(request);
		return;
	}
#define BUF_SIZE 256
	char time_buffer[BUF_SIZE]={0}; 
		get_time(time_buffer, BUF_SIZE);
	char last_modified[BUF_SIZE]={0}; 
		get_last_modified(file_buffer, last_modified, BUF_SIZE);
	char content_length[BUF_SIZE]={0};
		sprintf(content_length, "%ld", file_buffer.st_size);
	char content_type[] = "text"; // Only html
	// TODO: Support other type
#undef BUF_SIZE	


	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "200", "OK");
	set_header(dbuf, "Date", time_buffer);
	set_header(dbuf, "Server", server_info);
 	set_header(dbuf, "Last-Modified", last_modified);
	set_header(dbuf, "Content-Length", content_length);
	set_header(dbuf, "Content-Type", content_type);
	if(return_value==CLOSE){
		set_header(dbuf, "Connection", "Close");
	}else{
		set_header(dbuf, "Connection", "Keep-Alive");
	}

	// TODO: Open file and attach it to msg
	
	char file_content[4096];
	get_file_content(file_content, html_path, 4096);
	if(file_content==NULL){
		handle_404(dbuf, cli_addr);
		return ;
	}
	set_msg(dbuf, file_content);
	AccessLog("OK", cli_addr, "GET", 200);
	return ;
}
 
void handle_head(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
	// This should be modified if Get has other uri
	char * html_path = "/home/christopher/Programme/C/Web/webServerStartCodes-new/Code/static_site/index.html";
	
	struct stat file_buffer;
	if(stat(html_path, &file_buffer)){
		handle_404(dbuf, cli_addr);
		free_request(request);
		return;
	}
#define BUF_SIZE 256
	char time_buffer[BUF_SIZE]={0}; 
		get_time(time_buffer, BUF_SIZE);
	char last_modified[BUF_SIZE]={0}; 
		get_last_modified(file_buffer, last_modified, BUF_SIZE);
	char content_length[BUF_SIZE]={0};
		sprintf(content_length, "%ld", file_buffer.st_size);
	char content_type[] = "text"; // Only html
	// TODO: Support other type
#undef BUF_SIZE

	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "200", "OK");
	set_header(dbuf, "Date", time_buffer);
	set_header(dbuf, "Server", server_info);
 	set_header(dbuf, "Last-Modified", last_modified);
	set_header(dbuf, "Content-Length", content_length);
	set_header(dbuf, "Content-Type", content_type);
	if(return_value==CLOSE){
		set_header(dbuf, "Connection", "Close");
	}else{
		set_header(dbuf, "Connection", "Keep-Alive");
	}
	set_msg(dbuf, crlf);
	AccessLog("OK", cli_addr, "HEAD", 200);
	return ;
}

void handle_post(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
	/* Post: Just Echo back */
	AccessLog("Echo Back", cli_addr,"POST", 200);
	return ;
}

// Error Number Handler
void handle_400(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "400", "Bad request");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
	ErrorLog("400 Bad request", cli_addr);
}

void handle_404(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "404", "Not Found");
	set_msg(dbuf, crlf);
	ErrorLog("404 Not Found", cli_addr);
}

void handle_501(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "501", "Not Implemented");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
	ErrorLog("501 Not Implemented", cli_addr);
}

void handle_505(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "505", "HTTP Version not supported");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf);
	ErrorLog("505 HTTP Version not supported", cli_addr);
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

// Get Time
void get_time(char *time_buffer, size_t buffer_size){
	time_t now; 
	time(&now); 
	struct tm* Time=localtime(&now); 
	strftime(time_buffer, buffer_size, "%a, %d %b %Y %H:%M:%S %Z", Time);
}

// Get Last Modified
void get_last_modified(struct stat file_buffer, char * last_modified, size_t BUF_SIZE){
	struct tm *t = gmtime(&file_buffer.st_mtime);
	strftime(last_modified, BUF_SIZE, "%a, %d %b %Y %H:%M:%S %Z",t);
}

void get_file_content(char *dfbuf, char*path, int BUF_SIZE){
	int fd_in=0;
	if((fd_in=open(path, O_RDONLY))<0){
		ERROR("Cannot Open file '%s'\n" ,path);
		dfbuf = NULL;
		return;
	}
	struct stat file_stat;
	if((fstat(fd_in, &file_stat))<0){
		ERROR("Fetching Status of File '%s' Error\n", path);
		dfbuf = NULL;
		return;
	}
	size_t file_len = file_stat.st_size;
	if(file_len<=0){
		ERROR("File '%s' Empty\n" ,path);
		close(fd_in);
		dfbuf = NULL;
		return;
	}
	if(file_len > BUF_SIZE){
		ERROR("File Too large\n");
		close(fd_in);
		dfbuf = NULL;
		return ;
	}
	if(read(fd_in, dfbuf, BUF_SIZE)<1){
		ERROR("Error reading file '%s'\n" ,path);
		close(fd_in);
		dfbuf=NULL;
		return;
	}
	close(fd_in);
	return;

}


