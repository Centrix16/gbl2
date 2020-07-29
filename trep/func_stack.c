#include <stdio.h>
#include <stdlib.h>

#include "proto.h"

void fs_init(func_stack *fptr, func_stack *bottom) {
	fptr->prev = bottom;
	fptr->next = NULL;
	fptr->heap = NULL;
	fptr->func_indx = 0;
}

void fs_add(func_stack *fptr) {
	func_stack *new = calloc(1, sizeof(func));

	if (!new) {
		perror(__func__);
		exit(0);
	}

	fptr->next = new;
	fs_init(fptr->next, fptr);
}

void fs_del(func_stack *fptr) {
	if (fptr->prev) {
		fptr->prev->next = NULL;	
		free(fptr);
	}
}

void fs_crawl_stack(func_stack *fptr, void (*code)(func_stack*)) {
	if (!fptr->next) {
		(*code)(fptr);
		return ;
	}

	fs_crawl_stack(fptr->next, code);
	(*code)(fptr);
}
