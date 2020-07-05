/*
 * trep.c -- trep interpreter
 * v0.0
 * 30.06.2020
 * by asz
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "proto.h"

unit root;
elm bottom, *var_stack;
var *heap;

int line = 1;

char *err_messages[] = {
	"there should be a space between the parent and the heir: parent (heir)",
	"the heir does not have a parent, reminder: fun (some-fun-arg) \"some fun arg text\""
};

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("%s: %s <file>\n", argv[0], argv[0]);
		exit(0);
	}

	heap = new_var_area(256);
	init_elm(&bottom, NULL);
	var_stack = &bottom;
	var_stack->heap = heap;

	init_unit(&root, NULL);	

	interpret(argv[1]);

	if (root.child_num) {
		crawl_tree(&root, exec);
	}

	crawl_tree(&root, del_tree);
	crawl_stack(&bottom, del_elm);
	del_var_area(heap);
	return 0;
}

void error(int err_line, int type, char *token) {
	printf("Error, line %d: `%s`: %s\n", err_line, token, err_messages[type]);
}

void interpret(char *filename) {
	FILE *src = fopen(filename, "r");
	pars(src, NULL, &root);
	fclose(src);
}

void clear_buf(char *buf, int len) {
	for (int i = 0; i < len; i++)
		if (buf[i])
			buf[i] = 0;	
}

void pars_file(FILE *fptr, unit *uptr) {
	char buf[LEN] = "\0";
	int c = 0, is_quote = 0, sticking = 0, i = 0, type = PARENT;

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
	if (!fptr && !str)
		return ;
	else if (fptr)
		pars_file(fptr, uptr);
	else if (str)
		pars_str(str, uptr);
}

int is_complex_token(char *tok) {
	int i = 0, is_quote = 0;

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

void tree_builder(char *token, int type, unit *uptr) {
	if (type == PARENT) {
		if (is_parent && !nesting) {
			//crawl_tree(uptr, show_tree);
			crawl_tree(&root, exec);
			crawl_tree(&root, del_tree);
			refresh_unit(&root);

			set_value(uptr, token);
		}
		else {
			set_value(uptr, token);
			if (!nesting)
				is_parent = 1;
		}
	}
	else {
		new_child(uptr);

		if (is_complex_token(token)) {
			nesting++;
			pars(NULL, token, uptr->child[uptr->i-1]);
			nesting--;
		}
		else {
			set_value(get_child(uptr, get_i(uptr)-1), token);	
		}
	}
}
