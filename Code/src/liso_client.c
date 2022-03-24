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
#define BUF_SIZE (4096*10)
#define DEBUG

//#define RFC2616
#ifdef RFC2616////////////////////////////////////////////////////////////////
// Can be ignored in our lab
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
/**
 * @brief Check header_name "Connection" to determine the persistency.
 *
 * @param request	-->	Request (structure)
 *
 * @return 
 * 	-->	CONNECTION_PERSIST	: Persistent on
 * 	-->	CONNECTION_CLOSE	: Close Current Connection
 */
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
#endif////////////////////////////////////////////////////////////////

/**
 * @brief Handle MSG Send and Recieve Operation 
 *	--> NOT IMPORTANT IN OUR LAB
 * @param sock 		-->	Destination Socket
 * @param msg		--> 	Msg to be sent
 * @param fd_in		--> 	Not important here
 *
 * @return 	
 * 	-->	CONNECTION_PERSIST	: Persistent on
 * 	-->	CONNECTION_CLOSE	: Close Current Connection
 */
int handle_send(int sock, char *msg, int fd_in){
	char buf[BUF_SIZE];
	int bytes_received;
	fprintf(stdout, "========== Sending ==========\n%s", msg);
	send(sock, msg , strlen(msg), 0);
#ifdef DEBUG
	LOG("Contents Sent:%ld\n",strlen(msg));
#endif

#ifdef RFC2616////////////////////////////////////////////////////////////////
	int num_of_request = tot_request(msg);
#ifdef DEBUG
	LOG("Total Requests: --> %d\n" ,num_of_request);
#endif
	// Debug --> We send and recieve one at a time.
	// Even though when we deal with a pipeline
	int i;
	num_of_request = 1; 
	for(i=0;i<num_of_request;i++){
#endif////////////////////////////////////////////////////////////////

		if((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
		{
#ifdef DEBUG
			LOG("bytes_received: %d\n" ,bytes_received);
#endif	
			buf[bytes_received] = '\0';
			fprintf(stdout, "========== Recieve ==========\n%s\n" ,buf);

#ifdef RFC2616////////////////////////////////////////////////////////////////
			PRINT("%d/%d\n" ,i,num_of_request);
			Request *request = parse(buf, strlen(buf), fd_in);
			if(handle_request_client(request)==CONNECTION_CLOSE){
				return CONNECTION_CLOSE;
			}
#endif////////////////////////////////////////////////////////////////

			memset(buf, 0, sizeof(buf));
		}        

#ifdef RFC2616////////////////////////////////////////////////////////////////
	}
	return CONNECTION_PERSIST;
#else 
	return 1;
#endif////////////////////////////////////////////////////////////////

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
#ifdef RFC2616
			if(handle_send(sock, msg, fd_in)==CONNECTION_CLOSE){
				break;
			}
#else
			handle_send(sock, msg, fd_in);
#endif
			memset(msg, 0, sizeof(msg));
		}
	}
	freeaddrinfo(servinfo);
	close(sock);    
	return EXIT_SUCCESS;
}
