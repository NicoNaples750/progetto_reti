#include "green_pass.h"
#include "server_common.h"
#include "server_protocols.h"
#include "socket_op.h"
#include "utils.h"
#include <asm-generic/socket.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// Socket fd
int socket_id;
// Server V stuffs
int server_v_socket;
struct sockaddr_in s;
// Network conf
#define MAX_CONNECTIONS 500
// Constants
static const int VALIDITY_DATE = 6; 

void connect_server_v();
void close_connection_server_v();

void cleanup(int sig){
	if (sig == CLOSE_SIGNAL){
		printf("Terminating Centro Vaccinale. Cleaning up resources...\n");
		close(server_v_socket);
		close(socket_id);
		exit(EXIT_SUCCESS);
	}
}


void request_gp(int);

void requests_dispatch(int client_socket, int op){
	switch (op) {
		case CENTRO_VAX_REGISTER_GP: request_gp(client_socket); break;
		default: printf("Requested unknown operation %d. Skipping...\n",op);
	}
}

void request_gp(int client_socket){
	LOG("Requested op: request_gp\n");

	char code[GREEN_PASS_KEY_LENGTH];
	char name[STRING_MAX_LENGTH];
	char surname[STRING_MAX_LENGTH];
	genre sex;
	
	NON_CRITICAL_READ_C(client_socket, code, GREEN_PASS_KEY_LENGTH * sizeof(char), read_string, return);
	LOG("\tReceived code\n");
	NON_CRITICAL_READ_C(client_socket, name, STRING_MAX_LENGTH * sizeof(char), read_string, return);
	LOG("\tReceived name\n");
	NON_CRITICAL_READ_C(client_socket, surname, STRING_MAX_LENGTH * sizeof(char), read_string, return);
	LOG("\tReceived surname\n");
	NON_CRITICAL_READ(client_socket, &sex, sizeof(genre), return);
	LOG("\tReceived sex\n");


	if(!check_green_pass_key(code)){
		failure(client_socket);
		WARNING("Got invalid ID card value\n");
		return;
	}

	connect_server_v();
	request_op(server_v_socket, SERVER_V_REGISTER_GP);
	NON_CRITICAL_SEND(server_v_socket, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0, return);
	LOG("\tSend code\n");
	NON_CRITICAL_SEND(server_v_socket, &VALIDITY_DATE, sizeof(int), 0, return);
	LOG("\tSend validity date\n");
	NON_CRITICAL_SEND(server_v_socket, name, STRLEN(name), 0, return);
	LOG("\tSend nome\n");
	NON_CRITICAL_SEND(server_v_socket, surname, STRLEN(surname), 0, return);
	LOG("\tSend surname\n");
	NON_CRITICAL_SEND(server_v_socket, &sex,  sizeof(genre), 0, return);
	LOG("\tSend sex\n");

	if (get_op_result(server_v_socket) == OP_FAILURE){
		failure(client_socket);
		return;
	}

	success(client_socket);
	close(client_socket);
	close_connection_server_v();
}

void init_structs(){
	s.sin_family = AF_INET;
	s.sin_port = htons(SERVER_V_PORT);
	s.sin_addr.s_addr = inet_addr(LOCALHOST_ADDR);
}

void connect_server_v(){
	if(server_v_socket = socket(AF_INET, SOCK_STREAM, 0), server_v_socket < 0){
		ABORT("Error while creating socket. Aborting....");
	}

	if(connect(server_v_socket, (struct sockaddr*)&s, sizeof(s)) < 0)
		ABORT("Error while connecting to server. Aborting...");
}

void close_connection_server_v(){
	close(server_v_socket);
}



int main(int argc, char** argv){
	init_server(&socket_id, CENTRO_VAX_PORT, cleanup);
	init_structs();
	listen_requests(socket_id, MAX_CONNECTIONS, requests_dispatch);
}