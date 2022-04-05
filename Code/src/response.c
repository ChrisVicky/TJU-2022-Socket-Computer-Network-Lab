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

// #define DEBUG
#define BUF_SIZE 1024
char *my_http_version = "HTTP/1.1";
char *space = " ";
char *crlf = "\r\n";
char *colon = ":";

char *server_info = "liso/1.0";
char *default_path = "./static_site";

int current_clinet_fd = 0;

/**
 * @brief 
 * 	--> Handle Requests 
 *
 * @param client_sock	: client's socket
 * @param sock		: server's socket
 * @param dbuf		: buffer for sending msgs
 * @param cli_addr	: client's address
 *
 * @return 
 * 	-->	PERSISTENT	: ..
 * 	--> 	CLOSE		: Not important
 *
 * @Process
 * 	1. Parsing Error --> 400
 * 	2. Check Connection Close
 * 	3. Check Http Version --> 505
 * 	4. Methods Error --> 501
 * 	5. CGI ? Static Path,
 *
 */
int handle_request(int client_sock, int sock, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, dynamic_buffer *odbuf){
	current_clinet_fd = client_sock;
	Request *request = parse(dbuf->buf, dbuf->current, client_sock);

	// 400 Error Parsing
	if(request == NULL){
		handle_400(dbuf, cli_addr);
		return CLOSE;
	}

	// check connection:close
	Return_value return_value = PERSISTENT;
	char *connection_value = get_header_value(request, "Connection");
	if((connection_value!=NULL) && (!strcmp(connection_value, "Close"))){
		// Still --> May be a Post or Get to send msg here.
		return_value = CLOSE_FROM_CLIENT;
	}

	// 505 Http version not supported
	if(strcmp(my_http_version, request->http_version)){
		handle_505(dbuf, cli_addr);
		free_request(request);
		//	return CLOSE;
		//	No need to worry ,because we do not strictly follow RFC2616
		return CLOSE;
	}

	// TODO: 
	// 	Check Whether it is Completely Received

	// Methods, 501 Method not supported.
	METHODS this_method = method_switch(request->http_method);
	if(this_method==NOT_SUPPORTED){
		handle_501(dbuf, cli_addr);
		free_request(request);
		return CLOSE;
	}

	// Check whether body part is completely received;
	char *body_length_str = get_header_value(request, "Content-Length");
	if(body_length_str==NULL){
		//No body;
	}else{
		int body_length = atoi(body_length_str);
		if(body_length + odbuf->access_end > odbuf->current){
			// Means Body is not completely received;
			return NOT_COMPLETE;
		}
	}


	// Check cgi?
	char *cgi_prefix = "/cgi/";
	char prefix[10] = {0};
	if(strlen(request->http_uri) >= strlen(cgi_prefix)){
		strncpy(prefix, request->http_uri, strlen(cgi_prefix));
	}
	if(strcmp(cgi_prefix, prefix)){
		LOG("NOT CGI\n");
		//Static!!
		switch(this_method){
			case GET:
				return handle_get(request, dbuf, cli_addr, return_value);
			case HEAD:
				return handle_head(request, dbuf, cli_addr, return_value);
			case POST:
				handle_post(request, dbuf, cli_addr, return_value, odbuf);
				break;
			default:
				ERROR("Should Not be here");
				exit(EXIT_FAILURE);
		}
	}else{
		LOG("CGI !!\n");
		switch(this_method){
			case GET:
				if(handle_cgi_get(request, dbuf, cli_addr, odbuf)){
					handle_500(dbuf, cli_addr);
					return CLOSE;
				}
				return CLOSE;
			case POST:
				if(handle_cgi_post(request, dbuf, cli_addr, odbuf)){
					handle_500(dbuf, cli_addr);
					return CLOSE;
				}
				return CLOSE;
			default:
				handle_501(dbuf, cli_addr);
				return CLOSE;

		}
		// CGI HERE!
	}
	return PERSISTENT;
} 

