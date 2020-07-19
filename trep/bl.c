/*
 * bl.c -- base library of trep language
 * v0.6
 * 12.07.2020
 * by asz
 */

#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "proto.h"

char *built_in[] = {"output", "let", ";", "input", "~", "exit", "+", "-", "*", "/", "eval", ">=", "<=", ">", "<", "=", "!=", "!", "&", "|", "?", "while", "for"};
void (*built_in_funcs[])(unit*) = {output, let, no_eval, input, comment, quit, sum, sub, mul, divop, eval, more_or_equal, less_or_equal, more, less, equal, no_equal, notop, andop, orop, branching, while_loop, for_loop};

extern elm *var_stack;
extern int line;

/* service functions */

int find_s(char **arr, char *str) {
	for (int i = 0; i < sizeof(built_in) / sizeof(char*); i++) {
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

	for (int i = 0; i < var_stack->var_indx; i++) {
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

int is_int(char *str) {
	char *sptr = str;	

	while (*sptr) {
		if (isdigit(*sptr) || *sptr == '.')
			;
		else
			return 0;
		sptr++;
	}
	return 1;
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
	strcpy(uptr->value, get_child(uptr, i-1)->value);
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
			vptr->value = malloc((strlen(value)+1) * sizeof(char));
			strcpy(vptr->value, value);
		}
		else
			init_var_stack(var_stack, name, value);
	}
	strcpy(uptr->value, value);
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

void sum(unit *uptr) {
	double result = 0;
	char result_str[64] = "";

	for (int i = 0; i < uptr->child_num; i++) {
		if (!is_int(get_child(uptr, i)->value))
			error(line, expected_int, get_child(uptr, i)->value);
		result += atof(get_child(uptr, i)->value);
	}

	sprintf(result_str, "%g", result);
	strcpy(uptr->value, result_str);
}

void sub(unit *uptr) {
	double result = 0;
	char result_str[64] = "";

	if (!is_int(get_child(uptr, 0)->value))
			error(line, expected_int, get_child(uptr, 0)->value);
	if (uptr->child_num > 1)
		result = atof(get_child(uptr, 0)->value);

	for (int i = 1; i < uptr->child_num; i++) {
		if (!is_int(get_child(uptr, i)->value))
			error(line, expected_int, get_child(uptr, i)->value);
		result -= atof(get_child(uptr, i)->value);
	}

	sprintf(result_str, "%g", result);
	strcpy(uptr->value, result_str);
}

void mul(unit *uptr) {
	double result = 1;
	char result_str[64] = "";

	for (int i = 0; i < uptr->child_num; i++) {
		if (!is_int(get_child(uptr, i)->value))
			error(line, expected_int, get_child(uptr, i)->value);
		result *= atof(get_child(uptr, i)->value);
	}

	sprintf(result_str, "%g", result);
	strcpy(uptr->value, result_str);
}

void divop(unit *uptr) {
	double result = 0, tmp = 0;
	char result_str[64] = "";

	if (!is_int(get_child(uptr, 0)->value))
			error(line, expected_int, get_child(uptr, 0)->value);
	if (uptr->child_num > 1)
		result = atof(get_child(uptr, 0)->value);

	for (int i = 1; i < uptr->child_num; i++) {
		if (!is_int(get_child(uptr, i)->value))
			error(line, expected_int, get_child(uptr, i)->value);	

		tmp = atof(get_child(uptr, i)->value);
		if (!tmp) {
			error(line, nil_div, get_child(uptr, i)->value);	
			exit(0);
		}

		result /= tmp;
	}

	sprintf(result_str, "%g", result);
	strcpy(uptr->value, result_str);
}

void powop(unit *uptr) {
	double result = 1;
	char result_str[64] = "";

	if (uptr->child_num > 1)
		result = atof(get_child(uptr, 0)->value);

	for (int i = 1; i < uptr->child_num; i++)
		result = pow(result, atof(get_child(uptr, i)->value));

	sprintf(result_str, "%g", result);
	strcpy(uptr->value, result_str);
}

void comment(unit *uptr) { ; }

void quit(unit *uptr) {
	if (uptr->child_num)
		exit(atoi(get_child(uptr, 0)->value));
}

void eval(unit *uptr) {
	char tmp_buf[256] = "";
	unit *par = NULL;

	if (uptr->child_num) {
		strcpy(tmp_buf, get_child(uptr, 0)->value);

		par = uptr->parent; // To avoid deleting yourself
		uptr->parent = NULL;
		crawl_tree(uptr, del_tree);
		uptr->parent = par;

		uptr->child_num = 0;
		uptr->eval_me = 1;

		set_is_parent(0);
		pars(NULL, tmp_buf, uptr);
		crawl_tree(uptr, exec);
	}
}

void more_or_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) >= result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) >= atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void less_or_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) <= result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) <= atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void more(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) > result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) > atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void less(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) < result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) < atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) == result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) == atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void no_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(get_child(uptr, 0)->value) != result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(get_child(uptr, i)->value) != atof(get_child(uptr, i+1)->value);

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void notop(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num)
		result = atof(get_child(uptr, 0)->value) ? 0 : 1;

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void andop(unit *uptr) {
	int result = 1;
	char result_str[2];

	for (int i = 0; i < uptr->child_num; i++) {
		result = result && atof(get_child(uptr, i)->value);
		if (!result)
			break;
	}

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void orop(unit *uptr) {
	int result = 0;
	char result_str[2];

	for (int i = 0; i < uptr->child_num; i++) {
		result = result || atof(get_child(uptr, i)->value);
		if (result)
			break;
	}

	sprintf(result_str, "%d", result);
	strcpy(uptr->value, result_str);
}

void eval_expr(unit *uptr, char *buf) {
	unit *par = uptr->parent;
	uptr->parent = NULL;
	crawl_tree(uptr, del_tree);
	uptr->parent = par;

	uptr->child_num = 0;
	uptr->eval_me = 1;

	set_is_parent(0);
	pars(NULL, buf, uptr);
	crawl_tree(uptr, exec);
}

void branching(unit *uptr) {
	char *cond = NULL, *body = NULL;
	char tmp_buf[256] = "";
	int limit = uptr->child_num;

	if ((limit % 2) != 0)
		limit--;

	for (int i = 0; i < limit; i += 2) {
		cond = get_child(uptr, i)->value;
		body = get_child(uptr, i+1)->value;

		if (!strcmp(cond, "1")) {
			strcpy(tmp_buf, body);
			eval_expr(uptr, tmp_buf);

			return ;
		}
	}

	if (limit < uptr->child_num) {
		strcpy(tmp_buf, get_child(uptr, uptr->child_num-1)->value);
		eval_expr(uptr, tmp_buf);
	}
}

void while_loop(unit *uptr) {
	char cond[256] = "", body[256] = "";	
	unit *par = NULL, *work_unit = NULL;

	if (uptr->child_num == 2) {
		strcpy(cond, get_child(uptr, 0)->value);
		strcpy(body, get_child(uptr, 1)->value);

		par = uptr->parent;
		uptr->parent = NULL;
		crawl_tree(uptr, del_tree);
		uptr->parent = par;

		init_unit(uptr, uptr);
		new_child(uptr);
		work_unit = get_child(uptr, 0);

		eval_expr(work_unit, cond);
		while (strcmp(work_unit->value, "0")) {
			eval_expr(work_unit, body);
			eval_expr(work_unit, cond);
			
			printf("cond: %s\n", work_unit->value);
		}
	}
}

void for_loop(unit *uptr) {
	char *cond = NULL, *modif = NULL, *body = NULL;
	unit cond_tree, modif_tree, body_tree;

	if (uptr->child_num != 4)
		return;

	cond = get_child(uptr, 1)->value;
	modif = get_child(uptr, 2)->value;
	body = get_child(uptr, 3)->value;

	init_unit(&cond_tree, NULL); set_value(&cond_tree, cond);
	init_unit(&modif_tree, NULL); set_value(&modif_tree, modif);
	init_unit(&body_tree, NULL); set_value(&body_tree, body);

	eval_expr(&cond_tree, cond);

	while (strcmp(cond_tree.value, "0")) {
		eval_expr(&body_tree, body);
		crawl_tree(&body_tree, show_tree);
		eval_expr(&modif_tree, modif);
		crawl_tree(&body_tree, show_tree);

		crawl_tree(&cond_tree, del_tree);
		init_unit(&cond_tree, NULL); set_value(&cond_tree, cond);
		crawl_tree(&modif_tree, del_tree);
		init_unit(&modif_tree, NULL); set_value(&modif_tree, modif);
		crawl_tree(&body_tree, del_tree);
		init_unit(&body_tree, NULL); set_value(&body_tree, body);

		eval_expr(&cond_tree, cond);
	}
}
