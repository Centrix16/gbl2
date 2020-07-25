/*
 * bl.c -- base library of trep language
 * v0.7
 * 25.07.2020
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

	result = is_built_in(uptr->value);
	if (result > -1) {
		(*built_in_funcs[result])(uptr);
		return ;
	}

	result = is_variable(uptr->value);
	if (result > -1) {
		//strcpy(uptr->value, get_var_value_stack(var_stack, result));
		unit_set_ret_value(uptr, get_var_value_stack(var_stack, result));
		return ;
	}

	result = is_user_func(uptr->value);
	if (result > -1) {
		// code for user func
		return ;
	}

	unit_set_ret_value(uptr, uptr->value);
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
		if (isdigit(*sptr) || *sptr == '.' || *sptr == '-')
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

	tmp = unit_get_child(uptr, 0)->ret_value;
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
		tmp = unit_get_child(uptr, i)->ret_value;	

		if (!strcmp(tmp, "~n"))
			fputc('\n', stream);
		else if (!strcmp(tmp, "~t"))
			fputc('\t', stream);
		else
			fputs(del_sym(tmp, '\"'), stream);	
	}
	unit_set_ret_value(uptr, unit_get_child(uptr, i-1)->value);
}

void let(unit *uptr) {
	char *name = NULL;
	char *value = NULL;
	int result = 0;
	var *vptr = NULL;

	for (int i = 0; i < uptr->child_num; i += 2) {
		name = unit_get_child(uptr, i)->value;
		value = unit_get_child(uptr, i+1)->ret_value;

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
	unit_set_ret_value(uptr, value);
}

void no_eval(unit *uptr) {
	strcpy(uptr->value, uptr->child[0]->value);	
	crawl_tree(uptr, unit_show);
}

void input(unit *uptr) {
	FILE *stream = stdin;
	char *tmp = NULL, buf[LEN] = "";

	if (uptr->child_num) {
		tmp = unit_get_child(uptr, 0)->ret_value;

		if (!strcmp(tmp, "out"))
			stream = stdout;
		else if (!strcmp(tmp, "in"))
			stream = stdin;
		else if (!strcmp(tmp, "err"))
			stream = stderr;
	}

	fgets(buf, LEN, stream);
	unit_set_ret_value(uptr, del_sym(buf, '\n'));
}

void sum(unit *uptr) {
	double result = 0;
	char result_str[LEN] = "";

	for (int i = 0; i < uptr->child_num; i++) {
		if (!is_int(unit_get_child(uptr, i)->ret_value))
			error(line, expected_int, unit_get_child(uptr, i)->ret_value);
		result += atof(unit_get_child(uptr, i)->ret_value);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void sub(unit *uptr) {
	double result = 0;
	char result_str[64] = "";

	if (!is_int(unit_get_child(uptr, 0)->ret_value))
			error(line, expected_int, unit_get_child(uptr, 0)->ret_value);
	if (uptr->child_num > 1)
		result = atof(unit_get_child(uptr, 0)->ret_value);

	for (int i = 1; i < uptr->child_num; i++) {
		if (!is_int(unit_get_child(uptr, i)->ret_value))
			error(line, expected_int, unit_get_child(uptr, i)->ret_value);
		result -= atof(unit_get_child(uptr, i)->ret_value);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void mul(unit *uptr) {
	double result = 1;
	char result_str[64] = "";

	for (int i = 0; i < uptr->child_num; i++) {
		if (!is_int(unit_get_child(uptr, i)->ret_value))
			error(line, expected_int, unit_get_child(uptr, i)->ret_value);
		result *= atof(unit_get_child(uptr, i)->ret_value);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void divop(unit *uptr) {
	double result = 0, tmp = 0;
	char result_str[64] = "";

	if (!is_int(unit_get_child(uptr, 0)->ret_value))
			error(line, expected_int, unit_get_child(uptr, 0)->ret_value);
	if (uptr->child_num > 1)
		result = atof(unit_get_child(uptr, 0)->ret_value);

	for (int i = 1; i < uptr->child_num; i++) {
		if (!is_int(unit_get_child(uptr, i)->ret_value))
			error(line, expected_int, unit_get_child(uptr, i)->ret_value);	

		tmp = atof(unit_get_child(uptr, i)->ret_value);
		if (!tmp) {
			error(line, nil_div, unit_get_child(uptr, i)->ret_value);	
			exit(0);
		}

		result /= tmp;
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void powop(unit *uptr) {
	double result = 1;
	char result_str[64] = "";

	if (uptr->child_num > 1)
		result = atof(unit_get_child(uptr, 0)->value);

	for (int i = 1; i < uptr->child_num; i++)
		result = pow(result, atof(unit_get_child(uptr, i)->value));

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void comment(unit *uptr) { ; }

void quit(unit *uptr) {
	if (uptr->child_num)
		exit(atoi(unit_get_child(uptr, 0)->ret_value));
}

void eval(unit *uptr) {
	char tmp_buf[LEN] = "";
	unit *par = NULL;

	if (uptr->child_num) {
		strcpy(tmp_buf, unit_get_child(uptr, 0)->ret_value);

		par = uptr->parent; // To avoid deleting yourself
		uptr->parent = NULL;
		crawl_tree(uptr, unit_free);
		uptr->parent = par;

		uptr->child_num = 0;

		set_is_parent(0);
		pars(NULL, tmp_buf, uptr);
		crawl_tree(uptr, exec);
	}
}

void more_or_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) >= result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) >= atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void less_or_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) <= result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) <= atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void more(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) > result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) > atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void less(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) < result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) < atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) == result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) == atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void no_equal(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) != result;

	for (int i = 0; i < uptr->child_num - 1; i++)
		if (!result && i)
			break;
		else
			result = atof(unit_get_child(uptr, i)->ret_value) != atof(unit_get_child(uptr, i+1)->ret_value);

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void notop(unit *uptr) {
	int result = 0;
	char result_str[2] = "";

	if (uptr->child_num)
		result = atof(unit_get_child(uptr, 0)->ret_value) ? 0 : 1;

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void andop(unit *uptr) {
	int result = 1;
	char result_str[2];

	for (int i = 0; i < uptr->child_num; i++) {
		result = result && atof(unit_get_child(uptr, i)->ret_value);
		if (!result)
			break;
	}

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void orop(unit *uptr) {
	int result = 0;
	char result_str[2];

	for (int i = 0; i < uptr->child_num; i++) {
		result = result || atof(unit_get_child(uptr, i)->ret_value);
		if (result)
			break;
	}

	sprintf(result_str, "%d", result);
	unit_set_ret_value(uptr, result_str);
}

void eval_expr(unit *uptr, char *buf) {
	unit *par = uptr->parent;
	uptr->parent = NULL;
	crawl_tree(uptr, unit_free);
	uptr->parent = par;

	uptr->child_num = 0;

	set_is_parent(0);
	pars(NULL, buf, uptr);
	crawl_tree(uptr, exec);
}

void branching(unit *uptr) {
	char *cond = NULL, *body = NULL;	
	unit *body_unit = NULL;
	int limit = uptr->child_num;

	if ((limit % 2) != 0)
		limit--;

	for (int i = 0; i < limit; i += 2) {
		cond = unit_get_child(uptr, i)->ret_value;
		body = unit_get_child(uptr, i+1)->value;
		body_unit = unit_get_child(uptr, i+1);

		if (strcmp(cond, "0")) {
			strcpy(body, "\0");
			crawl_tree(body_unit, exec);
			return ;
		}
	}

	if (limit < uptr->child_num) {
		body = unit_get_child(uptr, limit)->value;	
		body_unit = unit_get_child(uptr, limit);

		strcpy(body, "\0");
		crawl_tree(body_unit, exec);
	}

	unit_set_ret_value(uptr, "0");
}

void while_loop(unit *uptr) {
	crawl_tree(unit_get_child(uptr, 0), unit_show);
	crawl_tree(unit_get_child(uptr, 1), unit_show);
}

void for_loop(unit *uptr) {
	
}
