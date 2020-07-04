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

int enclosure = 0;

void init_unit(unit *uptr, unit *new_parent) {
	uptr->child_num = 0;
	uptr->i = 0;
	uptr->parent = new_parent;
	uptr->is_free = 0;
	strcpy(uptr->value, "\0");

	for (int i = 0; i < CHILD_MAX; i++) {
		uptr->child[i] = NULL;
	}
}

void new_child(unit *parent) {
	int child_index = parent->i;
	unit *uptr = NULL;

	if (child_index > CHILD_MAX) {
		fprintf(stderr, "%s: too many childs\n", __func__);
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
	strcpy(parent->child[child_index]->value, "\0");
}

void set_value(unit *uptr, char *new_value) {
	sprintf(uptr->value, "%s", new_value);	
//	strcpy(uptr->value, new_value);
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

	(*func)(uptr);
	if (is_end_unit(uptr)) {
		return ;
	}

	for (int i = 0; i < uptr->child_num; i++) {
		enclosure++;	
		crawl_tree(get_child(uptr, i), func);
		enclosure--;
	}
}


void show_tree(unit *uptr) {
	if (uptr)
		printf("%*c%s\n", enclosure+1, ' ', uptr->value);
}

void del_tree(unit *uptr) {
	if (uptr->parent && uptr->is_free == 0) {
		uptr->child[0] = NULL;
		uptr->child_num = 0;
		uptr->i = 0;
		uptr->is_free = 1;
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
	strcpy(uptr->value, "\0");
}

/*int main() {
	char names[][16] = {"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};	
	unit root;

	init_unit(&root, NULL);

	for (int i = 0; i < 10; i++) {
		new_child(&root);
		set_value(get_child(&root, i), names[i]);
	}
	new_child(get_child(&root, 0));
	set_value(get_child(get_child(&root, 0), 0), "q");

	crawl_tree(&root, show_tree);
	crawl_tree(&root, del_tree);
	return 0;
}*/
