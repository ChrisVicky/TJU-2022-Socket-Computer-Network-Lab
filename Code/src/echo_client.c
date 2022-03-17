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
#include "util.h"
#define ECHO_PORT 9999
#define BUF_SIZE 4096
//#define BUF_SIZE 8192
//#define DEBUG
int main(int argc, char* argv[])
{
	if (argc != 3 && argc != 4)
	{
		fprintf(stderr, "usage: %s <server-ip> <port> <file>",argv[0]);
		return EXIT_FAILURE;
	}
	
       	char buf[BUF_SIZE];

	int status, sock;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	struct addrinfo *servinfo; //will point to the results
	hints.ai_family = AF_INET;  //IPv4
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
	hints.ai_flags = AI_PASSIVE; //fill in my IP for me

	if ((status = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) 
	{
		fprintf(stderr, "getaddrinfo error: %s \n", gai_strerror(status));
		return EXIT_FAILURE;
	}

	if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1)
	{
		fprintf(stderr, "Socket failed");
		return EXIT_FAILURE;
	}

	if (connect (sock, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		fprintf(stderr, "Connect");
		return EXIT_FAILURE;
	}

	char msg[BUF_SIZE]; 

/* support reading from files */
	if(argc == 4){	
#ifdef DEBUG
		LOG("Open file: '%s' \n" ,argv[3]);
#endif
		int fd_in = open(argv[3], O_RDONLY);
		if(fd_in<0){
			close(sock);
			freeaddrinfo(servinfo);
			ERROR("Failed to open the file\n");
			return EXIT_FAILURE;
		}
#ifdef DEBUG
		LOG("Reading file: '%s'\n" ,argv[3]);
#endif
		read(fd_in, msg, BUF_SIZE);
#ifdef DEBUG
		LOG("File Contents: '%s'\n" ,msg);
#endif
	}else{
		fgets(msg, BUF_SIZE, stdin);
	}

	int bytes_received;
	fprintf(stdout, "Sending '%s'", msg);
	send(sock, msg , strlen(msg), 0);
#ifdef DEBUG
	LOG("Contents Sent\n");
#endif
	//memset(buf, 0, sizeof(buf));
	if((bytes_received = recv(sock, buf, BUF_SIZE, 0)) > 1)
	{
#ifdef DEBUG	
		LOG("bytes_received: %d\n" ,bytes_received);
#endif	
		buf[bytes_received] = '\0';
		fprintf(stdout, "Received %s", buf);
	}        

	freeaddrinfo(servinfo);
	close(sock);    
	return EXIT_SUCCESS;
}
