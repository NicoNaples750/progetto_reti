#ifndef INCLUDE_GREEN_PASS
#define INCLUDE_GREEN_PASS

#include "defines.h"
#include "utils.h"

typedef enum genre {m = 0, f = 1} genre;

// Convert a char to a genre object
static const int parse_genre(char c, genre* g){
	if(c == 'm'){
		*g = m;
		return 0;
	}
	if(c == 'f'){
		*g = f;
		return 0;
	}
	return -1;
}

// Get the string representation from the genre
char* genre_string(genre g){
	if(g == m)
		return "male";
	if(g == f)
		return "female";
	return NULL;
}


typedef struct _green_pass{
	char key[GREEN_PASS_KEY_LENGTH];
	char name[STRING_MAX_LENGTH];
	char surname[STRING_MAX_LENGTH];
	genre sex;
	char issuing_date[DATE_LENGTH];
	char expiring_date[DATE_LENGTH];
	int validity_months;
	bool is_valid;
} _green_pass;
typedef _green_pass* green_pass;

/**
 * @brief Create a new empty green pass
 * 
 * @return green_pass 
 */
green_pass new_empty_green_pass(){
	return malloc(sizeof(_green_pass));
}

/**
 * @brief Create a new green pass
 * 
 * @param key Green pass key
 * @param name Person name
 * @param surname Person surname
 * @param sex Person genre
 * @param issuing_date Green pass release day
 * @param expiring_date Green pass expiration day
 * @param validity_months Green pass validity span
 * @param is_valid If the green pass is valid
 * @return green_pass 
 */
green_pass new_green_pass(char* key,
						  char* name,
						  char* surname,
						  genre sex,
						  char* issuing_date,
						  char* expiring_date,
						  int validity_months,
						  bool is_valid)
{
	green_pass gp = (green_pass) malloc(sizeof(_green_pass));
	SAFE_STRCPY(gp->key,key, sizeof(gp->key))
	SAFE_STRCPY(gp->name, name, sizeof(gp->name));
	SAFE_STRCPY(gp->name, surname, sizeof(gp->surname));
	gp->sex = sex;
	if(is_date(issuing_date)){
		SAFE_STRCPY(gp->issuing_date, issuing_date, DATE_LENGTH)
	}
	if(is_date(expiring_date)){
		SAFE_STRCPY(gp->expiring_date, expiring_date, DATE_LENGTH)
	}
	gp->validity_months = validity_months;
	gp->is_valid = is_valid;
	return gp;
}

/**
 * @brief Check if key is a well-formatted green pass key
 * 
 * @param key The key to check
 * @return true if the check is well-formed 
 * @return false if key is NULL or it doesn't follow the requirements
 */
bool check_green_pass_key(char* key){
	return !(POINTER_CHECK_NULL(key) || strlen(key) != GREEN_PASS_KEY_LENGTH - 1);
}

/**
 * @brief Check if the green pass is legaly valid. This depends
 *		  on whatever the green pass is mark as valid and/or it has not expired
 * 
 * @param gp 
 * @return true 
 * @return false 
 */
bool is_legally_valid(green_pass gp){
	struct tm issuing_date, expiring_date;
	strptime(gp->issuing_date, DATE_FORMAT, &issuing_date);
	strptime(gp->expiring_date, DATE_FORMAT, &expiring_date);

	double diff = difftime(mktime(&expiring_date), mktime(&issuing_date));

	return gp->is_valid && diff >= 0;
	
}

inline bool check_green_pass_validity(green_pass gp){
	return check_green_pass_key(gp->key) &&
		   is_date(gp->issuing_date) &&
		   is_date(gp->expiring_date) && 
		   is_legally_valid(gp);
}


void print_green_pass(green_pass gp){
	printf("key: %-30s\nname: %-30s\nsurname: %-30s\nsex: %-30s\nissuing_date: %-30s\nexpiring_date: %-30s\nvalidity_months: %-30d\nis_valid: %-30d\n",
		gp->key,
		gp->name,
		gp->surname,
		genre_string(gp->sex),
		gp->issuing_date,
		gp->expiring_date,
		gp->validity_months,
		gp->is_valid
	);
}


#endif /* INCLUDE_GREEN_PASS */
