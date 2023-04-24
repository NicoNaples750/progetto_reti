#ifndef INCLUDE_DEFINES
#define INCLUDE_DEFINES

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
// Perror 
#include <stdio.h>
#include <errno.h>

#define CLOSE_SIGNAL SIGINT
// time.h macro
#define __USE_XOPEN 1
#include "time.h"
#define DATE_LENGTH 11
#define DATE_FORMAT "%d/%m/%Y"

#define GREEN_PASS_KEY_LENGTH 21
#define STRING_MAX_LENGTH 256


#endif /* INCLUDE_DEFINES */