/**
 * @brief 	: handle get requests
 *
 * @param request		: requests details
 * @param dbuf			: buffer for returned msgs
 * @param cli_addr		: client's address
 * @param return_value		: returned value
 */
// Handler --> Get, Post, Head
int handle_get(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
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
		return CLOSE;
	}
	char time_buffer[BUF_SIZE]={0}; 
	get_time(time_buffer, BUF_SIZE);
	char last_modified[BUF_SIZE]={0}; 
	get_last_modified(file_buffer, last_modified, BUF_SIZE);
	char content_length[BUF_SIZE]={0};
	sprintf(content_length, "%ld", file_buffer.st_size);
	//char content_type[BUF_SIZE] = {0};
	char *content_type = get_file_type(uri_dbuf->buf);

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
		return CLOSE;
	}
#ifdef DEBUG
	//print_dynamic_buffer(dfbuf);
#endif
	set_msg(dbuf, dfbuf->buf, dfbuf->current);
	//set_msg(dbuf, crlf);
	AccessLog("OK", cli_addr, "GET", 200, current_clinet_fd);
	return PERSISTENT;
}

/**
 * @brief 		: Handle Head requests
 *
 * @param request	: For requests details
 * @param dbuf		: For containing return msg
 * @param cli_addr	: Client's address
 * @param return_value	: NOT IMPORTANT
 */
int handle_head(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value){
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
		return CLOSE;
	}
	char time_buffer[BUF_SIZE]={0}; 
	get_time(time_buffer, BUF_SIZE);
	char last_modified[BUF_SIZE]={0}; 
	get_last_modified(file_buffer, last_modified, BUF_SIZE);
	char content_length[BUF_SIZE]={0};
	sprintf(content_length, "%ld", file_buffer.st_size);
	//char content_type[BUF_SIZE] = {0};
	char *content_type = get_file_type(uri_dbuf->buf);

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
	AccessLog("OK", cli_addr, "HEAD", 200, current_clinet_fd);
	free_dynamic_buffer(uri_dbuf);
	return PERSISTENT;
}

void handle_post(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, int return_value, dynamic_buffer *odbuf){
	/**
	 * 1. Check uri
	 * /


	// Get Paras from Request;
	char *ch_length = get_header_value(request, "Content-Length");
	if(ch_length==NULL){
	ErrorLog("No attribute Content-Length, Parsing Failed", cli_addr, 400);
	handle_400(dbuf, cli_addr);
	return;
	}
	int content_length = atoi(get_header_value(request, "Content-Length"));
	dynamic_buffer *paras = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(paras);
	catpart_dynamic_buffer(paras, odbuf, odbuf->access_end, content_length);
	// Update Global Buffer Access_end;
	odbuf->access_end += content_length;


	// Post: Just Echo back 
	*/
	AccessLog("Echo Back", cli_addr,"POST", 200, current_clinet_fd);
	return ;
}

// Error Number Handler
void handle_400(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "400", "Bad request");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("400 Bad request", cli_addr, current_clinet_fd);
}

void handle_404(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "404", "Not Found");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("404 Not Found", cli_addr, current_clinet_fd);
}

void handle_500(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "500", "Internal Server Error");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("500 Internal Server Error", cli_addr, current_clinet_fd);
}
void handle_501(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "501", "Not Implemented");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("501 Not Implemented", cli_addr, current_clinet_fd);
}

void handle_505(dynamic_buffer *dbuf, struct sockaddr_in cli_addr){
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "505", "HTTP Version not supported");
	set_header(dbuf, "Connection", "Close");
	set_msg(dbuf, crlf, strlen(crlf));
	ErrorLog("505 HTTP Version not supported", cli_addr, current_clinet_fd);
}

