/*
 * proto.h -- main header file
 * 27.07.2020
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
	char *ret_value;
	int child_num;
	struct __unit__ *parent;
	struct __unit__ *child[CHILD_MAX];
} unit;

typedef struct __var__ {
	char *name;
	char *value;
} var;

typedef struct __elm__ {
	var *heap;
	int var_indx;
	struct __elm__ *prev;
	struct __elm__ *next;
} elm;

typedef struct __function__ {
	char *name;
	char *args;
	unit *body;
} func;

typedef struct __func_stack__ {
	func *heap;
	int func_indx;
	struct __func_stack__ *prev;
	struct __func_stack__ *next;
} func_stack;

/* trep.c */
void error(int, int, char*);
void interpret(char *filename);
void pars(FILE *, char *, unit *);
void tree_builder(char *, int, unit *);
void set_is_parent(int new);

/* tree.c */
void unit_init(unit *, unit *);
void unit_new_child(unit *);
void unit_set_value(unit *, char *);
unit *unit_get_child(unit *, int);
int unit_is_end(unit *);
void crawl_tree(unit *, void (*)(unit*));
void unit_show(unit *);
void unit_free(unit *);
int unit_get_i(unit *);
void unit_set_ret_value(unit *, char *);
void free_tree(unit *);
void print_tree(unit *);
void unit_copy(unit *, unit *);
void tree_copy(unit **, unit *);

/* bl.c */
/* service */
void exec(unit *);
int is_recoverable_unit(char *);
void recover(unit *);

/* lang */
void output(unit *),
	 let(unit *),
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
	 while_loop(unit *),
	 for_loop(unit *),
	 str(unit *),
	 val(unit *),
	 def(unit *);

/* var.c */
var *new_var_area(int);
void init_var(var *, int, char *, char *);
void init_var_stack(elm *stack, char *name, char *val);
void del_var_area(var *);
void crawl_heap(var *, void (*)(var *));
void show_var(var *);
char *get_var_value_stack(elm *stack, int indx);
var *get_var_stack(elm *stack, int indx);

/* var_stack.c */
void init_elm(elm *, elm *);
void add_next(elm *);
void del_elm(elm *);
void crawl_stack(elm *, void (*)(elm*));

/* func.c */
func *func_init_heap(int);
void func_init_stack(func_stack *, char *, char *, unit *);
void func_free_heap(func *);
void func_crawl_heap(func *, void (*)(func*));
void func_show(func *);

/* func_stack.c */
void fs_init(func_stack *, func_stack *);
void fs_add(func_stack *);
void fs_del(func_stack *);
void fs_crawl_stack(func_stack *, void (*)(func_stack*));

enum error_types {
	space_between_parent_and_child = 0,
	not_parent,
	expected_int,
	nil_div,
	not_such_var,
	unk_param,
	incorrect_data,
	arg_error
};

#endif
