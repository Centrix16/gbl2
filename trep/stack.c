#include <stdio.h>
#include <stdlib.h>

#include "proto.h"

void init_elm(elm *eptr, elm *bottom) {
	eptr->prev = bottom;
	eptr->next = NULL;
	eptr->heap = NULL;
	eptr->var_indx = 0;
}

void add_next(elm *eptr) {
	elm *new = calloc(1, sizeof(elm));

	if (!new) {
		perror(__func__);
		exit(0);
	}
	eptr->next = new;
	init_elm(eptr->next, eptr);
}

void del_elm(elm *eptr) {
	if (eptr->prev) {
		eptr->prev->next = NULL;	
		free(eptr);
	}
}

void crawl_stack(elm *eptr, void (*func)(elm*)) {
	if (!eptr->next) {
		(*func)(eptr);	
		return ;
	}
	crawl_stack(eptr->next, func);
	(*func)(eptr);
}

void show_elm(elm *eptr) {
	puts(eptr->tag);	
}

/*int main() {
	elm bottom;

	init_elm(&bottom, NULL);
	bottom.tag = "0";

	add_next(&bottom);
	bottom.next->tag = "1";

	add_next(bottom.next);
	bottom.next->next->tag = "2";

	crawl_stack(&bottom, show_elm);
	crawl_stack(&bottom, del_elm);
	return 0;
}*/
