#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

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
	s.sin_port = htons(CENTRO_VAX_PORT);
	s.sin_addr.s_addr = INADDR_ANY;

	if(connect(client_fd, (struct sockaddr*)&s, sizeof(s)) < 0)
		ABORT("Error while connecting to server. Aborting...");
	
	request_op(client_fd, CENTRO_VAX_REGISTER_GP);

	char code[GREEN_PASS_KEY_LENGTH];
	char name[STRING_MAX_LENGTH];
	char surname[STRING_MAX_LENGTH];
	genre sex;
	char sex_c;
	
	printf("Inserisci il codice della tessera: ");
	FGETS_NO_NEWLINE(code, GREEN_PASS_KEY_LENGTH);
	printf("Inserisci il nome: ");
	FGETS_NO_NEWLINE(name, STRING_MAX_LENGTH);
	printf("Inserisci il cognome: ");
	FGETS_NO_NEWLINE(surname, STRING_MAX_LENGTH);
	printf("Inserisci il sesso (m,f): ");
	scanf("%c",&sex_c);
	if(parse_genre(sex_c, &sex) < 0){
		ABORT("Unrecognized genre selected. Aborting...\n");
	}

	CRITICAL_SEND(client_fd, code, SIZEOF(GREEN_PASS_KEY_LENGTH,char), 0);
	LOG("code sended\n");
	CRITICAL_SEND(client_fd, name, STRLEN(name), 0);
	LOG("name sended\n");
	CRITICAL_SEND(client_fd, surname, STRLEN(surname), 0);
	LOG("surname sended\n");
	CRITICAL_SEND(client_fd, &sex,  sizeof(genre), 0);
	LOG("sex sended\n");

	if(get_op_result(client_fd) == OP_FAILURE)
		fprintf(stderr, "Error while registering green pass.\n");
	else
		fprintf(stdout, "Green pass successfully registered.\n");

	close(client_fd);
	return 0;
}