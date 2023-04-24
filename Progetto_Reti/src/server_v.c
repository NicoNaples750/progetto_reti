#include <bits/pthreadtypes.h>
#include <pthread.h>
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
pthread_mutex_t lock;

list green_pass_db;
// Network conf
#define MAX_CONNECTIONS 500

void request_gp(int);
void verify_gp(int);
void validate_invalidate_gp(int);
void print_gp(int);

void cleanup(int sig){
	if (sig == CLOSE_SIGNAL){
		printf("Terminating Server V. Cleaning up resources...\n");
		close(socket_id);
		exit(EXIT_SUCCESS);
	}
}

void requests_dispatch(int client_socket, int op){
	switch (op) {
		case SERVER_V_REGISTER_GP: request_gp(client_socket); break;
		case SERVER_V_VERIFY_GP: verify_gp(client_socket); break;
		case SERVER_V_VALIDATE_INVALIDATE_GP: validate_invalidate_gp(client_socket); break;
		case SERVER_V_PRINT_GP: print_gp(client_socket); break;
		default: printf("Requested unknown operation %d. Skipping...\n",op);
	}
}

/**
* Create new green pass and store it
*/
void request_gp(int client_socket){
	pthread_mutex_lock(&lock);

	LOG("Requested op: request_gp\n");
	green_pass gp = new_empty_green_pass();

	NON_CRITICAL_READ(client_socket, gp->key, GREEN_PASS_KEY_LENGTH * sizeof(char),return);
	NON_CRITICAL_READ(client_socket, &gp->validity_months, sizeof(int),return);
	
	if(gp->validity_months < 0){
		failure(client_socket);
		WARNING("Got invalid date during Green Pass request");
		return;
	}
	NON_CRITICAL_READ_C(client_socket, gp->name, STRING_MAX_LENGTH, read_string, return);
	NON_CRITICAL_READ_C(client_socket, gp->surname, STRING_MAX_LENGTH, read_string, return);
	NON_CRITICAL_READ(client_socket, &gp->sex, sizeof(genre), return);

	// Create issuing and expiring date
	struct tm* current_date = get_current_date();
	char* current_date_s = parse_date(current_date);
	extend_date(current_date, gp->validity_months);
	char* expiring_date = parse_date(current_date);
	SAFE_STRCPY(gp->issuing_date, current_date_s, strlen(current_date_s) + 1);
	SAFE_STRCPY(gp->expiring_date, expiring_date, strlen(expiring_date) + 1);
	gp->is_valid = true;

	if(exist(green_pass_db, gp->key)){
		WARNING("Inserting already-existing gp. Aborted\n");
		failure(client_socket);
		return;
	}
	insert(green_pass_db, gp);
	pthread_mutex_unlock(&lock);
	
	success(client_socket);
	close(client_socket);
	LOG("Green pass successfully registered \n");
}

/**
* Check if green pass is valid
*/
void verify_gp(int client_socket){
	pthread_mutex_lock(&lock);

	LOG("Requested op: verify_gp\n");
	
	char code[GREEN_PASS_KEY_LENGTH];
	NON_CRITICAL_READ(client_socket, code, GREEN_PASS_KEY_LENGTH * sizeof(char),return);
	
	if(!check_green_pass_key(code)){
		failure(client_socket);
		WARNING("Got invalid ID card value\n");
		return;
	}
	
	green_pass gp = find_elem(green_pass_db, code);
	if(gp == NULL){
		failure(client_socket);
		WARNING("Green pass not found\n");
		return;
	}

	success(client_socket);
	NON_CRITICAL_SEND(client_socket, &gp->is_valid, sizeof(bool), 0, return);
	pthread_mutex_unlock(&lock);

	close(client_socket);
	LOG("Request fullfilled\n");
}

/**
* Change green pass validity
*/
void validate_invalidate_gp(int client_socket){
	pthread_mutex_lock(&lock);
	LOG("Requested op: validate_invalidate_gp\n");
	char code[GREEN_PASS_KEY_LENGTH];
	NON_CRITICAL_READ(client_socket, code, GREEN_PASS_KEY_LENGTH * sizeof(char),return);
	
	if(!check_green_pass_key(code)){
		failure(client_socket);
		WARNING("Got invalid ID card value\n");
		return;
	}
	green_pass gp = find_elem(green_pass_db, code);
	if(gp == NULL){
		failure(client_socket);
		WARNING("Green pass not found\n");
		return;
	}

	gp->is_valid = !gp->is_valid;
	if(gp->is_valid){
		LOG("Validated green pass\n")
	}else{
		LOG("Invalidated green pass\n")
	}
	
	success(client_socket);
	NON_CRITICAL_SEND(client_socket, &gp->is_valid, sizeof(bool), 0, return);
	
	pthread_mutex_unlock(&lock);
	LOG("Request fullfilled\n");
	close(client_socket);
}

/**
* Print a green pass
*/
void print_gp(int client_socket){
	pthread_mutex_lock(&lock);
	LOG("Requested op: print_gp\n");
	char code[GREEN_PASS_KEY_LENGTH];
	NON_CRITICAL_READ(client_socket, code, GREEN_PASS_KEY_LENGTH * sizeof(char),return);
	
	if(!check_green_pass_key(code)){
		failure(client_socket);
		WARNING("Got invalid ID card value\n");
		return;
	}
	green_pass gp = find_elem(green_pass_db, code);
	if(gp == NULL){
		failure(client_socket);
		WARNING("Green pass not found\n");
		return;
	}

	print_green_pass(gp);
	pthread_mutex_unlock(&lock);
	
	success(client_socket);
	close(client_socket);
}	


void init_structs(){
	green_pass_db = new_list();
}

int main(int argc, char** argv){
	init_server(&socket_id, SERVER_V_PORT, cleanup);
	init_structs();
	listen_requests(socket_id, MAX_CONNECTIONS, requests_dispatch);
}
