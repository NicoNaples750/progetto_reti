#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "defines.h"
#include "server_protocols.h"
#include "socket_op.h"
#include "green_pass.h"
#include "utils.h"

#define PRINT_GP 1
int client_fd;


void cleanup(int sig){
	if (sig == CLOSE_SIGNAL){
		printf("Terminating Client utility. Cleaning up resources...\n");
		close(client_fd);
		exit(EXIT_SUCCESS);
	}
}

void print_gp(int server_socket){
	request_op(server_socket, SERVER_V_PRINT_GP);
	
	char code[GREEN_PASS_KEY_LENGTH];	
	printf("Inserisci il codice della tessera: ");
	fflush(stdout);
	FGETS_NO_NEWLINE(code, GREEN_PASS_KEY_LENGTH);
	
	CRITICAL_SEND(server_socket, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0);
	LOG("code sended\n");
	
	if(get_op_result(server_socket) == OP_FAILURE){
		ABORT("Error while printing Green pass. Aborting...\n");
	}
}

int main(){
	signal(CLOSE_SIGNAL, cleanup);
	struct sockaddr_in s;

	if(client_fd = socket(AF_INET, SOCK_STREAM, 0), client_fd < 0){
		ABORT("Error while creating client socket. Aborting....");
	}

	s.sin_family = AF_INET;
	s.sin_port = htons(SERVER_V_PORT);
	s.sin_addr.s_addr = INADDR_ANY;

	if(connect(client_fd, (struct sockaddr*)&s, sizeof(s)) < 0)
		ABORT("Error while connecting to server. Aborting...");

	do {
		int choice;
		printf("Scegli l'operazione da eseguire:\n");
		printf("\t(1) Stampa un green pass\n");

		scanf("%d/n",&choice);
		CLEAN_STDIN();
		switch (choice) {
			case PRINT_GP: print_gp(client_fd); break;
			default:
				printf("Invalid operation %d selected. Skipping...\n", choice);
		}
	}while (true);

	return 0;
}