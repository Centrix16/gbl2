ct = gcc
flags = -O0 -g -o
src = trep.c bl.c tree.c var.c stack.c
libs = -lm
out = ti

all:
	$(ct) $(flags) $(out) $(src) $(libs)

tree:
	$(ct) $(flags) t tree.c

var:
	$(ct) $(flags) v var.c

stack:
	$(ct) $(flags) s stack.c

r:
	rm $(out)
	$(ct) $(flags) $(out) $(src)
