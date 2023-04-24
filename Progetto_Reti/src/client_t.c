#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include "server_protocols.h"
#include "socket_op.h"
#include "green_pass.h"
#include "utils.h"

int main(){
	struct sockaddr_in s;
	int client_fd;

	if(client_fd = socket(AF_INET, SOCK_STREAM, 0), client_fd < 0){
		ABORT("Error while creating client socket. Aborting....");
	}

	s.sin_family = AF_INET;
	s.sin_port = htons(SERVER_G_PORT);
	s.sin_addr.s_addr = INADDR_ANY;

	if(connect(client_fd, (struct sockaddr*)&s, sizeof(s)) < 0)
		ABORT("Error while connecting to server. Aborting...");
	
	request_op(client_fd, SERVER_G_VALIDATE_INVALIDATE_GP);

	char code[GREEN_PASS_KEY_LENGTH];
	
	printf("Inserisci il codice della tessera: ");
	FGETS_NO_NEWLINE(code, GREEN_PASS_KEY_LENGTH);

	CRITICAL_SEND(client_fd, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0);
	LOG("code sended\n");

	if(get_op_result(client_fd) == OP_FAILURE){
		ABORT("Error while verifying Green pass. Aborting...\n");
	}

	bool res;
	CRITICAL_READ(client_fd, &res, sizeof(bool));
	fprintf(stdout, "The green pass is now %s\n", res == true ? "valid" : "invalid");

	close(client_fd);
	return 0;
}