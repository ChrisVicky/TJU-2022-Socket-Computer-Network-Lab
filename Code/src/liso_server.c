/******************************************************************************
 * echo_server.c                                                               *
 *                                                                             *
 * Description: This file contains the C source code for an echo server.  The  *
 *              server runs on a hard-coded port and simply write back anything*
 *              sent to it by connected clients.  It does not support          *
 *              concurrent clients.                                            *
 *                                                                             *
 * Authors: Athula Balachandran <abalacha@cs.cmu.edu>,                         *
 *          Wolf Richter <wolf@cs.cmu.edu>                                     *
 *                                                                             *
 *******************************************************************************/

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include "parse.h"
#include "util.h"
#include "buffer.h"
#include "response.h"

#define ECHO_PORT 9999
#define BUF_SIZE 1024
// #define BUF_SIZE 1
#define DEBUG

char * dest = "\r\n\r\n";

int close_socket(int sock)
{
	if (close(sock))
	{
		fprintf(stderr, "Failed closing socket.\n");
		return 1;
	}
	return 0;
}


/**
 * @brief 	Deal with buffer --> which contains (multiple) requests 
 * 		if it's a pipeline request
 * @param dbuf 		--> 	dynamic_buffer, requests
 * @param readret	-->	size of buffer
 * @param client_sock	-->	client's sock
 * @param sock		-->	server's sock
 * @param fd_in		-->	No meaning here
 * @param cli_addr	-->	client's address
 *
 * @return 
 * 	-->	PERSISTENT	: Go on waiting for msg
 * 	-->	EXIT_FAILURE	: End this connection
 * 	-->	CLOSE		: Close current connection ( Which can be ignored in our lab )
 */
int deal_buf(dynamic_buffer * dbuf, size_t readret, int client_sock, int sock, int fd_in, struct sockaddr_in cli_addr){
	char * t, *temp=dbuf->buf;
	int cnt = 0;
	/* deal pipeline */
	dynamic_buffer *return_buffer = (dynamic_buffer*) malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(return_buffer);
	while((t=strstr(temp,dest))!=NULL){
		int len = t - temp;
#ifdef DEBUG
		PRINT("============================CURRENT CNT: %d=========================\n",cnt++);
#endif
		dynamic_buffer * each = (dynamic_buffer *)malloc(sizeof(dynamic_buffer));
		init_dynamic_buffer(each);
		memset_dynamic_buffer(each);
		append_dynamic_buffer(each, temp, len);
		append_dynamic_buffer(each, dest, strlen(dest));
		temp = t + strlen(dest);
#ifdef DEBUG
		LOG("Starting dealing with msg\n----------\n%ld\n---------\n" ,each->current);
#endif
		Return_value result = handle_request(client_sock, sock, each, cli_addr);	
		append_dynamic_buffer(return_buffer, each->buf, each->current);
#ifdef DEBUG
		LOG("MSG Appended\n");
#endif
		if(result==CLOSE)
			return CLOSE;
		free_dynamic_buffer(each);
	}
	update_dynamic_buffer(dbuf, temp);
#ifdef DEBUG
	LOG("msg to be sent\n======================= Sending ======================\n%s\n" ,return_buffer->buf);
#endif
	if(!return_buffer->current){
#ifdef DEBUG
		LOG("Not complete\n");
#endif
		free_dynamic_buffer(return_buffer);
		return PERSISTENT;
	}
	if (send(client_sock, return_buffer->buf, return_buffer->current, 0) != return_buffer->current)
	{
		close_socket(client_sock);
		close_socket(sock);
		free_dynamic_buffer(return_buffer);
		fprintf(stderr, "Error sending to client.\n");
		return EXIT_FAILURE;
	}

#ifdef DEBUG
	LOG("End with this buf, go back persistently\n");
#endif
	free_dynamic_buffer(return_buffer);
	return PERSISTENT;
} 
int main(int argc, char* argv[])
{
	int sock, client_sock;
	ssize_t readret; /* int */
	socklen_t cli_size; /* uint */
	struct sockaddr_in addr, cli_addr; 
	/*unsigned short int, unsigned int*/
	char buf[BUF_SIZE];

	fprintf(stdout, "----- Echo Server -----\n");

	/* all networked programs must create a socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "Failed creating socket.\n");
		return EXIT_FAILURE;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(ECHO_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;

	/* servers bind sockets to ports---notify the OS they accept connections */
	if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)))
	{
		close_socket(sock);
		fprintf(stderr, "Failed binding socket.\n");
		return EXIT_FAILURE;
	}


	if (listen(sock, 5))
	{
		close_socket(sock);
		fprintf(stderr, "Error listening on socket.\n");
		return EXIT_FAILURE;
	}

	/* finally, loop waiting for input and then write it back */
	while (1)
	{
#ifdef DEBUG
		LOG("Start Listening at port %d\n" ,ECHO_PORT);
#endif
		cli_size = sizeof(cli_addr);
		if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr ,&cli_size)) == -1)
		{
			close(sock);
			fprintf(stderr, "Error accepting connection.\n");
			return EXIT_FAILURE;
		}

		readret = 0;
		dynamic_buffer *dbuf = (dynamic_buffer *) malloc(sizeof(dynamic_buffer));
		init_dynamic_buffer(dbuf);
		while((readret = recv(client_sock, buf, BUF_SIZE, 0)) >= 1)
		{
#ifdef DEBUG
			LOG("Msg recieved: '%ld' from client : %s:%d\n", strlen(buf),inet_ntoa(cli_addr.sin_addr),(int) ntohs(cli_addr.sin_port) );
#endif
			append_dynamic_buffer(dbuf, buf, readret);
			/* parse requests */
			print_dynamic_buffer(dbuf);
			if(deal_buf(dbuf, readret, client_sock, sock, 8192, cli_addr)!=PERSISTENT){
				break;
			}
			memset(buf, 0, sizeof(buf));
		} 

		free_dynamic_buffer(dbuf);
		if (readret == -1)
		{
			close_socket(client_sock);
			close_socket(sock);
			fprintf(stderr, "Error reading from client socket.\n");
			return EXIT_FAILURE;
		}

		if (close_socket(client_sock))
		{
			close_socket(sock);
			fprintf(stderr, "Error closing client socket.\n");
			return EXIT_FAILURE;
		}
	}

	close_socket(sock);

	return EXIT_SUCCESS;
}
