#include "util.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>

#define _SIZE 1000

typedef struct{
	char *argc[2];
	size_t argv;
	char *ENVP[_SIZE];
	size_t cnt;
}CGI_ARG;


void init_CGI_ARG(CGI_ARG*);
void append_KV(CGI_ARG*, char*, char*);
void free_CGI_ARG(CGI_ARG*);
void append_arg(CGI_ARG*, char*);
void kv_to_ENVP(CGI_ARG*, char**, int*);
void print_CGI(CGI_ARG*);


//char* ARGV[] = {
//FILENAME,
//NULL
//};

//char* ENVP[] = {
//"CONTENT_LENGTH=",
//"CONTENT-TYPE=",
//"GATEWAY_INTERFACE=CGI/1.1",
//"QUERY_STRING=action=opensearch&search=HT&namespace=0&suggest=",
//"REMOTE_ADDR=128.2.215.22",
//"REMOTE_HOST=gs9671.sp.cs.cmu.edu",
//"REQUEST_METHOD=GET",
//"SCRIPT_NAME=/w/api.php",
//"HOST_NAME=en.wikipedia.org",
//"SERVER_PORT=80",
//"SERVER_PROTOCOL=HTTP/1.1",
//"SERVER_SOFTWARE=Liso/1.0",
//"HTTP_ACCEPT=application/json, text/javascript, */*; q=0.01",
//"HTTP_REFERER=http://en.wikipedia.org/w/index.php?title=Special%3ASearch&search=test+wikipedia+search",
//"HTTP_ACCEPT_ENCODING=gzip,deflate,sdch",
//"HTTP_ACCEPT_LANGUAGE=en-US,en;q=0.8",
//"HTTP_ACCEPT_CHARSET=ISO-8859-1,utf-8;q=0.7,*;q=0.3",
//"HTTP_COOKIE=clicktracking-session=v7JnLVqLFpy3bs5hVDdg4Man4F096mQmY; mediaWiki.user.bucket%3Aext.articleFeedback-tracking=8%3Aignore; mediaWiki.user.bucket%3Aext.articleFeedback-options=8%3Ashow; mediaWiki.user.bucket:ext.articleFeedback-tracking=8%3Aignore; mediaWiki.user.bucket:ext.articleFeedback-options=8%3Ashow",
//"HTTP_USER_AGENT=Mozilla/5.0 (X11; Linux i686) AppleWebKit/535.1 (KHTML, like Gecko) Chrome/14.0.835.186 Safari/535.1",
//"HTTP_CONNECTION=keep-alive",
//"HTTP_HOST=en.wikipedia.org",
//NULL
//};
//
//
//
//