// utility: get header value
char *get_header_value(Request *request, char *header_name){
	int i;
	for(i=0;i<request->header_count;i++){
		if(!strcmp(request->headers[i].header_name, header_name))
			return request->headers[i].header_value;
	}
	return NULL;
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


/**
 * @brief Set response --> Code and Description
 *
 * @param dbuf
 * @param code
 * @param description
 */
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


/**
 * @brief Set Header with Key: Value
 *
 * @param dbuf
 * @param key
 * @param value
 */
void set_header(dynamic_buffer *dbuf, char *key, char *value){
	append_dynamic_buffer(dbuf, key, strlen(key));
	append_dynamic_buffer(dbuf, colon, strlen(colon));
	append_dynamic_buffer(dbuf, space, strlen(space));
	append_dynamic_buffer(dbuf, value, strlen(value));
	append_dynamic_buffer(dbuf, crlf, strlen(crlf));
	return ;
}

/**
 * @brief Set Msg, usually an external \r\n or body part
 *
 * @param dbuf
 * @param msg
 * @param len
 */
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
void get_last_modified(struct stat file_buffer, char * last_modified, size_t buf_size){
	struct tm *t = gmtime(&file_buffer.st_mtime);
	strftime(last_modified, buf_size, "%a, %d %b %Y %H:%M:%S %Z",t);
}

// Get Type
char* get_file_type(char * path){
	char *result = (char*)malloc(sizeof(char*)*100);
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
		return result;
	}
	for(i=0;extension[i];i++){
		extension[i] = tolower(extension[i]);
	}
	switch(get_TYPE(extension)){
		case HTML:
			// In case we use Chinese, UTF-8 is strictly needed.
			strcpy(result, "text/html; charset=utf-8");
			return result;
		case CSS:
			strcpy(result, "text/css");
			return result;
		case PNG:
			strcpy(result, "image/png");
			return result;
		case JPEG:
			strcpy(result, "image/jpeg");
			return result;
		case GIF:
			strcpy(result, "image/gif");
			return result;
		case ICO:
			strcpy(result, "image/ico");
			return result;
		default:
			strcpy(result, "application/octet-stream");
			return result;
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
	}else if(!strcmp(extension, "ico") || !strcmp(extension, "image/ico")){
		return ICO;
	}else{
		return NONE;
	}
}

/**
 * @brief 		: Read from files 
 *
 * @param dfbuf		: For Containing Contents
 * @param path		: Path to the File
 *
 * @return 
 * 	--> 		0 : Success
 * 	-->		1 : Failed
 */
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

///////////////////////////////////////////////////////////////////////////////////////
// Deal with CGI REQUESTS
void execve_error_handler()
{
	switch (errno)
	{
		case E2BIG:
			fprintf(stderr, "The total number of bytes in the environment \
					(envp) and argument list (argv) is too large.\n");
			return;
		case EACCES:
			fprintf(stderr, "Execute permission is denied for the file or a \
					script or ELF interpreter.\n");
			return;
		case EFAULT:
			fprintf(stderr, "filename points outside your accessible address \
					space.\n");
			return;
		case EINVAL:
			fprintf(stderr, "An ELF executable had more than one PT_INTERP \
					segment (i.e., tried to name more than one \
						interpreter).\n");
			return;
		case EIO:
			fprintf(stderr, "An I/O error occurred.\n");
			return;
		case EISDIR:
			fprintf(stderr, "An ELF interpreter was a directory.\n");
			return;
		case ELIBBAD:
			fprintf(stderr, "An ELF interpreter was not in a recognised \
					format.\n");
			return;
		case ELOOP:
			fprintf(stderr, "Too many symbolic links were encountered in \
					resolving filename or the name of a script \
					or ELF interpreter.\n");
			return;
		case EMFILE:
			fprintf(stderr, "The process has the maximum number of files \
					open.\n");
			return;
		case ENAMETOOLONG:
			fprintf(stderr, "filename is too long.\n");
			return;
		case ENFILE:
			fprintf(stderr, "The system limit on the total number of open \
					files has been reached.\n");
			return;
		case ENOENT:
			fprintf(stderr, "The file filename or a script or ELF interpreter \
					does not exist, or a shared library needed for \
					file or interpreter cannot be found.\n");
			return;
		case ENOEXEC:
			fprintf(stderr, "An executable is not in a recognised format, is \
					for the wrong architecture, or has some other \
					format error that means it cannot be \
					executed.\n");
			return;
		case ENOMEM:
			fprintf(stderr, "Insufficient kernel memory was available.\n");
			return;
		case ENOTDIR:
			fprintf(stderr, "A component of the path prefix of filename or a \
					script or ELF interpreter is not a directory.\n");
			return;
		case EPERM:
			fprintf(stderr, "The file system is mounted nosuid, the user is \
					not the superuser, and the file has an SUID or \
					SGID bit set.\n");
			return;
		case ETXTBSY:
			fprintf(stderr, "Executable was open for writing by one or more \
					processes.\n");
			return;
		default:
			fprintf(stderr, "Unkown error occurred with execve().\n");
			return;
	}
}

