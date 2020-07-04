/************************************************************************
 This is a very simple program that uses the FreeImage library.
 It loads the input JPEG file and saves it as a BMP file.
 For further examples (for instance, how to access and manipulate
 the pixel data) read the library manual.
************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* The FreeImage library header */
#include <FreeImage.h>
#include <stdbool.h>

void PrintUsage(const char *progName);

int main(int argc, char **argv) {

	/* Check correct number of parameters */ 
	/* We expect one argument which is the input filename */
	if (argc != 2) {
		PrintUsage(argv[0]);
		return -1;
	}

	/* Initialize the FreeImage library */
    FreeImage_Initialise(true);

	/* Load the jpg file to internal data structure FIBITMAP */
	FIBITMAP * sourceBitmap = FreeImage_Load(FIF_JPEG, argv[1], JPEG_DEFAULT);

	/* check that the image was properly loaded */
	if (! sourceBitmap) {
		printf("Couldn't load %s\n", argv[1]);
		
		/* Unload the FreeImage library */
		FreeImage_DeInitialise();

		return -2;
	}

	/* save the image as bmp */
	char * outFilename;
	outFilename = (char *)malloc(sizeof(char)*(strlen(argv[1]) + 1));
	if (outFilename == NULL) {
		printf("Failed to allocate memory\n");
		FreeImage_DeInitialise();
		return -3;
	}
	/* create the output filename with extension bmp */ 
	strcpy(outFilename, argv[1]);
	sprintf(outFilename + (strlen(outFilename) - 3), "bmp");

	/* Save the image as bmp */
	FreeImage_Save(FIF_BMP, sourceBitmap, outFilename, 0);

	/* Release the image data structure */
	FreeImage_Unload(sourceBitmap);

	/* Release our own allocated memory */
	free(outFilename);

	/* Unload the FreeImage library */
	FreeImage_DeInitialise();

	return 0;
}

void PrintUsage(const char *progName) {
	printf("Converts JPEG images to BMP images.\n");
	printf("Usage:\n");
	printf("%s <source_image.bmp>\n", progName);
}
