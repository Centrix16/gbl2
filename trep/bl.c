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

char *del_sym(char *str, int symbol) {
	static char buf[256] = "";
	int i, j;

	for (i = 0, j = 0; str[i] && j < 256; i++) {
		if (str[i] != symbol)
			buf[j++] = str[i];
	}
	buf[j] = '\0';
	return buf;
}

/* lang functions */

void output(unit *uptr) {
	char *tmp = NULL;
	FILE *stream = stdout;
	int i = 0;

	tmp = get_child(uptr, 0)->value;
	if (!is_str(tmp)) {
		if (!strcmp(tmp, "out"))
			; // stream = stdout
		else if (!strcmp(tmp, "in"))
			stream = stdin;
		else if (!strcmp(tmp, "err"))
			stream = stderr;	

		i = 1; // first - stream
	}
	for (; i < uptr->child_num; i++) {
		tmp = get_child(uptr, i)->value;	

		if (is_str(tmp))
			fputs(del_sym(get_child(uptr, i)->value, '\"'), stream);	
		else {
			if (!strcmp(tmp, "~n"))
				fputc('\n', stream);
			else if (!strcmp(tmp, "~t"))
				fputc('\t', stream);
		}
	}
}
