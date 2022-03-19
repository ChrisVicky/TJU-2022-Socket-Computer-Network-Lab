/* C declarations used in actions */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "parse.h"
#include "util.h"
#define BUF_SIZE 8192
#define DEBUG

const char *dest = "\r\n\r\n";

void get_next_request(char *source, char *buf, int *now, int *offset){
	memset(buf, 0, BUF_SIZE * sizeof(char));
	int len = strlen(source);
	while((*now)!=len){
		buf[(*offset)++] = source[(*now)++];
		if((*now)>=2 && source[*now]=='\n' && source[(*now)-1]=='\r' && source[(*now)-2]=='\r'){
			buf[(*offset)++] = source[(*now)++];
			return ;
		}
	}
#ifdef DEBUG
	ERROR("Error-->Cannot tear it appart\n");
#endif
	buf = NULL;
	return;
}
int deal_request(Request * request){
	if(request==NULL){
		ERROR("HTTP/1.1 400 Bad request\r\n\r\n");
		return 1;
	}
	//Just printing everything
	LOG("Http Method: '%s'\n",request->http_method);
//	LOG("Http Version: '%s'\n",request->http_version);
//	LOG("Http Uri:'%s'\n",request->http_uri);
//	int index = 0;
//	for(index = 0;index < request->header_count;index++){
//		LOG("Request Header\n");
//		LOG("Header name: '%s'; Header Value: '%s'\n",request->headers[index].header_name,request->headers[index].header_value);
//	}
	return 0;
}
int main(int argc, char **argv){
	//Read from the file the sample
	int fd_in = open(argv[1], O_RDONLY);
	int index;
	char buf[8192];
	if(fd_in < 0) {
		ERROR("Failed to open the file\n");
		return 0;
	}
	int readRet = read(fd_in,buf,8192);
	//Parse the buffer to the parse function. You will need to pass the socket fd and the buffer would need to
	//be read from that fd

	char * t, *temp=buf;
	while((t=strstr(temp,dest))!=NULL){
		int len = t - temp;
		char each[8192];
		memset(each, 0, sizeof(each));
		strncpy(each, temp, len);
		strncat(each, dest, strlen(dest));
		temp = t + strlen(dest);
		Request *request = parse(each, strlen(each), fd_in);
		if(deal_request(request)){
			return 0;
		}
		free(request->headers);
		free(request);
	}
	return 0;
}