char *get_path_info(char *uri){
	char *end = strstr(uri, "?");
	if(end==NULL){
		end = uri + strlen(uri);
	}
	char *temp = uri;
	char *t;
	while((t=strstr(temp, "/"))!=NULL){
		char *w = strstr(t, "/");
		if(w==NULL || w > end){
			break;
		}
		temp = t;
		// t: Last / before the end of uri.
	}
	char *ret = (char*) malloc(sizeof(char*)*(end - t));
	strncpy(ret, t, end-t);
	return ret;	
}

char *get_script_name(char *uri){
	if(uri==NULL)
		return NULL;
	char *end = strstr(uri, "?");
	if(end==NULL){
		end = uri + strlen(uri);
	}
		char *temp = (char*) malloc(sizeof(char*) * (end-uri+1));
		strncpy(temp+1, uri, end-uri);
		temp[0] = '.';
		temp[end-uri+1] = '\0';
		return temp;
	

}
char *get_query_string(char *uri){
	char *t = strstr(uri, "?");
	if(t==NULL) 
		return NULL;
	return t+1;
}
void set_EVNP(CGI_ARG* arg, Request* request, struct sockaddr_in cli_addr){
	append_KV(arg, "CONTENT_LENGTH", get_header_value(request, "Content-Length"));
	append_KV(arg, "CONTENT_TYPE", get_file_type(request->http_uri));
	append_KV(arg, "GATEWAY_INTERFACE", "CGI/1.1");
	//append_KV(arg, "PATH_INFO", get_path_info(request->http_uri));
	// NULL MAY HAPPEN
	if(method_switch(request->http_method)==GET)
		append_KV(arg, "QUERY_STRING", get_query_string(request->http_uri));
	append_KV(arg, "REMOTE_ADDR", inet_ntoa(cli_addr.sin_addr));
	append_KV(arg, "REQUEST_METHOD", request->http_method);
	append_KV(arg, "REQUEST_URI", request->http_uri);
	append_KV(arg, "SCRIPT_NAME", get_script_name(request->http_uri));
	append_KV(arg, "SERVER_PORT", "9999"); // This can be improved!!!!!
	append_KV(arg, "SERVER_PROTOCOL", "HTTP/1.1");
	append_KV(arg, "SERVER_SOFTWARE", "Liso/1.0");
	append_KV(arg, "HTTP_ACCEPT", get_header_value(request, "Accept"));
	append_KV(arg, "HTTP_REFERER", get_header_value(request, "Referer"));
	append_KV(arg, "HTTP_ACCEPT_ENCODING", get_header_value(request, "Accept-Encoding"));
	append_KV(arg, "HTTP_ACCEPT_LANGUAGE", get_header_value(request, "Accept-Language"));
	append_KV(arg, "HTTP_ACCEPT_CHARSET", get_header_value(request, "Accept-Charset"));
	append_KV(arg, "HTTP_HOST", get_header_value(request, "Host"));
	append_KV(arg, "HTTP_COOKIE", get_header_value(request, "Cookie"));
	append_KV(arg, "HTTP_USER_AGENT", get_header_value(request, "User-Agent"));
	append_KV(arg, "HTTP_CONNECTION", get_header_value(request, "Connection"));
}

