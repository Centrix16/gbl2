/*
 * proto.h -- main header file
 * 12.07.2020
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
	char value[LEN];
	int eval_me;
	int child_num;
	int i;
	int is_free;
	struct __unit__ *parent;
	struct __unit__ *child[CHILD_MAX];
} unit;

typedef struct __var__ {
	char *name;
	char *value;
} var;

typedef struct __elm__ {
	var *heap;
	char *tag;
	int var_indx;
	struct __elm__ *prev;
	struct __elm__ *next;
} elm;

/* trep.c */
void error(int, int, char*);
void interpret(char *filename);
void pars(FILE *, char *, unit *);
void tree_builder(char *, int, unit *);
void set_is_parent(int new);

/* tree.c */
void init_unit(unit *, unit *);
void new_child(unit *);
void set_value(unit *, char *);
unit *get_child(unit *, int);
void set_end(unit *);
int is_end_unit(unit *);
void crawl_tree(unit *, void (*)(unit*));
void show_tree(unit *);
void del_tree(unit *);
int get_i(unit *);
void refresh_unit(unit *);

/* bl.c */
void exec(unit *);
void output(unit *),
	 let(unit *),
	 no_eval(unit *),
	 input(unit *),
	 sum(unit *),
	 sub(unit *),
	 mul(unit *),
	 divop(unit *),
	 powop(unit *),
	 comment(unit *),
	 eval(unit *),
	 quit(unit *),
	 more_or_equal(unit *),
	 less_or_equal(unit *),
	 more(unit *),
	 less(unit *),
	 equal(unit *),
	 no_equal(unit *),
	 notop(unit *),
	 andop(unit *),
	 orop(unit *),
	 branching(unit *),
	 while_loop(unit *);

/* var.c */
var *new_var_area(int);
void init_var(var *, int, char *, char *);
void init_var_stack(elm *stack, char *name, char *val);
void del_var_area(var *);
void crawl_heap(var *, void (*)(var *));
void show_var(var *);
char *get_var_value_stack(elm *stack, int indx);
var *get_var_stack(elm *stack, int indx);

/* stack.c */
void init_elm(elm *, elm *);
void add_next(elm *);
void del_elm(elm *);
void crawl_stack(elm *, void (*)(elm*));
void show_elm(elm *);

enum error_types {
	space_between_parent_and_child = 0,
	not_parent,
	expected_int,
	nil_div
};

#endif
