/*
 * var.c -- variable library for trep interpreter
 * v0.2
 * 08.07.2020
 * by asz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"

var *new_var_area(int len) {
	size_t size = sizeof(var);
	var *vptr = NULL;

	vptr = (var*)calloc(len, size);
	if (!vptr) {
		perror(__func__);
		exit(0);
	}
	return vptr;
}

void free_var_area(var *vptr) {
	if (vptr)
		free(vptr);
}

void init_var(var *vptr, int indx, char *name, char *val) {
	vptr[indx].name = malloc((strlen(name)+1) * sizeof(char));
	vptr[indx].value = malloc((strlen(val)+1) * sizeof(char));

	strcpy(vptr[indx].name, name);
	strcpy(vptr[indx].value, val);
}

void init_var_stack(elm *stack, char *name, char *val) {
	var *vptr = stack->heap;	
	int indx = stack->var_indx;

	vptr[indx].name = malloc((strlen(name)+1) * sizeof(char));
	vptr[indx].value = malloc((strlen(val)+1) * sizeof(char));

	strcpy(vptr[indx].name, name);
	strcpy(vptr[indx].value, val);

	stack->var_indx++;
}

void free_var(var *vptr, int indx) {
	if (vptr[indx].name && vptr[indx].value) {
		free(vptr[indx].name);
		free(vptr[indx].value);
	}
}

void del_var_area(var *vptr) {
	for (int i = 0; vptr[i].name; i++) {
		free_var(vptr, i);
	}
	free_var_area(vptr);
}

void crawl_heap(var *vptr, void (*func)(var*)) {
	for (int i = 0; vptr[i].name; i++)
		(*func)(&vptr[i]);
}

void show_var(var *vptr) {
	printf("%s = %s\n", vptr->name, vptr->value);	
}

char *get_var_value_stack(elm *stack, int indx) {
	return stack->heap[indx].value;
}

var *get_var_stack(elm *stack, int indx) {
	return &stack->heap[indx];	
}
