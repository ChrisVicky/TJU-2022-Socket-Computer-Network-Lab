/* C declarations used in actions */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "parse.h"

#define SUCCESS(format, ...) printf("\33[1;33m" format "\33[0m",## __VA_ARGS__)
#define ERROR(format, ...) printf("\33[1;31m ERROR: " format "\33[0m", ## __VA_ARGS__)
#define PRINT(format, ...) printf("\33[1;37m" format "\33[0m", ## __VA_ARGS__)
#define LOG(format, ...) printf("\33[1;36m LOG: " format "\33[0m", ## __VA_ARGS__)


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
  Request *request = parse(buf,readRet,fd_in);
  //Just printing everything
  LOG("Http Method: '%s'\n",request->http_method);
  LOG("Http Version: '%s'\n",request->http_version);
  LOG("Http Uri:'%s'\n",request->http_uri);
  for(index = 0;index < request->header_count;index++){
    LOG("Request Header\n");
    LOG("Header name: '%s'; Header Value: '%s'\n",request->headers[index].header_name,request->headers[index].header_value);
  }
  free(request->headers);
  free(request);
  return 0;
}
