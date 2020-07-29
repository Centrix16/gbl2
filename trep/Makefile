ct = gcc # compile tool
dbg_flags = -O0 -g -Wall -o
flags = -Wall -o
src = trep.c bl.c tree.c var.c func.c var_stack.c func_stack.c
libs = -lm
out = ti
dir = samples

all:
	$(ct) $(dbg_flags) $(out) $(src) $(libs)

tree:
	$(ct) $(dbg_flags) t tree.c

var:
	$(ct) $(dbg_flags) v var.c

stack:
	$(ct) $(dbg_flags) s stack.c

func:
	$(ct) $(dbg_flags) f func.c func_stack.c tree.c

r:
	rm $(out)
	$(ct) $(dbg_flags) $(out) $(src)

release:
	$(ct) $(flags) $(out) $(src) $(libs)

test:
	./$(out) $(dir)/output.trep
	./$(out) $(dir)/input.trep
	./$(out) $(dir)/var.trep
	./$(out) $(dir)/math.trep
	./$(out) $(dir)/logic.trep
	./$(out) $(dir)/eval.trep
	./$(out) $(dir)/if.trep
	./$(out) $(dir)/for.trep
	./$(out) $(dir)/while.trep
	./$(out) $(dir)/pow.trep