/**
 * @brief 		: fork a new thread to deal with CGI issue
 *			: Both use in execve(FILENAME, ARGV, ENVP)
 * @param ARGV	 	: Arguments 
 * @param ENVP		: Environment Parameters
 *
 * @return 
 */
#define FILENAME "./cgi/cgi_dumper.py"
int forker(char **ARGV, char **ENVP, dynamic_buffer* dbuf, char *SCRIPT_NAME){
	/*************** BEGIN VARIABLE DECLARATIONS **************/
	LOG("Begin Forker\n");
	pid_t pid;
	int stdin_pipe[2];
	int stdout_pipe[2];
	char buf[BUF_SIZE];
	int readret;
	/*************** END VARIABLE DECLARATIONS **************/

	/*************** BEGIN PIPE **************/
	/* 0 can be read from, 1 can be written to */
	if (pipe(stdin_pipe) < 0)
	{
		fprintf(stderr, "Error piping for stdin.\n");
		return EXIT_FAILURE;
	}

	if (pipe(stdout_pipe) < 0)
	{
		fprintf(stderr, "Error piping for stdout.\n");
		return EXIT_FAILURE;
	}
	/*************** END PIPE **************/

	/*************** BEGIN FORK **************/
	pid = fork();
	/* not good */
	if (pid < 0)
	{
		fprintf(stderr, "Something really bad happened when fork()ing.\n");
		return EXIT_FAILURE;
	}

	/* child, setup environment, execve */
	if (pid == 0)
	{
		/*************** BEGIN EXECVE ****************/
		close(stdout_pipe[0]);
		close(stdin_pipe[1]);
		dup2(stdout_pipe[1], fileno(stdout));
		dup2(stdin_pipe[0], fileno(stdin));
		/* you should probably do something with stderr */

		/* pretty much no matter what, if it returns bad things happened... */
		if (execve(SCRIPT_NAME, ARGV, ENVP))
		{
			execve_error_handler();
			fprintf(stderr, "Error executing execve syscall.\n");
			return EXIT_FAILURE;
		}
		/*************** END EXECVE ****************/ 
	}

	char* POST_BODY = "This is the stdin body...\n";
	if (pid > 0)
	{
		fprintf(stdout, "Parent: Heading to select() loop.\n");
		close(stdout_pipe[1]);
		close(stdin_pipe[0]);

		if (write(stdin_pipe[1], POST_BODY, strlen(POST_BODY)) < 0)
		{
			fprintf(stderr, "Error writing to spawned CGI program.\n");
			return EXIT_FAILURE;
		}

		close(stdin_pipe[1]); /* finished writing to spawn */

		/* you want to be looping with select() telling you when to read */
		while((readret = read(stdout_pipe[0], buf, BUF_SIZE-1)) > 0)
		{
			buf[readret] = '\0'; /* nul-terminate string */
			fprintf(stdout, "Got from CGI: BEGIN BUF<<%s>>END OF BUF\n", buf);
			append_dynamic_buffer(dbuf, buf, strlen(buf));
		}

		close(stdout_pipe[0]);
		close(stdin_pipe[1]);

		if (readret == 0)
		{
			fprintf(stdout, "CGI spawned process returned with EOF as \
					expected.\n");
			return EXIT_SUCCESS;
		}
	}
	/*************** END FORK **************/
	return EXIT_SUCCESS;
}

/**
 * @brief Handle CGI requests
 *
 * @param request 	: Request
 * @param dbuf		: return buffer
 * @param cli_addr	: client's addr
 * @param odbuf		: Request buffer
 *
 * @return 		: Return_value
 */
