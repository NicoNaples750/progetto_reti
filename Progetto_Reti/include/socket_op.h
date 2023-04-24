#ifndef INCLUDE_SOCKET_OP
#define INCLUDE_SOCKET_OP

#include "utils.h"
#include "server_protocols.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int get_requested_op(int socket_fd){
	int req;
	if (read(socket_fd, &req, sizeof(int)) < 0){
		ABORT("Error while getting requested operation. Aborting...");
	}
	return req;
}

int request_op(int socket_fd, int code){
	return send(socket_fd, &code, sizeof(int), MSG_MORE) > 0;
}

int success(int socket_fd){
	return send(socket_fd, &OP_SUCCESS, sizeof(int),0) > 0;
}

int failure(int socket_fd){
	return send(socket_fd, &OP_FAILURE, sizeof(int),0) > 0;
}

int get_op_result(int socket_fd){
	int res;
	if (read(socket_fd, &res, sizeof(int)) < 0){
		ABORT("Error while getting requested operation. Aborting...");
	}
	return res;
}

int read_string(int socket, char* dst, int size){
	int cur;
	for (cur = 0; cur < size; cur++) {
		if(read(socket, &dst[cur], sizeof(char)) < 0){
			return -1;
		}
		if(dst[cur] == '\0')
			return 1;
	}
	return dst[cur] == '\0';
}




#endif /* INCLUDE_SOCKET_OP */
