#include <stdio.h>

int f() {
	static int var = 0;
	printf("%d\n", var);
	var++;
	f();
	return 0;
}

int main() {
	f();	
	return 0;
}
