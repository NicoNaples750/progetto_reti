#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "server_common.h"
#include "server_protocols.h"
#include "socket_op.h"
#include "green_pass.h"
#include "list.h"
#include "defines.h"
#include "utils.h"

// Socket fd
int socket_id = 0;
int server_v_socket = 0;
struct sockaddr_in server_v_props;
// Network conf
#define MAX_CONNECTIONS 500

void verify_gp(int);
void validate_invalidate_gp(int);

void cleanup(int sig){
	if (sig == CLOSE_SIGNAL){
		printf("Terminating Server G. Cleaning up resources...\n");
		close(socket_id);
		exit(EXIT_SUCCESS);
	}
}

void init_structs(){
	server_v_props.sin_family = AF_INET;
	server_v_props.sin_port = htons(SERVER_V_PORT);
	server_v_props.sin_addr.s_addr = inet_addr(LOCALHOST_ADDR);
}

void connect_server_v(){
	if(server_v_socket = socket(AF_INET, SOCK_STREAM, 0), server_v_socket < 0){
		ABORT("Error while creating socket. Aborting....");
	}

	if(connect(server_v_socket, (struct sockaddr*)&server_v_props, sizeof(server_v_props)) < 0)
		ABORT("Error while connecting to server. Aborting...");
}

void close_connection_server_v(){
	close(server_v_socket);
}

void requests_dispatch(int client_socket, int op){
	switch (op) {
		case SERVER_G_VERIFY_GP: verify_gp(client_socket); break;
		case SERVER_G_VALIDATE_INVALIDATE_GP: validate_invalidate_gp(client_socket); break;
		default: printf("Requested unknown operation %d. Skipping...\n",op);
	}
}

void verify_gp(int client_socket){
	LOG("Requested op: verify_gp\n");

	char code[GREEN_PASS_KEY_LENGTH];
	NON_CRITICAL_READ(client_socket, code, sizeof(code), return);

	if(!check_green_pass_key(code)){
		WARNING("Received malformed green pass key. Aborting\n");
		failure(client_socket);
		return;
	}

	connect_server_v();
	request_op(server_v_socket, SERVER_V_VERIFY_GP);
	NON_CRITICAL_SEND(server_v_socket, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0, return);
	int res = get_op_result(server_v_socket);
	if(res == OP_FAILURE){
		WARNING("Error while veryfing Green Pass. Aborting\n");
		failure(client_socket);
		return;
	}
	bool is_valid;
	NON_CRITICAL_READ(server_v_socket, &is_valid, sizeof(bool), return);
	success(client_socket);
	NON_CRITICAL_SEND(client_socket, &is_valid, sizeof(bool), 0, return);

	close(client_socket);
	close_connection_server_v();
	LOG("Request fulfilled\n")
}

void validate_invalidate_gp(int client_socket){
	LOG("Requested op: validate_invalidate_gp\n");

	char code[GREEN_PASS_KEY_LENGTH];
	NON_CRITICAL_READ(client_socket, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), return);

	if(!check_green_pass_key(code)){
		WARNING("Received malformed green pass key. Aborting\n");
		failure(client_socket);
		return;
	}

	connect_server_v();
	request_op(server_v_socket, SERVER_V_VALIDATE_INVALIDATE_GP);
	NON_CRITICAL_SEND(server_v_socket, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0, return);
	int res = get_op_result(server_v_socket);
	if(res == OP_FAILURE){
		WARNING("Error while changing Green Pass validation. Aborting\n");
		failure(client_socket);
		return;
	}
	bool validity;
	NON_CRITICAL_READ(server_v_socket, &validity, sizeof(bool), return);
	success(client_socket);
	NON_CRITICAL_SEND(client_socket, &validity, sizeof(bool), 0, return);

	close(client_socket);
	close_connection_server_v();
	LOG("Request fulfilled\n")
}


int main(int argc, char** argv){
	init_server(&socket_id, SERVER_G_PORT, cleanup);
	init_structs();
	listen_requests(socket_id, MAX_CONNECTIONS, requests_dispatch);
}