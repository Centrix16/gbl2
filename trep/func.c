#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "proto.h"

func *func_init_heap(int len) {
	size_t size = sizeof(func);
	func *fptr = NULL;

	fptr = (func*)calloc(len, size);
	if (!fptr) {
		perror(__func__);
		exit(0);
	}

	return fptr;
}

void func_free_only_heap(func *fptr) {
	if (fptr)
		free(fptr);
}

void func_init_stack(func_stack *fs, char *name, char *args, unit *tree) {
	func *fptr = fs->heap;
	int indx = fs->func_indx;

	fptr[indx].name = malloc(strlen(name)+1);
	fptr[indx].args = malloc(strlen(args)+1);
	fptr[indx].body = malloc(sizeof(unit));

	strcpy(fptr[indx].name, name);
	strcpy(fptr[indx].args, args);

	tree_copy(&fptr[indx].body, tree);
	fs->func_indx++;
}

void func_free(func *fptr, int indx) {
	if (fptr[indx].name) {
		free(fptr[indx].name);
		free(fptr[indx].args);

		crawl_tree(fptr[indx].body, unit_free);
	}
}

void func_free_heap(func *fptr) {
	for (int i = 0; fptr[i].name; i++) {
		func_free(fptr, i);
	}
	func_free_only_heap(fptr);
}

void func_crawl_heap(func *fptr, void (*code)(func*)) {
	for (int i = 0; fptr[i].name; i++)
		(*code)(&fptr[i]);
}

void func_show(func *fptr) {
	printf("name = `%s` args = `%s`\n", fptr->name, fptr->args);	
	crawl_tree(fptr->body, unit_show);
}

/*
int main() {
	func_stack bottom, *fs = NULL;
	unit root;

	fs_init(&bottom, NULL);
	fs = &bottom;

	fs->heap = func_init_heap(LEN);

	unit_init(&root, NULL);
	unit_new_child(&root);
	unit_new_child(&root);

	strcpy(root.child[0]->value, "1");
	strcpy(root.child[1]->value, "2");

	func_init_stack(fs, "f", "a b c", &root);
	func_crawl_heap(fs->heap, func_show);

	func_free_heap(fs->heap);
	fs_crawl_stack(fs, fs_del);
	return 0;
}*/
