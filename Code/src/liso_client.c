/******************************************************************************
 * echo_client.c                                                               *
 *                                                                             *
 * Description: This file contains the C source code for an echo client.  The  *
 *              client connects to an arbitrary <host,port> and sends input    *
 *              from stdin.                                                    *
 *                                                                             *
 * Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
 *          Wolf Richter <wolf@cs.cmu.edu>                                     *
 *                                                                             *
 *******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <time.h>
#include "parse.h"
#include "util.h"
#define ECHO_PORT 9999
//#define BUF_SIZE 4096
#define BUF_SIZE 4096
#define DEBUG
const char dest[] = "\r\n\r\n";
const char header_name_connection[] = "Connection";
const char header_value_close[] = "close";
int tot_request(char * buf){
	char * t, *temp=buf;
	int tot = 0;
	while((t=strstr(temp,dest))!=NULL){
		tot ++;
		temp = t + strlen(dest);
	}
	return tot;
}
#define CONNECTION_CLOSE 1
#define CONNECTION_PERSIST 0
int handle_request_client(Request * request){
	if(request==NULL){
		// Server can never be wrong, so this must be an Error Msg
		return CONNECTION_PERSIST;
	}
	Request_header *headers = request->headers;
	int i;
	for(i=0;i<request->header_count;i++){
		if(!strcmp(headers[i].header_name, header_name_connection)){
			if(!strcmp(headers[i].header_value, header_value_close)){
				return CONNECTION_CLOSE;
			}
		}
	}
	return CONNECTION_PERSIST;
}
int handle_send(int sock, char *msg, int fd_in){
	char buf[BUF_SIZE];
	int bytes_received;
	fprintf(stdout, "========== Sending ==========\n%s", msg);
	send(sock, msg , strlen(msg), 0);
#ifdef DEBUG
	LOG("Contents Sent:%ld\n",strlen(msg));
#endif
	//memset(buf, 0, sizeof(buf));
	int num_of_request = tot_request(msg);
	int i;
	for(i=0;i<num_of_request;i++){
		if((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
		{
#ifdef DEBUG
			LOG("bytes_received: %d\n" ,bytes_received);
#endif	
			buf[bytes_received] = '\0';
			fprintf(stdout, "========== Recieve ==========\n%s\n" ,buf);
		/* 	Request *request = parse(buf, strlen(buf), fd_in);
			if(handle_request_client(request)==CONNECTION_CLOSE){
				return CONNECTION_CLOSE;
			}
		*/
			memset(buf, 0, sizeof(buf));
		}        
	}
	return CONNECTION_PERSIST;
}
int main(int argc, char* argv[])
{
	if (argc < 3) {
		fprintf(stderr, "usage: %s <server-ip> <port> <file1> <file2> ... <filen>", argv[0]);
		return EXIT_FAILURE;
	}
	int status, sock;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	struct addrinfo *servinfo; //will point to the results
	hints.ai_family = AF_INET;  //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
	hints.ai_flags = AI_PASSIVE; //fill ,in my IP for me

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo error: %s \n", gai_strerror(status));
		return EXIT_FAILURE;
	}

	if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		fprintf(stderr, "Socket failed");
		return EXIT_FAILURE;
	}

	if (connect (sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		fprintf(stderr, "Connect");
		return EXIT_FAILURE;
	}
	char msg[BUF_SIZE]; 

	/* support reading from files */
	if(argc==3){
		fgets(msg, BUF_SIZE, stdin);
		handle_send(sock, msg, 0);
	}else{
		// Handle Persistent Connection
		int agc=3;
		for(agc=3;agc<argc;agc++){
#ifdef DEBUG
			LOG("Open file: '%s' \n" ,argv[agc]);
#endif
			int fd_in = open(argv[agc], O_RDONLY);
			if(fd_in<0){
				close(sock);
				freeaddrinfo(servinfo);
				ERROR("Failed to open the file\n");
				return EXIT_FAILURE;
			}
#ifdef DEBUG
			LOG("Reading file: '%s'\n" ,argv[agc]);
#endif
			if(read(fd_in, msg, BUF_SIZE)<1){
				ERROR("Failed to read the file\n");
				freeaddrinfo(servinfo);
				close(sock);
				return EXIT_FAILURE;
			}
			if(handle_send(sock, msg, fd_in)==CONNECTION_CLOSE){
				break;
			}
			memset(msg, 0, sizeof(msg));
		}
	}
	freeaddrinfo(servinfo);
	close(sock);    
	return EXIT_SUCCESS;
}
