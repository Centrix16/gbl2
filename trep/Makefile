ct = gcc
flags = -O0 -g -Wall -o
src = trep.c bl.c tree.c var.c stack.c
libs = -lm
out = ti
dir = samples

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

test:
	./$(out) $(dir)/eval.trep
	./$(out) $(dir)/if.trep
	./$(out) $(dir)/input.trep
	./$(out) $(dir)/logic.trep
	./$(out) $(dir)/math.trep
	./$(out) $(dir)/output.trep
	./$(out) $(dir)/var.trep
