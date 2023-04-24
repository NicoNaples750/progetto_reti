#ifndef INCLUDE_SERVER_PROTOCOLS
#define INCLUDE_SERVER_PROTOCOLS

/**
* This file holds the code for requesting an operation to a specific server
*/

//Common
const int OP_SUCCESS = 1;
const int OP_FAILURE = 0;
#define THREAD_POOL_SIZE 50
#define LOCALHOST_ADDR "127.0.0.1"

typedef struct thread_args{
	int socket;
	void (*dispatch_fun)(int,int);
} thread_args;

// Server V
#define SERVER_V_PORT 2000
#define SERVER_V_REGISTER_GP 1
#define SERVER_V_VERIFY_GP 2
#define SERVER_V_VALIDATE_INVALIDATE_GP 3
#define SERVER_V_PRINT_GP 4

// Server G
#define SERVER_G_PORT 2001
#define SERVER_G_VERIFY_GP 1
#define SERVER_G_VALIDATE_INVALIDATE_GP 2

// Centro Vaccinale
#define CENTRO_VAX_PORT 2002
#define CENTRO_VAX_REGISTER_GP 1

#endif /* INCLUDE_SERVER_PROTOCOLS */
