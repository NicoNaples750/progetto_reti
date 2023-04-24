#ifndef INCLUDE_UTILS
#define INCLUDE_UTILS

#include "defines.h"
#include <bits/types/timer_t.h>
#include <stddef.h>

//Strcpy wrapper to prevent buffer's overflow errors.
//Checks if dest buffer can hold the src string and then safely
//calls strncpy
#define SAFE_STRCPY(s_dest, s_src, dest_size) { \
	if (strlen(s_src) >= dest_size) \
		ABORT("Destination string is too small for the requested copy. Aborting..."); \
	strncpy(s_dest,s_src, dest_size); }\

#define POINTER_CHECK_NULL(ptr) ptr == NULL

#define ABORT(msg) { \
	perror(msg); \
	exit(EXIT_FAILURE); }

#define WARNING(msg) { \
	fprintf(stderr,msg); \
}

#ifdef NDEBUG
#define LOG(msg) \
	fprintf(stdout, msg);
#else
#define LOG(msg)
#endif

#define STRLEN(s) (strlen(s) + 1) * sizeof(char)
#define SIZEOF(n,type) n * sizeof(type)

//remove newline from stdin
#define CLEAN_STDIN() {char ch; while ((ch = getchar()) != '\n' && ch != EOF);} 

#define FGETS_NO_NEWLINE(var,len) { \
	fgets(var,len,stdin); \
	if(var[strlen(var) - 1] == '\n'){ \
		var[strlen(var) - 1] = '\0'; \
	}else{ \
		CLEAN_STDIN(); \
	}}

//Read wrapper that kills execution if an error occurs
#define CRITICAL_READ(fd,buf,size) { \
	if(read(fd,buf,size) <= 0) \
		ABORT("Error while reading. Aborting..."); \
}

//Function wrapper that issues a warning if an error occurs
//within 'read_fun' function, and run the 'fallback' command
#define NON_CRITICAL_READ_C(fd,buf,size,read_fun,fallback) \
	if(read_fun(fd,buf,size) <= 0){ \
		WARNING("Error while reading\n"); \
		fallback; \
	} \

//A non-critical C standard read
#define NON_CRITICAL_READ(fd,buf,size,fallback) \
	NON_CRITICAL_READ_C(fd,buf,size,read,fallback)

//Send wrapper that aborts execution if an error occurs
#define CRITICAL_SEND(fd,buf,size,flags) { \
	if(send(fd,buf,size,flags) <= 0) \
		ABORT("Error while writing. Aborting..."); \
}

//Send wrapper that issues a warning if an error occurs, 
//then runs the fallback command
#define NON_CRITICAL_SEND(fd,buf,size,flags, fallback) { \
	if(send(fd,buf,size,flags) <= 0) { \
		WARNING("Error while writing\n"); \
		fallback; \
	} \
}


bool is_date(char* buff){
	if(POINTER_CHECK_NULL(buff) || strlen(buff) < DATE_LENGTH)
		return false;
	struct tm tm;
	return strptime(buff, DATE_FORMAT, &tm) != NULL;
}

char* parse_date(struct tm* date){
	char* buf = malloc(DATE_LENGTH * sizeof(char));
	strftime(buf, DATE_LENGTH * sizeof(char), DATE_FORMAT, date);
	return buf;
}

struct tm* get_current_date(){
	time_t timer = time(NULL);
	return localtime(&timer);
}

void extend_date(struct tm* current, int months){
	current->tm_mon = current->tm_mon + months;
	mktime(current);
}



#endif /* INCLUDE_UTILS */
