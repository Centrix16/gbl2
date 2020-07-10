/*
 * bl.c -- base library of trep language
 * v0.3
 * 08.07.2020
 * by asz
 */

#include <string.h>
#include <stdlib.h>

#include "proto.h"

#define BUILT_LEN 6

char *built_in[] = {"output", "let", ";", "input", "~", "exit"};
void (*built_in_funcs[])(unit*) = {output, let, no_eval, input, comment, quit};
extern elm *var_stack;

/* service functions */

int find_s(char **arr, char *str) {
	for (int i = 0; i < BUILT_LEN; i++) {
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
	var *vptr = var_stack->heap;

	for (int i = 0; vptr[i].name; i++) {
		if (!strcmp(vptr[i].name, tok))
			return i;
	}
	return -1;
}

int is_user_func(char *tok) {
	return -1;	
}

void exec(unit *uptr) {
	int result = 0;

	if (!uptr->eval_me)
		return ;

	if (uptr->parent && !strcmp(uptr->parent->value, ";"))
		return ;

	result = is_built_in(uptr->value);
	if (result > -1) {
		(*built_in_funcs[result])(uptr);
		return ;
	}

	result = is_variable(uptr->value);
	if (result > -1) {
		strcpy(uptr->value, get_var_value_stack(var_stack, result));
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
		else
			fputs(tmp, stream);

		i = 1; // first - stream
	}
	for (; i < uptr->child_num; i++) {
		tmp = get_child(uptr, i)->value;	

		if (!strcmp(tmp, "~n"))
			fputc('\n', stream);
		else if (!strcmp(tmp, "~t"))
			fputc('\t', stream);
		else
			fputs(del_sym(get_child(uptr, i)->value, '\"'), stream);	
	}
}

void let(unit *uptr) {
	char *name = NULL;
	char *value = NULL;
	int result = 0;
	var *vptr = NULL;

	for (int i = 0; i < uptr->child_num; i += 2) {
		name = get_child(uptr, i)->value;
		value = get_child(uptr, i+1)->value;

		result = is_variable(name);
		if (result > -1) {
			vptr = get_var_stack(var_stack, result);

			free(vptr->value);
			vptr->value = malloc(strlen(value)+1);
			strcpy(vptr->value, value);
		}
		else
			init_var_stack(var_stack, name, value);
	}
}

void no_eval(unit *uptr) {
	strcpy(uptr->value, uptr->child[0]->value);	
	uptr->eval_me = 0;
}

void input(unit *uptr) {
	FILE *stream = stdin;
	char *tmp = NULL;

	if (uptr->child_num) {
		tmp = get_child(uptr, 0)->value;

		if (!strcmp(tmp, "out"))
			stream = stdout;
		else if (!strcmp(tmp, "in"))
			stream = stdin;
		else if (!strcmp(tmp, "err"))
			stream = stderr;
	}

	fgets(uptr->value, LEN, stream);
	strcpy(uptr->value, del_sym(uptr->value, '\n'));
}

void comment(unit *uptr) { ; }

void quit(unit *uptr) {
	if (uptr->child_num)
		exit(atoi(get_child(uptr, 0)->value));
}
