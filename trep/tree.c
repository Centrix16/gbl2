/*
 * tree.c -- tree library for trep interpreter
 * v0.2
 * 27.07.2020
 * by asz
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"

size_t memory = 0;
int force_crawl = 0;

void unit_init(unit *uptr, unit *new_parent) {
	uptr->child_num = 0;
	uptr->ret_value = NULL;

	strcpy(uptr->value, "\0");

	for (int i = 0; i < uptr->child_num; i++)
		uptr->child[i] = NULL;	
	if (new_parent != uptr)
		uptr->parent = new_parent;
}

void unit_new_child(unit *parent) {
	int child_index = parent->child_num;
	unit *uptr = NULL;

	if (child_index > CHILD_MAX) {
		fprintf(stderr, "%s: too many childs, i = %d\n", __func__, child_index);
		exit(0);
	}

	memory += sizeof(unit);
	uptr = (unit*)malloc(sizeof(unit));
	if (!uptr) {
		fprintf(stderr, "%s: malloc fail\n", __func__);
		exit(0);
	}

	parent->child[child_index] = uptr;
	parent->child_num++;
	unit_init(parent->child[child_index], parent);
}

void unit_set_value(unit *uptr, char *new_value) {
	strcpy(uptr->value, new_value);
}

unit *unit_get_child(unit *parent, int child_index) {
	if (child_index < parent->child_num && child_index > -1)	
		return parent->child[child_index];
	return NULL;
}

int unit_is_end(unit *uptr) {
	if ((uptr->child_num) == 0)
		return 1;
	return 0;
}

void crawl_tree(unit *parent, void (*func)(unit*)) {
	unit *uptr = parent;

	if (!uptr)
		return ;

	if (!strcmp(uptr->value, ";") && !force_crawl)
		return ;

	if (unit_is_end(uptr)) {
		(*func)(uptr);
		return ;
	}

	for (int i = 0; i < uptr->child_num; i++)
		crawl_tree(unit_get_child(uptr, i), func);
	(*func)(uptr);
}

void unit_show(unit *uptr) {
	if (uptr)
		printf("%s\n", uptr->value);
}

void unit_free(unit *uptr) {
	if (uptr->parent) {
		if (uptr->ret_value)
			free(uptr->ret_value);

		free(uptr);
		memory -= sizeof(unit);
	}
}

int unit_get_i(unit *uptr) {
	return uptr->child_num;
}

void unit_set_ret_value(unit *uptr, char *value) {
	char *sptr = NULL;

	if (uptr->ret_value) {
		sptr = realloc(uptr->ret_value, (strlen(value)+1) * sizeof(char));
		if (!sptr) {
			perror(__func__);
			exit(0);
		}

		uptr->ret_value = sptr;
	}
	else {
		uptr->ret_value = malloc((strlen(value)+1) * sizeof(char));
		if (!uptr->ret_value) {
			perror(__func__);
			exit(0);
		}
	}
	strcpy(uptr->ret_value, value);
}

void free_tree(unit *uptr) {
	force_crawl = 1;

	crawl_tree(uptr, unit_free);

	force_crawl = 0;
}

void print_tree(unit *uptr) {
	force_crawl = 1;

	crawl_tree(uptr, unit_show);

	force_crawl = 0;
}

void unit_copy(unit *to, unit *from) {
	if (to && from)
		strcpy(to->value, from->value);
	else
		return ;

	if (to->ret_value && from->ret_value)
		strcpy(to->ret_value, from->ret_value);

	to->child_num = from->child_num;
	to->parent = from->parent;

	for (int i = 0; i < from->child_num; i++)
		to->child[i] = from->child[i];
}

void tree_copy(unit **dst, unit *src) {
	if (!src)
		return ;

	if (!dst)
		*dst = (unit*)malloc(sizeof(unit));

	unit_copy(*dst, src);
	for (int i = 0; i < src->child_num; i++)
		tree_copy(&((*dst)->child[i]), src->child[i]);
}
