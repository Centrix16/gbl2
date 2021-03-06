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

char *built_in[] = {"output", "let", ";", "input", "~", "exit", "+", "-", "*", "/", "eval", ">=", "<=", ">", "<", "=", "!=", "!", "&", "|", "?", "while", "for", "str", "val", "def", NULL};
void (*built_in_funcs[])(unit*) = {output, let, NULL, input, comment, quit, sum, sub, mul, divop, eval, more_or_equal, less_or_equal, more, less, equal, no_equal, notop, andop, orop, branching, while_loop, for_loop, str, val, def};

extern elm *var_stack;
extern func_stack *func_top;
extern int line;

/* service functions */

int arr_len(char **arr) {
	int i = 0;

	while (arr[i]) i++;

	return i;
}

int find_s(char **arr, char *str) {
	if (!str)
		return -1;

	for (int i = 0; i < arr_len(arr); i++) {
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
	func *fptr = func_top->heap;

	for (int i = 0; i < func_top->func_indx; i++) {
		if (!strcmp(fptr[i].name, tok))
			return i;
	}

	return -1;
}

int arg_setter(elm *eptr, unit *uptr, char *arg_list) {
	char *name = NULL;
	int i = 0;

	name = strtok(arg_list, " ");
	while (name) {
		init_var_stack(eptr, name, unit_get_child(uptr, i)->ret_value);
		name = strtok(NULL, " ");	
		i++;
	}

	return i;
}

char *get_function_ret_value(unit *uptr) {
	return unit_get_child(uptr, uptr->child_num-1)->ret_value;
}

void exec(unit *uptr) {
	int result = 0, need_recover = -1;
	int args_setted = 0;
	func *fptr = NULL;

	result = is_built_in(uptr->value);
	if (result > -1) {
		need_recover = is_recoverable_unit(uptr->value);

		(*built_in_funcs[result])(uptr);

		if (need_recover == -1)
			crawl_tree(uptr, recover);
		return ;
	}

	result = is_variable(uptr->value);
	if (result > -1) {
		unit_set_ret_value(uptr, get_var_value_stack(var_stack, result));
		return ;
	}

	result = is_user_func(uptr->value);
	if (result > -1) {
		fptr = &func_top->heap[result];
		
		/* creating a new viewport */
		add_next(var_stack);
		var_stack = var_stack->next;
		var_stack->heap = new_var_area(LEN);

		/* part of argument processing */
		args_setted = arg_setter(var_stack, uptr, fptr->args);
		if (args_setted != uptr->child_num)
			error(line, arg_error, uptr->value);

		/* part of a function call */
		strcpy(fptr->body->value, " ");
		crawl_tree(fptr->body, exec);
		strcpy(fptr->body->value, ";");

		/* deleting the new viewport */
		var_stack = var_stack->prev;
		del_var_area(var_stack->next->heap);
		del_elm(var_stack->next);

		/* returning the function value */
		unit_set_ret_value(uptr, get_function_ret_value(fptr->body));

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

int is_recoverable_unit(char *val) {
	char *list[] = {"val", NULL};

	if (!val)
		return -1;

	return find_s(list, val);
}

void recover(unit *uptr) {
	if (is_recoverable_unit(uptr->ret_value) > -1)
		strcpy(uptr->value, uptr->ret_value);
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
		else {
			if (!strcmp(tmp, "~n"))
				fputc('\n', stream);
			else if (!strcmp(tmp, "~t"))
				fputc('\t', stream);
			else
				fputs(tmp, stream);
		}

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
	int limit = uptr->child_num;

	if ((limit % 2) != 0)
		limit--;	

	for (int i = 0; i < limit; i += 2) {
		name = unit_get_child(uptr, i)->value;
		value = unit_get_child(uptr, i+1)->ret_value;

		if (is_str(value))
			value = del_sym(value, '\"');

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
	char result_str[LEN] = "";

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

void comment(unit *uptr) { ; }

void quit(unit *uptr) {
	if (uptr->child_num)
		exit(atoi(unit_get_child(uptr, 0)->ret_value));
}

void eval(unit *uptr) {
	unit *eval_unit = NULL;
	char *code = NULL;

	if (uptr->child_num) {
		unit_new_child(uptr);	

		eval_unit = unit_get_child(uptr, uptr->child_num-1);
		code = unit_get_child(uptr, 0)->ret_value;

		set_is_parent(0);
		eval_unit->parent = NULL;

		pars(NULL, code, eval_unit);
		crawl_tree(eval_unit, exec);
		crawl_tree(eval_unit, unit_free);

		uptr->child_num--;

		unit_set_ret_value(uptr, eval_unit->ret_value);
	}
}

void more_or_equal(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;	
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) >= result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) >= atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void less_or_equal(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) <= result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) <= atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void more(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) > result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) > atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void less(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) < result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) < atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void equal(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) == result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) == atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void no_equal(unit *uptr) {
	char *op1 = NULL, *op2 = NULL;
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num == 1)
		result = atof(unit_get_child(uptr, 0)->ret_value) != result;

	for (int i = 0; i < uptr->child_num - 1; i++) {
		op1 = unit_get_child(uptr, i)->ret_value;
		op2 = unit_get_child(uptr, i+1)->ret_value;

		if (!result && i)
			break;
		else
			result = atof(op1) != atof(op2);
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void notop(unit *uptr) {
	char result_str[2] = "";
	double result = 0;

	if (uptr->child_num)
		result = atof(unit_get_child(uptr, 0)->ret_value) ? 0 : 1;

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void andop(unit *uptr) {
	char result_str[2];
	double result = 1;

	for (int i = 0; i < uptr->child_num; i++) {
		result = result && atof(unit_get_child(uptr, i)->ret_value);
		 
		if (!result)
			break;
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
}

void orop(unit *uptr) {
	char result_str[2];
	double result = 0;

	for (int i = 0; i < uptr->child_num; i++) {
		result = result || atof(unit_get_child(uptr, i)->ret_value);

		if (result)
			break;
	}

	sprintf(result_str, "%g", result);
	unit_set_ret_value(uptr, result_str);
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
			unit_set_ret_value(uptr, cond);
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
	unit *cond_unit = NULL, *body_unit = NULL;
	char *cond = NULL, *body = NULL,
		 i_str[LEN] = "";
	int i = 0;

	if (uptr->child_num == 2) {
		cond_unit = unit_get_child(uptr, 0);	
		body_unit = unit_get_child(uptr, 1);	

		cond = cond_unit->ret_value;	
		body = body_unit->value;

		if (strcmp(cond, "0")) {
			if (!strcmp(body, ";"))
				strcpy(body, "\0");	

			crawl_tree(body_unit, exec);
			crawl_tree(cond_unit, exec);
			i++;
			while_loop(uptr);

			strcpy(body, ";");
		}
	}

	sprintf(i_str, "%d", i);
	unit_set_ret_value(uptr, i_str);
}

void for_loop(unit *uptr) {
	unit *cond_unit = NULL, *body_unit = NULL, *modf_unit = NULL;
	char *cond = NULL, *body = NULL, *modf = NULL,
		 i_str[LEN] = "";
	int i = 0;

	if (uptr->child_num == 4) {
		cond_unit = unit_get_child(uptr, 1);
		modf_unit = unit_get_child(uptr, 2);
		body_unit = unit_get_child(uptr, 3);

		cond = cond_unit->ret_value;
		body = body_unit->value;
		modf = modf_unit->value;

		if (strcmp(cond, "0")) {
			if (!strcmp(body, ";"))
				strcpy(body, "\0");
			if (!strcmp(modf, ";"))
				strcpy(modf, "\0");

			crawl_tree(body_unit, exec);
			crawl_tree(modf_unit, exec);
			crawl_tree(cond_unit, exec);
			i++;
			for_loop(uptr);

			strcpy(body, ";");
		}
	}

	sprintf(i_str, "%d", i);
	unit_set_ret_value(uptr, i_str);
}

void str(unit *uptr) {
	char *op = NULL, *count = NULL, *variable = NULL,
		 *var_value = NULL, *tmp_ptr = NULL, *c = NULL;
	int indx = 0, len = 0;
	var *dst = NULL;
	char ret[LEN] = "";
	
	if (uptr->child_num >= 3) {
		op = unit_get_child(uptr, 0)->ret_value;
		count = unit_get_child(uptr, 1)->ret_value;
		variable = unit_get_child(uptr, 2)->value;

		if (is_variable(variable) < 0) {
			error(line, not_such_var, variable);
			exit(0);
		}
		else
			dst = get_var_stack(var_stack, is_variable(variable));

		indx = atoi(count);
		if (indx < 0) {
			error(line, incorrect_data, count);
			exit(0);
		}

		var_value = dst->value;
		len = strlen(var_value);

		if (!strcmp("set", op)) {
			if (len < indx) {
				tmp_ptr = (char*)realloc(var_value, indx+1);
				if (tmp_ptr) {
					perror(__func__);
					exit(0);
				}

				dst->value = tmp_ptr;
			}

			c = unit_get_child(uptr, 3)->ret_value;
			if (is_str(c))
				c = del_sym(c, '\"');
			var_value[indx] = *(c);

			strcpy(ret, var_value);
		}
		else if (!strcmp("get", op)) {
			tmp_ptr = var_value;

			if (indx > len) {
				error(line, incorrect_data, count);
				exit(0);
			}
			else if (indx == len) {
				tmp_ptr = "0";
				indx = 0;
			}

			ret[0] = tmp_ptr[indx];
			ret[1] = 0;
		}
		else {
			error(line, unk_param, op);
			exit(0);
		}

		unit_set_ret_value(uptr, ret);
	}
}

void val(unit *uptr) {
	char buf[LEN] = "";
	unit *child = NULL;

	if (uptr->child_num > 1) {
		child = unit_get_child(uptr, 0);

		strcpy(buf, child->ret_value);

		for (int i = 1; i < uptr->child_num; i++) {
			child = unit_get_child(uptr, i);	
			strcat(buf, child->ret_value);
		}
	}

	strcpy(uptr->value, buf);
	unit_set_ret_value(uptr, "val");
}

void def(unit *uptr) {
	unit *name_unit = NULL, *arg_list_unit = NULL, *body_unit = NULL;

	name_unit = unit_get_child(uptr, 0);
	arg_list_unit = unit_get_child(uptr, 1);
	body_unit = unit_get_child(uptr, 2);

	func_init_stack(func_top, name_unit->value, arg_list_unit->value, body_unit);
	unit_set_ret_value(uptr, name_unit->value);
}
