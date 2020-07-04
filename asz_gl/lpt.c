#include <stdio.h>
#include <stdlib.h>
#include "lodepng/lodepng.h"

int main() {
	unsigned char *image = 0;	
	unsigned int err = 0, w = 0, h = 0;

	err = lodepng_decode32_file(&image, &w, &h, "pic.png");
	if (err)
		fprintf(stderr, "some error\n");
	free(image);
	return 0;
}
