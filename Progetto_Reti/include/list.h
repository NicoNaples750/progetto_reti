#ifndef INCLUDE_LIST
#define INCLUDE_LIST

#include "green_pass.h"
#include "utils.h" 

#define INIT_SIZE 20

typedef struct _list{
	green_pass* data;
	int size;
	int max_size;
} _list;

typedef _list* list;

green_pass find_elem(list l, char* key){
	for(int i = 0; i < l->size; i++){
		if(strcmp(l->data[i]->key, key) == 0)
			return l->data[i];
	}
	return NULL;
}

bool exist(list l, char* key){
	return find_elem(l, key) != NULL;
}

list new_list(){
	list l = malloc(sizeof(_list));
	l->data = malloc(sizeof(green_pass) * INIT_SIZE);
	l->size = 0;
	l->max_size = INIT_SIZE;

	return l;
} 

void expand(list l, int new_size){
	if (new_size < l->size){
		WARNING("Shrinking the list, data will be lost\n");
	}
	l->data = realloc(l->data, new_size);
	l->max_size = new_size;
}

void insert(list l, green_pass gp){
	if(exist(l, gp->key))
		return;
		
	if(l->size == l->max_size)
		expand(l, l->max_size * 2);
	l->data[l->size++] = gp;
}

green_pass get_elem(list l, int pos){
	if(pos >= l->size)
		ABORT("Accessed non-existing element in list. Aborting...");
	return l->data[pos];
}



#endif /* INCLUDE_LIST */
