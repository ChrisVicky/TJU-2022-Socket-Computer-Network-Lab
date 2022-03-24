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

char *server_info = "liso/1.0";
char *default_path = "./static_site";

/**
 * @brief Handle Requests mainly set 
 *
 * @param client_sock
 * @param sock
 * @param dbuf
 * @param cli_addr
 *
 * @return 
 */
int handle_request(int client_sock, int sock, dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	Request *request = parse(dbuf->buf, dbuf->current, client_sock);

	// 400 Error Parsing
	if(request == NULL){
		handle_400(dbuf, cli_addr);
		return PERSISTENT;
	}

	// check connection:close
	Return_value return_value = PERSISTENT;
	char *connection_value = get_header_value(request, "Connection");
	if(!strcmp(connection_value, "Close")){
		return_value = CLOSE;
	}

	// 505 Http version not supported
	if(strcmp(my_http_version, request->http_version)){
		handle_505(dbuf, cli_addr);
		free_request(request);
	
		//	return CLOSE;
		//	No need to worry ,because we do not strictly follow RFC2616
		return PERSISTENT;
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
			return PERSISTENT;
	}
	return PERSISTENT;
} 

// Handler --> Get, Post, Head
void handle_get(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
	dynamic_buffer *uri_dbuf = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
	//char *default_path =  "/home/christopher/Programme/C/Web/webServerStartCodes-new/Code/static_site";
	init_dynamic_buffer(uri_dbuf);
	append_dynamic_buffer(uri_dbuf, default_path, strlen(default_path));
	append_dynamic_buffer(uri_dbuf, request->http_uri, strlen(request->http_uri));
	if(!strcmp(request->http_uri, "/")){
		// By default, This shall be index.html
		append_dynamic_buffer(uri_dbuf, "index.html", strlen("index.html"));
	}
	struct stat file_buffer;
	if(stat(uri_dbuf->buf, &file_buffer)){
#ifdef DEBUG
		ERROR("Error address '%s'\n" ,uri_dbuf->buf);
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
	char content_type[BUF_SIZE] = {0};
		get_file_type(uri_dbuf->buf, content_type);
#undef BUF_SIZE	

	reset_dynamic_buffer(dbuf);
	set_response(dbuf, "200", "OK");
	set_header(dbuf, "Date", time_buffer);
	set_header(dbuf, "Server", server_info);
 	set_header(dbuf, "Last-Modified", last_modified);
	set_header(dbuf, "Content-Length", content_length);
	set_header(dbuf, "Content-Type", content_type);
	if(return_value==CLOSE){
		set_header(dbuf, "Connection", "Close");
	}else{
		set_header(dbuf, "Connection", "keep-alive");
	}
	set_msg(dbuf, crlf, strlen(crlf));

	// TODO: Open file and attach it to msg
	
	dynamic_buffer * dfbuf = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(dfbuf);
	if(get_file_content(dfbuf, uri_dbuf->buf)){
#ifdef DEBUG
		ERROR("Get File Content Error");
#endif
		handle_404(dbuf, cli_addr);
		return ;
	}
#ifdef DEBUG
	//print_dynamic_buffer(dfbuf);
#endif
	set_msg(dbuf, dfbuf->buf, dfbuf->current);
	//set_msg(dbuf, crlf);
	AccessLog("OK", cli_addr, "GET", 200);
	return ;
}
 
void handle_head(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
	// This should be modified if Get has other uri
	dynamic_buffer *uri_dbuf = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
	//char *default_path =  "/home/christopher/Programme/C/Web/webServerStartCodes-new/Code/static_site";
	init_dynamic_buffer(uri_dbuf);
	append_dynamic_buffer(uri_dbuf, default_path, strlen(default_path));
	append_dynamic_buffer(uri_dbuf, request->http_uri, strlen(request->http_uri));
	if(!strcmp(request->http_uri, "/")){
		append_dynamic_buffer(uri_dbuf, "index.html", strlen("index.html"));
	}
	struct stat file_buffer;
	if(stat(uri_dbuf->buf, &file_buffer)){
#ifdef DEBUG
		ERROR("Error address '%s'\n" ,uri_dbuf->buf);
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
	char content_type[BUF_SIZE] = {0};
		get_file_type(uri_dbuf->buf, content_type);
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
		set_header(dbuf, "Connection", "keep-alive");
	}
	set_msg(dbuf, crlf, strlen(crlf));
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
	//set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("400 Bad request", cli_addr);
}

void handle_404(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "404", "Not Found");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("404 Not Found", cli_addr);
}

void handle_501(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "501", "Not Implemented");
	//set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("501 Not Implemented", cli_addr);
}

void handle_505(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "505", "HTTP Version not supported");
	//set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
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

void set_msg(dynamic_buffer *dbuf, char* msg, int len){
	append_dynamic_buffer(dbuf, msg, len);
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

// Get Type
TYPE get_file_type(char * path, char *result){
	size_t len = strlen(path);
	int i;
	char extension[100]={0};
	for(i=len-1;i>=0 && len-i<100;i--){
		int c_len = len - i;
		if(path[i]=='.'){
			strncpy(extension, path+len-c_len+1, c_len-1);
			break;
		}
	}
	if(!strlen(extension)){
		strcpy(result, "application/octet-stream");
		return NONE;
	}
	for(i=0;extension[i];i++){
		extension[i] = tolower(extension[i]);
	}
	switch(get_TYPE(extension)){
		case HTML:
			strcpy(result, "text/html");
			return HTML;
		case CSS:
			strcpy(result, "text/css");
			return CSS;
		case PNG:
			strcpy(result, "image/png");
			return PNG;
		case JPEG:
			strcpy(result, "image/jpeg");
			return JPEG;
		case GIF:
			strcpy(result, "image/gif");
			return GIF;
		default:
			strcpy(result, "application/octet-stream");
			return NONE;
	}
}
// Get File TYPE
TYPE get_TYPE(char *extension){
	if(!strcmp(extension, "html") || !strcmp(extension, "text/html")){
		return HTML;
	}else if(!strcmp(extension, "css") || !strcmp(extension, "text/css")){
		return CSS;
	}else if(!strcmp(extension, "png") || !strcmp(extension, "image/png")){
		return PNG;
	}else if(!strcmp(extension, "jpeg") || !strcmp(extension, "image/jpeg")){
		return JPEG;
	}else if(!strcmp(extension, "gif") || !strcmp(extension, "image/gif")){
		return GIF;
	}else{
		return NONE;
	}
}

// Get File 
// TODO: Add BUF_SIZE
int get_file_content(dynamic_buffer * dfbuf, char*path){
	int fd_in=0;
	if((fd_in=open(path, O_RDONLY))<0){
		ERROR("Cannot Open file '%s'\n" ,path);
		free_dynamic_buffer(dfbuf);
		return 1;
	}
	struct stat file_stat;
	if((fstat(fd_in, &file_stat))<0){
		ERROR("Fetching Status of File '%s' Error\n", path);
		free_dynamic_buffer(dfbuf);
		return 1;
	}
	size_t file_len = file_stat.st_size;
	if(file_len<=0){
		ERROR("File '%s' Empty\n" ,path);
		free_dynamic_buffer(dfbuf);
		close(fd_in);
		return 1;
	}
	char *file = mmap(0, file_len, PROT_READ, MAP_PRIVATE, fd_in, 0);
	if(file==MAP_FAILED){
		close(fd_in);
		free_dynamic_buffer(dfbuf);
		ERROR("ERROR Mappig file\n");
		return 1;
	}
	append_dynamic_buffer(dfbuf, file, file_len);
	close(fd_in);
	return 0;

}


