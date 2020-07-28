/*
 * trep.c -- trep interpreter
 * v0.2
 * 25.07.2020
 * by asz
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "proto.h"
#include "unilog.h"

#define DBG 0

unit root;
elm bottom, *var_stack;
var *heap;

int line = 1;
extern int memory;

char *err_messages[] = {
	"there should be a space between the parent and the heir: parent (heir)",
	"the heir does not have a parent, reminder: fun (some-fun-arg) \"some fun arg text\"",
	"expected int",
	"division by zero detected",
	"not such a variable",
	"unknown parameter",
	"incorrect data"
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s: %s <file>\n", argv[0], argv[0]);
		exit(0);
	}

	log_mode("w");
	target("log");

	heap = new_var_area(256);
	init_elm(&bottom, NULL);
	var_stack = &bottom;
	var_stack->heap = heap;

	unit_init(&root, NULL);	

	interpret(argv[1]);

	if (root.child_num) {
		crawl_tree(&root, exec);
	}

	free_tree(&root);
	crawl_stack(&bottom, del_elm);
	del_var_area(heap);

	end();

#if DBG
	printf("\n\n\tmemory = %d\n", memory);
#endif

	return 0;
}

void error(int err_line, int type, char *token) {
	log("%s: err_line = %d, type = %d, token = `%s`\n", __func__, err_line, type, token);	

	fprintf(stderr, "\nError, line %d: `%s`: %s\n", err_line, token, err_messages[type]);
}

void interpret(char *filename) {
	FILE *src = fopen(filename, "r");

	log("%s: filename = %s\n", __func__, filename);

	pars(src, NULL, &root);
	fclose(src);
}

void clear_buf(char *buf, int len) {
	log("%s: buf = `%s`, len = %d\n", __func__, buf, len);

	for (int i = 0; i < len; i++)
		if (buf[i])
			buf[i] = 0;	
}

void pars_file(FILE *fptr, unit *uptr) {
	char buf[LEN] = "\0";
	int c = 0, is_quote = 0, sticking = 0, i = 0, type = PARENT;

	log("%s: fptr = %p, uptr = %p\n", __func__, fptr, uptr);

	if (feof(fptr))
		return ;	

	c = fgetc(fptr);
	while (!feof(fptr)) {
		switch (c) {
			case '(':
				if (is_quote) {
					buf[i++] = c;
					break;
				}

				if (type == PARENT && buf[i-1] && i) {
					error(line, space_between_parent_and_child, buf);
					exit(0);
				}

				sticking++;
				type = CHILD;

				if (sticking > 1)
					buf[i++] = c;
			break;

			case ')':
				if (is_quote) {
					buf[i++] = c;
					break;
				}

				sticking--;

				if (!sticking) {
					buf[i] = '\0';	
					tree_builder(buf, type, uptr);
					i = 0;
					type = PARENT;
					
					clear_buf(buf, LEN);
				}
				else
					buf[i++] = c;
			break;

			case '\"':
				if (type == PARENT && buf[i-1] && i) {
					error(line, space_between_parent_and_child, buf);
					exit(0);
				}

				is_quote = !is_quote;
				buf[i++] = '\"';

				if (!is_quote && !sticking) {
					buf[i] = '\0';
					tree_builder(buf, type, uptr);
					i = 0;
					
					clear_buf(buf, LEN);
				}
				if (!sticking)
					type = is_quote;
			break;

			case ' ':
				if (is_quote || sticking)
					buf[i++] = ' ';	
				else if (i) {
					buf[i] = '\0';
					tree_builder(buf, type, uptr);
					i = 0;

					clear_buf(buf, LEN);
				}
			break;

			case '\n':
				line++;

				if (is_quote)
					buf[i++] = c;	
			break;

			default:
				if (isspace(c)) {
					if (is_quote)
						buf[i++] = c;	
				}
				else
					buf[i++] = c;
			break;
		}

		c = fgetc(fptr);
	}
}

void pars_str(char *str, unit *uptr) {
	char buf[LEN] = "\0";
	int c = 0, is_quote = 0, sticking = 0, i = 0, j = 0, type = PARENT;

	log("%s: str = `%s`, uptr = %p\n", __func__, str, uptr);

	if (!*str)
		return ;

	c = str[j++];
	while (str[j-1]) {
		switch (c) {
			case '(':
				if (is_quote) {
					buf[i++] = c;
					break;
				}

				if (type == PARENT && buf[i-1] && i) {
					error(line, space_between_parent_and_child, buf);
					exit(0);
				}

				sticking++;
				type = CHILD;

				if (sticking > 1)
					buf[i++] = c;
			break;

			case ')':
				if (is_quote) {
					buf[i++] = c;
					break;
				}

				sticking--;

				if (!sticking) {
					buf[i] = '\0';
					tree_builder(buf, type, uptr);
					i = 0;
					type = PARENT;

					clear_buf(buf, LEN);
				}
				else
					buf[i++] = c;	
			break;

			case '\"':
				if (type == PARENT && buf[i-1] && i) {
					error(line, space_between_parent_and_child, buf);
					exit(0);
				}

				is_quote = !is_quote;
				buf[i++] = '\"';

				if (!is_quote && !sticking) {
					buf[i] = '\0';
					tree_builder(buf, type, uptr);
					i = 0;

					clear_buf(buf, LEN);
				}
				if (!sticking)
					type = is_quote;
			break;

			case ' ':
				if (is_quote || sticking)
					buf[i++] = ' ';	
				else if (i) {
					buf[i] = '\0';
					tree_builder(buf, type, uptr);
					i = 0;

					clear_buf(buf, LEN);
				}
			break;

			case '\n':
				line++;

				if (is_quote)
					buf[i++] = c;	
			break;

			default:
				buf[i++] = c;
			break;
		}

		c = str[j++];
	}	
}

void pars(FILE *fptr, char *str, unit *uptr) {
	log("%s: fptr = %p, str = `%s`, uptr = %p\n", __func__, fptr, str, uptr);	

	if (!fptr && !str)
		return ;
	else if (fptr)
		pars_file(fptr, uptr);
	else if (str)
		pars_str(str, uptr);
}

int is_complex_token(char *tok) {
	int i = 0, is_quote = 0;

	log("%s: tok = `%s`\n", __func__, tok);

	while (tok[i]) {
		if (tok[i] == '\"')
			is_quote = !is_quote;
		else if ((tok[i] == '(' || tok[i] == ')') && !is_quote)
			return 1;
		i++;
	}
	return 0;
}

int nesting = 0, is_parent = 0;

void set_is_parent(int new) {
	log("%s: new = %d\n", __func__, new);	

	is_parent = new;
}

void tree_builder(char *token, int type, unit *uptr) {
	log("%s: token = `%s`, type = %d, uptr = %p\n", __func__, token, type, uptr);	
	if (type == PARENT) {
		if (is_parent && !nesting) {
			crawl_tree(uptr, exec);
			free_tree(uptr);
			unit_init(uptr, uptr);

			unit_set_value(uptr, token);
		}
		else {
			unit_set_value(uptr, token);
			if (!nesting)
				is_parent = 1;
		}
	}
	else {
		unit_new_child(uptr);

		if (is_complex_token(token)) {
			nesting++;
			pars(NULL, token, uptr->child[uptr->child_num-1]);
			nesting--;
		}
		else {
			unit_set_value(unit_get_child(uptr, unit_get_i(uptr)-1), token);	
		}
	}
}
