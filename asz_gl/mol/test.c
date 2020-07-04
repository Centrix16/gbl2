#include <stdio.h>

void f(char**);

int main(void) {
	char *s = "q";	
	f(&s);
	printf("%s", s);	
	return 0;
}

void f(char **str) {
	*str = "hi!";	
}
