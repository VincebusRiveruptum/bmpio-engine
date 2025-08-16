/*
	This file handles bmp file operations
*/

#include "bmp.h"
#include <math.h>

BMPfile *loadBMPfile(char *fileName){
	FILE *fp = NULL;
	BMPfile *newFile = NULL;
	char *id = (char *)calloc(3, sizeof(char));
	int padding = 0;
	int y;

	fp = fopen(fileName, "rb");

	if (!(fp = fopen(fileName, "rb"))){
		printf("\nError, file not found!");
		return NULL;
	}

	printf("\nLoading %s ", fileName);

	newFile = (BMPfile *)malloc(sizeof(BMPfile));
	newFile->bmpData = (BMPdata *)malloc(sizeof(BMPdata));
	newFile->bmpData->bmp = NULL;
	newFile->bmpData->palette = (Color *)malloc(256 * sizeof(Color));

	if (newFile == NULL || newFile->bmpData == NULL || newFile->bmpData->palette == NULL){
		printf("Memory allocation failed\n");
		return NULL;
	}

	fread(id, 2, 1, fp);

	printf("%s", id);

	if (strcmp(id, "BM") != 0){
		/* El archivo es invalido no se crea la bmp */
		printf("\nInvalid file. %s", id);
		free(newFile);
		return NULL;
	}

	strncpy(newFile->fh.id, id, 2);

	fread(&(newFile->fh), 12, 1, fp);
	fread(&(newFile->ih), 40, 1, fp);

	printf("[ X : %ld, Y : %ld ]", newFile->ih.x, newFile->ih.y);

	newFile->bmpData->width = newFile->ih.x;
	newFile->bmpData->height = newFile->ih.y;

	/* Lectura de paleta */

	fread((newFile->bmpData->palette), 1024, 1, fp);

	/* Lectura de imagen */

	newFile->bmpData->bmp = (unsigned char **)malloc(sizeof(unsigned char *) * newFile->ih.y);

	if (newFile->bmpData->bmp == NULL){
		printf("\nCould not allocate bmp height.");
		return 0;
	}

	while ((newFile->ih.x + padding) % 4 != 0){
		padding++;
	};

	for (y = (int) newFile->ih.y - 1; y >= 0; y--){
		newFile->bmpData->bmp[y] = (unsigned char *)malloc(sizeof(unsigned char) * (newFile->ih.x));

		if (newFile->bmpData->bmp[y] == NULL)
		{
			printf("\nCould not allocate bitmap width on loop index : %d", y);
			return 0;
		}
		else
		{
			fread(newFile->bmpData->bmp[y], newFile->ih.x + padding, 1, fp);
		}
		
	}

	free(id);
	fclose(fp);

	return newFile;
}

/* This will draw an image on the screen*/
void drawBitmap(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor){
	long i, j;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;

	if (bmp != NULL){
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				color = bmp[i][j];
				if (color != maskcolor){
					putPixelX(j + x, i + y, color);
				}
			}
		}
	}
	
}

void addBMPtoList(List **bmpList, BMPdata *bmpData){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = bmpData;
	newNode->next = NULL;
	newNode->prev = NULL;

	addToList(bmpList, newNode);
}

void drawList(List *list){
	BMPdata *currentBmp = NULL;
	int i = 0;

	if (list != NULL){
		for (i = 0; i < list->length; i++){
			currentBmp = getNodeByIndex(&list, i)->data;

			if (currentBmp != NULL)
			{
				drawBitmap(&currentBmp, 1, 1, 255);
			}
		}
	}
	else{
		return;
	}
}

void setPalette(Color *palette){
	int i;

	for (i = 0; i < 256; i++){
		setPal(i, palette[i].r >> 2, palette[i].g >> 2, palette[i].b >> 2);
	}
}

int round(float x) {
    return (int)(x + 0.5f);
}

/* This will draw an image on the screen*/
void drawBitmapDistorted(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor, int angle){
	long i, j;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;

	//unsigned float degr = PI / 6; // 30°
	float xp, yp;
	int nearestX, nearestY;

	// sin( pi / 6 ) = 0.5
	// cos( pi / 6 ) = 0.8660

	float rad = (PI * angle ) / 180;
	float angcos = cos(rad);
	float angsin = sin(rad);

	float halfx = width / 2;
	float halfy = height / 2;

	if (bmp != NULL){
		for (i = 0; i < height; i++){
			for (j = 0; j < width; j++){
				color = bmp[i][j];
				if (color != maskcolor){

					xp = (angcos * (j - halfy) + ( angsin * (i - halfx))) + (x + (160));
					yp = (- 1 * angsin * (j - halfy) + ( angcos * (i - halfx))) + (y + 100);
					
					nearestX = (int) round(xp);
					nearestY = (int) round(yp);

					if((nearestX < 320 && nearestX >= 0) && (nearestY < 200 && nearestY >= 0)){
						putPixelX(nearestX, nearestY, color);
					}
				}
			}
		}
	}
}

