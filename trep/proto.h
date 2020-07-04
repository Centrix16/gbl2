/*
 * proto.h -- main header file
 * v0.0
 * 30.06.2020
 * by asz
 */

#ifndef __TREP_INTERPRETER_FUNCTION_PROTOTYPE_FILE_H_INCLUDED__
#define __TREP_INTERPRETER_FUNCTION_PROTOTYPE_FILE_H_INCLUDED__

#include <stdio.h>

#define LEN 256
#define CHILD_MAX 256

#define PARENT 0
#define CHILD 1

typedef struct __unit__ {
	char value[256];
	int child_num;
	int i;
	int is_free;
	struct __unit__ *parent;
	struct __unit__ *child[CHILD_MAX];
} unit;

/* trep.c */
void error(int, int, char*);
void interpret(char *filename);
void pars(FILE *, char *, unit *);
void tree_builder(char *, int, unit *);

/* tree.c */
void init_unit(unit *, unit *);
void new_child(unit *);
void set_value(unit *dst_unit, char *new_value);
unit *get_child(unit *parent, int child_index);
void set_end(unit *uptr);
int is_end_unit(unit *uptr);
void crawl_tree(unit *parent, void (*func)(unit*));
void show_tree(unit *uptr);
void del_tree(unit *uptr);
int get_i(unit *dst);
void refresh_unit(unit *uptr);

/* bl.c */
void exec(unit *uptr);
void output(unit *uptr);

enum error_types {
	space_between_parent_and_child = 0,
	not_parent,
};

#endif
