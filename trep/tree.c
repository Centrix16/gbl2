/*
 * tree.c -- tree library for trep interpreter
 * v0.0
 * 30.06.2020
 * by asz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"

void init_unit(unit *uptr, unit *new_parent) {
	uptr->child_num = 0;
	uptr->i = 0;
	uptr->parent = new_parent;
	uptr->is_free = 0;
	uptr->eval_me = 1;

	strcpy(uptr->value, "\0");

	for (int i = 0; i < CHILD_MAX; i++) {
		uptr->child[i] = NULL;
	}
}

void new_child(unit *parent) {
	int child_index = parent->i;
	unit *uptr = NULL;

	if (child_index > CHILD_MAX) {
		fprintf(stderr, "%s: too many childs, i = %d\n", __func__, child_index);
		exit(0);
	}

	uptr = (unit*)malloc(sizeof(unit));
	if (!uptr) {
		fprintf(stderr, "%s: malloc fail\n", __func__);
		exit(0);
	}

	parent->child[child_index] = uptr;
	init_unit(parent->child[child_index], parent);
	parent->child_num++;
	parent->i++;
}

void set_value(unit *uptr, char *new_value) {
//	sprintf(uptr->value, "%s", new_value);	
	strcpy(uptr->value, new_value);
}

unit *get_child(unit *parent, int child_index) {
	if (child_index < parent->child_num && child_index > -1)	
		return parent->child[child_index];
	return NULL;
}

void set_end(unit *uptr) {
	uptr->child_num = 0;
	uptr->child[0] = NULL;
}

int is_end_unit(unit *uptr) {
	if ((uptr->child_num) == 0)
		return 1;
	return 0;
}

void crawl_tree(unit *parent, void (*func)(unit*)) {
	unit *uptr = parent;

	if (!uptr)
		return ;

	if (is_end_unit(uptr)) {
		(*func)(uptr);
		return ;
	}

	for (int i = 0; i < uptr->child_num; i++)
		crawl_tree(get_child(uptr, i), func);
	(*func)(uptr);
}


void show_tree(unit *uptr) {
	if (uptr)
		puts("%s: ", __func__, uptr->value);
}

void del_tree(unit *uptr) {
	if (uptr->parent && uptr->is_free == 0) {
		uptr->child[0] = NULL;
		uptr->child_num = 0;
		uptr->i = 0;
		uptr->is_free = 1;
		uptr->eval_me = 0;
		strcpy(uptr->value, "\0");

		free(uptr);
	}
}

int get_i(unit *uptr) {
	return uptr->i;
}

void refresh_unit(unit *uptr) {
	uptr->i = 0;
	uptr->child_num = 0;
	uptr->child[0] = NULL;
	uptr->eval_me = 1;
	strcpy(uptr->value, "\0");
}
