CC=gcc -Wall -g -O0
LIBS=`pkg-config --libs --static --cflags "glfw3" "glew" "freeglut" "cglm"` -lfreeimage -lmathc
FILES=fll/fll.c ill/ill.c lodepng.c mathc/mathc.c
CFLAGS=$(LIBS)

hw:
	$(CC) hello_world.c -o hw $(CFLAGS)
ht:
	$(CC) hello_triangle.c -o ht $(CFLAGS)
hr:
	$(CC) hello_rectangle.c -o hr $(CFLAGS)
ha:
	$(CC) hello_animation.c -o ha $(CFLAGS)
hc:
	$(CC) hello_colors.c -o hc $(CFLAGS)
tex:
	$(CC) texture.c -o tex $(CFLAGS)
tex2:
	$(CC) texture2.c -o tex2 $(CFLAGS)
mat:
	$(CC) matrix.c $(FILES) -o mat $(CFLAGS)
test:
	$(CC) test.c -o test $(CFLAGS)

rm:
	rm *.exe
