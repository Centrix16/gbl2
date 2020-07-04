/*
 * bl.c -- base library of trep language
 * v0.0
 * 30.06.2020
 * by asz
 */

#include <string.h>

#include "proto.h"

char *built_in[] = {"output"};
void (*built_in_funcs[])(unit*) = {output};

/* service functions */

int find_s(char **arr, char *str) {
	for (int i = 0; i < sizeof(arr) / sizeof(char*); i++) {
		if (!strcmp(arr[i], str)) {
			return i;
		}
	}
	return -1;
}

int is_built_in(char *tok) {
	return find_s(built_in, tok);
}

int is_variable(char *tok) {
	return 0;
}

int is_user_func(char *tok) {
	return 0;	
}

void exec(unit *uptr) {
	int result = 0;

	result = is_built_in(uptr->value);
	if (result > -1) {
		(*built_in_funcs[result])(uptr);
		return ;
	}

	result = is_variable(uptr->value);
	if (result > -1) {
		// code for variable
		return ;
	}

	result = is_user_func(uptr->value);
	if (result > -1) {
		// code for user func
		return ;	
	}
}

int is_str(char *tok) {
	return strchr(tok, '\"') ? 1 : 0;	
}

/* lang functions */

void output(unit *uptr) {
	if ()	
	for (; i < uptr->child_num; i++)
		puts(get_child(uptr, i)->value);	
}
