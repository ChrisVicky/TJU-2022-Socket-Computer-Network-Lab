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
#include <sys/select.h>

#include "parse.h"
#include "util.h"
#include "buffer.h"
#include "response.h"

#define MAX_FD_SIZE 1024
//#define MAX_FD_SIZE 40
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
 * @param cli_addr	-->	client's address
 *
 * @return 
 * 	-->	PERSISTENT	: Go on waiting for msg
 * 	-->	EXIT_FAILURE	: End this connection
 * 	-->	CLOSE		: Close current connection ( Which can be ignored in our lab )
 */
int deal_buf(dynamic_buffer * dbuf, size_t readret, int client_sock, int sock, struct sockaddr_in cli_addr){
	print_dynamic_buffer(dbuf);
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
		ERROR("Error sending to client.\n");
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
	fd_set tot_fds;
	dynamic_buffer *ADBUF[MAX_FD_SIZE];
	int i;
	for(i=0;i<MAX_FD_SIZE;i++){
		ADBUF[i] = (dynamic_buffer*) malloc(sizeof(dynamic_buffer));
	}

	fprintf(stdout, "------------ Echo Server ------------\n");

	/* all networked programs must create a socket */
	if ((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
	{
		fprintf(stderr, "Failed creating socket.\n");
		return EXIT_FAILURE;
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(ECHO_PORT);
	addr.sin_addr.s_addr = INADDR_ANY;
	// dealwith binding error;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, buf, sizeof(buf));
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

	FD_ZERO(&tot_fds); // INIT
	FD_SET(sock, &tot_fds); // Add Server Socket to SET;


	/* finally, loop waiting for input and then write it back */
	while (1)
	{
		fd_set tmp_fds = tot_fds;
		PRINT("_______________________________________________________________________________\n");
		PRINT("			START LISTENING AT 127.0.0.1 : %d \n" ,ECHO_PORT);
		PRINT("|______________________________________________________________________________|\n");
#ifdef DEBUG
		LOG("Start Listening at port %d\n" ,ECHO_PORT);
#endif
		int cnt, fd;
		if((cnt = select(MAX_FD_SIZE+1, &tmp_fds, NULL, NULL, NULL)) < 1){
			ERRORLOG("Select < 1, No Clients");
			return EXIT_FAILURE;
		}
#ifdef DEBUG
		LOG("After Selecting! cnt : %d\n" ,cnt);
#endif
		for(fd = 0; fd< MAX_FD_SIZE;fd++){
			if(!FD_ISSET(fd, &tmp_fds)){
#ifdef DEBUG
				//LOG("fd %d Not in SET\n" ,fd);
#endif
				// Not in the set;
				continue;
			}
#ifdef DEBUG
			LOG("cnt : %d\n" ,cnt);
#endif
			if(!cnt--){
#ifdef DEBUG
				LOG("CNT USED UP --> cnt %d, Break\n" ,cnt);
#endif 
				break;
			}
			if(fd == sock){
				/* 	Client's fd equals to server's socket
				 * --> 	Request for Connection;
				 *  */
				cli_size = sizeof(cli_addr);
				if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr ,&cli_size)) == -1)
				{
					close(sock);
					fprintf(stderr, "Error accepting connection.\n");
					return EXIT_FAILURE;
				}
				FD_SET(client_sock, &tot_fds);
				init_dynamic_buffer(ADBUF[client_sock]);
#ifdef DEBUG
				LOG("Accepting Connection FROM %d, client_sock %d\n" ,fd, client_sock);
#endif
			}else{
				/* Have Connected */
				memset(buf, 0, sizeof(buf));
				if((readret=recv(fd, buf, BUF_SIZE,0))>=1){
#ifdef DEBUG
					LOG("STARTING RECV MSG FROM %d\n" ,fd);
#endif
					append_dynamic_buffer(ADBUF[fd], buf, readret);
					if(deal_buf(ADBUF[fd], readret, fd, sock, cli_addr)!=PERSISTENT){
						// TODO: cli_addr is not updated --> It's not the right one;
						break;
					}
				}else if(!readret){
					/* Receive Nothing --> Close Connection */
					free_buffer_dynamic_buffer(ADBUF[fd]);
					close_socket(fd);
					FD_CLR(fd, &tot_fds);
#ifdef DEBUG
					LOG("Close Client on FD %d\n" ,fd);
#endif
				}else{
					close_socket(fd);
#ifdef DEBUG
					ERROR("Readret < 0!\n");
					break;
#endif
				}
			}

		} 
	}
	for(i=0;i<MAX_FD_SIZE;i++) free_dynamic_buffer(ADBUF[i]);
	close_socket(sock);

	return EXIT_SUCCESS;
}
