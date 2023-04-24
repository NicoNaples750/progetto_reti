#ifndef INCLUDE_SERVER_COMMON
#define INCLUDE_SERVER_COMMON

#include "utils.h"
#include <bits/pthreadtypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "server_protocols.h"
#include "socket_op.h"
#include <stdatomic.h>

//Current active threads
atomic_int active_threads;

/**
 * @brief Init a simple server
 * 
 * @param socket_id Output socket of the server
 * @param port Port to be bind
 * @param cleanup Cleanup function when shutting down
 */
void init_server(int* socket_id, int port, void (*cleanup)(int)){
	signal(CLOSE_SIGNAL, cleanup);
	
	struct sockaddr_in socket_props;

	if(*socket_id = socket(AF_INET, SOCK_STREAM, 0), *socket_id == -1){
		ABORT("Error while creating socket. Aborting...")
	}

	socket_props.sin_family = AF_INET;
	socket_props.sin_addr.s_addr = inet_addr(LOCALHOST_ADDR);
	socket_props.sin_port = htons(port);

	int option = 1;
	// Set the address as usable instantly after the socket is closed
	setsockopt(*socket_id, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
	if(bind(*socket_id, (struct sockaddr*)&socket_props, sizeof(socket_props)) == -1){
		ABORT("Error while binding socket address. Aborting...")
	}

	//Init active threads
	atomic_init(&active_threads, 0);
}

/**
 * @brief Handle a request. This is passed to a newly created thread
 * 
 * @param args Thread args (socket + dispatch function)
 * @return void* 
 */
void* handle_request(void* args){
	atomic_fetch_add(&active_threads, 1);
	thread_args* t_args = (thread_args*) args;
	int op = get_requested_op(t_args->socket);
	t_args->dispatch_fun(t_args->socket, op);
	atomic_fetch_add(&active_threads, -1);
	pthread_exit(0);
}

/**
 * @brief Listen and fulfill requests
 * 
 * @param socket_id Socket to listen on
 * @param max_conn Max connection that can be fullfilled together
 * @param dispatch_fun Function that select the requested server op
 */
void listen_requests(int socket_id, int max_conn, void (*dispatch_fun)(int,int)){
	int list_res;
	
	struct sockaddr_in client_props;
	int client_props_len = sizeof(client_props);
	socklen_t* client_len_wrapper = (socklen_t*) &client_props_len;
	struct sockaddr* client_props_wrapper = (struct sockaddr*) &client_props;

	do {
		printf("Listening...\n");
		if(list_res = listen(socket_id, max_conn), list_res < 0){
			ABORT("An error occurred while listening on socket. Aborting...")
		}

		int client_socket = accept(socket_id, client_props_wrapper, client_len_wrapper);

		//Spin waiting to free a thread
		while (atomic_load(&active_threads) == THREAD_POOL_SIZE) 
		{
			// printf("Thread pool full. Waiting...\n");
			// sleep(1);
		}

		pthread_t thread;
		thread_args args;
		args.socket = client_socket;
		args.dispatch_fun = dispatch_fun;
		LOG("Launching thread...\n");
		pthread_create(&thread, NULL, handle_request,&args);
		
	}while (true);
}

#endif /* INCLUDE_SERVER_COMMON */
