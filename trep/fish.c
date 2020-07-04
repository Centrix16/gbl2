#include <stdio.h>
#include <ctype.h>

typedef struct _a {
	struct _a arr[4];
} a;

int main() {
	a test;	
	printf("%d\n", sizeof(test));	
	return 0;
}