int handle_cgi_get(Request* request, dynamic_buffer* dbuf, struct sockaddr_in cli_addr, dynamic_buffer* odbuf){
	// Check Script Existent
	char *scriptName = get_script_name(request->http_uri);
	struct stat file_buffer;
	if(stat(scriptName, &file_buffer)){
		ERROR("Script >>%s<< Not Exists\n" ,scriptName);
		return EXIT_FAILURE;
	}
	// Get paras from odbuf.
	CGI_ARG * arg = (CGI_ARG*) malloc(sizeof(CGI_ARG));
	init_CGI_ARG(arg);
	// SET ENVP
	set_EVNP(arg, request, cli_addr);
	// SET ARG
	append_arg(arg, scriptName);

	arg->ENVP[arg->cnt] = NULL;
	arg->argc[arg->argv] = NULL;	
	dynamic_buffer *body = (dynamic_buffer*) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(body);

	if(forker(arg->argc, arg->ENVP, body, scriptName)){
		handle_500(dbuf, cli_addr);
		return EXIT_FAILURE;
	}

	// SET RESPONSE
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "200", "OK");
	char time_buffer[BUF_SIZE] = {0};
		get_time(time_buffer, BUF_SIZE);
	set_header(dbuf, "Date", time_buffer);
	set_header(dbuf, "Server", server_info);
	// Set Content-Length
	char cttlength[BUF_SIZE];
	sprintf(cttlength, "%ld", body->current);
	set_header(dbuf, "Content-Length", cttlength);
	set_msg(dbuf, crlf, strlen(crlf));
	set_msg(dbuf, body->buf, body->current);
	return EXIT_SUCCESS;
}


/**
 * @brief Deal with CGI Post
 *
 * @param request	: request structure
 * @param dbuf		: Return Buffer	
 * @param cli_addr	: Client's address
 * @param odbuf		: Original Buffer
 *
 * @return 
 */
int handle_cgi_post(Request *request, dynamic_buffer *dbuf, struct sockaddr_in cli_addr, dynamic_buffer *odbuf){
	// Check Script Existent
	char *scriptName = get_script_name(request->http_uri);
	struct stat file_buffer;
	if(stat(scriptName, &file_buffer)){
		ERROR("Script >>%s<< Not Exists\n" ,scriptName);
		return EXIT_FAILURE;
	}
	// Get paras from odbuf.
	CGI_ARG * arg = (CGI_ARG*) malloc(sizeof(CGI_ARG));
	init_CGI_ARG(arg);
	// SET ENVP
	set_EVNP(arg, request, cli_addr);
	// SET ARG
	append_arg(arg, get_script_name(request->http_uri));

	// Get PARAS
	dynamic_buffer *paras = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(paras);
	int content_length = atoi(get_header_value(request, "Content-Length"));
	catpart_dynamic_buffer(paras, odbuf, odbuf->access_end, content_length);
	
	// Update Global Buffer Access_end;
	odbuf->access_end += content_length;
	
	// SET QUERY_STRING!! 
	append_KV(arg, "QUERY_STRING", paras->buf);

	arg->ENVP[arg->cnt] = NULL;
	arg->argc[arg->argv] = NULL;	
	dynamic_buffer *body = (dynamic_buffer*) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(body);

	if(forker(arg->argc, arg->ENVP, body, scriptName)){
		handle_500(dbuf, cli_addr);
		return EXIT_FAILURE;
	}

	// SET RESPONSE
	memset_dynamic_buffer(dbuf);
	set_response(dbuf, "200", "OK");
	char time_buffer[BUF_SIZE] = {0};
		get_time(time_buffer, BUF_SIZE);
	set_header(dbuf, "Date", time_buffer);
	set_header(dbuf, "Server", server_info);
	char cttlength[BUF_SIZE];
	sprintf(cttlength, "%ld", body->current);
	set_header(dbuf, "Content-Length", cttlength);
	set_msg(dbuf, crlf, strlen(crlf));
	set_msg(dbuf, body->buf, body->current);
	return EXIT_SUCCESS;
}

