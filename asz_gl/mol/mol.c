#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define END_EXPR "__$end$__"
#define TOKS 2048
#define TOK_LEN 256
#define SPECIAL_CHARS "()\n"

int is_special_char(int);
void check(void*);
void check_overflow(int, int);
int is_special_char(int);
void run_spec(int code);
void read(char *name);
void print_toks();

void open_brace(),
	 close_brace(),
	 new_line(),
	 space();

char buffer[TOKS][TOK_LEN];
int tok_i = 0, i = 0, in_form = 0;

int main(void) {
	read("code");	
	return 0;
}

int fgetc_s(FILE *flow) {
	int ret = 0;

	if (feof(flow))
		return -1;
	ret = fgetc(flow);
	return ret;
}

void check(void *ptr) {
	if (!ptr) {
		perror("ami");
		exit(0);
	}
}

void check_overflow(int max, int i) {
	if (i == max)
		exit(0);
}

int is_special_char(int c) {
	char specs[] = "()\n ";	
	int ret = 0;

	for (; ret < strlen(specs); ret++)
		if (specs[ret] == c) return ret;	
	return -1;
}

void write_buf(int c) {
	if (in_form) {
		buffer[tok_i][i++] = c;
	}
	else {
		if (isspace(c))
			return ;
		else
			buffer[tok_i][i++] = c;	
	}

	check_overflow(TOKS, tok_i);
	check_overflow(TOK_LEN, i);
}

void read(char *name) {
	FILE *src = fopen(name, "r");
	int c = 0, is_spec = 0;

	check((void*)src);

	c = fgetc_s(src);
	while (!feof(src)) {
		is_spec = is_special_char(c);	

		if (is_spec > -1)
			run_spec(is_spec);
		else
			write_buf(c);	
		c = fgetc_s(src);
	}
}

void run_spec(int code) {
	void (*fun[])() = {open_brace, close_brace, new_line, space};
	int len = sizeof(fun) / sizeof(void (*)());

	if (code < len)
		(*fun[code])();	
}

void open_brace() {
	in_form++;

	if (in_form > 1) {
		tok_i++;
		i = 0;
		write_buf('(');
	}
	else {
		tok_i = 0;
		i = 0;
	}
}

void close_brace() {
	in_form--;	

	if (!in_form) {	
		//buffer[tok_i+1] = END_EXPR;
		strcpy(buffer[tok_i+1], END_EXPR);
		i = 0;
		print_toks(buffer[tok_i]);
	}
	else
		write_buf(')');
}

void new_line() {
	puts("new line");
}

void space() {
	if (in_form == 1) {
		//puts(buffer[0]);	
		tok_i++;
		i = 0;
	}
	else
		write_buf(' ');	
}

void print_toks() {
	for (int j = 0; strcmp(buffer[j], END_EXPR); j++)
		puts(buffer[j]);
}
