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
// #define DEBUG

char * dest = "\r\n\r\n";
int CNT_NOW = 0;

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
	/**
	 * strstr --> Split it out ... hanle ... return them all together.
	 */
	char * t;
	/* deal pipeline */
	Return_value result = PERSISTENT;	
	dynamic_buffer * each = (dynamic_buffer *)malloc(sizeof(dynamic_buffer));
	init_dynamic_buffer(each);

//	LOG("After Init,begin while\n");
	while((t=strstr(dbuf->buf,dest))!=NULL){
		int len = t - dbuf->buf;
		memset_dynamic_buffer(each);
		append_dynamic_buffer(each, dbuf->buf, len);
		append_dynamic_buffer(each, dest, strlen(dest));
		// Update Access end --> The end of a caption
		dbuf->access_end += each->current;

#ifdef DEBUG
		LOG("Starting dealing with msg\n----------\n%s\n---------\n" ,each->buf);
#endif
		result = handle_request(client_sock, sock, each, cli_addr, dbuf);	
		if(result==CLOSE_FROM_CLIENT){
			// client is asking for close;
			free_dynamic_buffer(each);
			return CLOSE;
		}
		if(result==NOT_COMPLETE){
			// Need to receive more Cannot Return now;
			dbuf->access_end -= each->current;
			free_dynamic_buffer(each);
			return NOT_COMPLETE;
		}		
		// Can be sent to client;
#ifdef DEBUG
	LOG("msg to be sent\n======================= Sending %ld: %ld ======================\n%s\n" ,each->current, strlen(each->buf),each->buf);
#endif	
		if(send(client_sock, each->buf, each->current, 0)!=each->current){
			// Something is wrong
			close_socket(client_sock);
			close_socket(sock);
			free_dynamic_buffer(each);
			ERROR("ERROR Sending Back to Client\n");
			return ERROR;
		}
		if(result==CLOSE){
			break;
		}
		update_dynamic_buffer(dbuf);
	}
	free_buffer_dynamic_buffer(each);
#ifdef DEBUG
	LOG("End with this buf, go back persistently\n");
#endif
	return result;
} 

int main(int argc, char* argv[])
{
	int sock, client_sock;
	ssize_t readret; /* int */
	socklen_t cli_size; /* uint */
	struct sockaddr_in addr, cli_addr_tmp; 
	/*unsigned short int, unsigned int*/
	char buf[BUF_SIZE];
	fd_set tot_fds;
	struct sockaddr_in cli_addr[MAX_FD_SIZE];
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
	int _cnt = 0, _tot = 0;
	/* finally, loop waiting for input and then write it back */
	while (1)
	{
		fd_set tmp_fds = tot_fds;
		PRINTHEAD(ECHO_PORT, _cnt, _tot);
		int cnt, fd;
		if((cnt = select(MAX_FD_SIZE+1, &tmp_fds, NULL, NULL, NULL)) < 1){
			ERRORLOG("Select < 1, No Clients\n");
//			continue;
			return EXIT_FAILURE;
		}
		for(fd = 0; fd< MAX_FD_SIZE;fd++){
			if(!FD_ISSET(fd, &tmp_fds)){
				continue;
			}
			if(!cnt--){
				break;
			}
			if(fd == sock){
				/* 	Client's fd equals to server's socket
				 * --> 	Request for Connection;
				 *  */
				cli_size = sizeof(cli_addr_tmp);
				if ((client_sock = accept(sock, (struct sockaddr *) &cli_addr_tmp ,&cli_size)) == -1)
				{
					close(sock);
					fprintf(stderr, "Error accepting connection.\n");
					return EXIT_FAILURE;
				}
				cli_addr[client_sock] = cli_addr_tmp;
				FD_SET(client_sock, &tot_fds);
				init_dynamic_buffer(ADBUF[client_sock]);
				_cnt ++;
				_tot ++;
				AcceptLog(cli_addr[client_sock], client_sock);
			}else{
				/* Have Connected */
				memset(buf, 0, sizeof(buf));
				if((readret=recv(fd, buf, BUF_SIZE,0))>=1){
					DealLog(cli_addr[fd], fd, "Msg RECV");
#ifdef DEBUG
					LOG("MSG RECVED:\n%s\n" ,buf);
#endif
					append_dynamic_buffer(ADBUF[fd], buf, strlen(buf));
					Return_value ret = deal_buf(ADBUF[fd], readret, fd, sock, cli_addr[client_sock]);
				
					switch(ret){
						case CORRECT: 
							break;
						case ERROR:
							exit(EXIT_FAILURE);
						case CLOSE:
							close_socket(fd);
							free_buffer_dynamic_buffer(ADBUF[fd]);
							FD_CLR(fd, &tot_fds);
							LeaveLog(cli_addr[fd],fd);
							_cnt --;
							break;
						case PERSISTENT:
							DealLog(cli_addr[fd], fd,"Complete with this");
							break;
						default:
							DealLog(cli_addr[fd], fd,"Complete with this");
					}
				}else if(!readret){
					/* Receive Nothing --> Close Connection */
					free_buffer_dynamic_buffer(ADBUF[fd]);
					close_socket(fd);
					FD_CLR(fd, &tot_fds);
					LeaveLog(cli_addr[fd], fd);
					_cnt--;
				}else{
					close_socket(fd);
					ERROR("Readret < 0!\n");
					break;
				}
			}
		} 
	}
	for(i=0;i<MAX_FD_SIZE;i++) free_dynamic_buffer(ADBUF[i]);
	close_socket(sock);
	return EXIT_SUCCESS;
}
