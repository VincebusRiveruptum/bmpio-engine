#include "ASSETS.H"

/*
	I've been thinking of making this ASSETS.C file as the 'glue' for everything, however,
	at which point the glue will be so thick that it will be a mess.
*/
Config *gameConfig = NULL;
List *bmpList = NULL;
Color *globalPalette = NULL;

bool checkConfig(){
	if(gameConfig){
		if( gameConfig->assetsPath &&
			gameConfig->logType && 
			gameConfig->playerName	
		)	return true;
	}
	return false;
}

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

/* Optimized Plane-batched drawing */
void drawBitmapPlaneBatch(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor){
	long i, j, plane;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    unsigned long row_offs;

	if (bmp != NULL){
        for (plane = 0; plane < 4; plane++) {
            // Set VGA Map Mask for this plane
            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0x01 << plane);

            for (i = 0; i < height; i++) {
                row_offs = page_offs + (unsigned long)(i + y) * 80;
                for (j = plane; j < width; j += 4) {
                    color = bmp[i][j];
                    if (color != maskcolor) {
                        putPixelASM(row_offs + ((j + x) >> 2), color);
                    }
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

/* Precomputed Trig Tables for performance */
static long sintable[360];
static long costable[360];
static int trigInitialized = 0;

void initTrig() {
    int i;
    for (i = 0; i < 360; i++) {
        float rad = (PI * i) / 180.0f;
        sintable[i] = (long)(sin(rad) * 256.0f);
        costable[i] = (long)(cos(rad) * 256.0f);
    }
    trigInitialized = 1;
}

/* This will draw an image distorted/rotated using Fixed Point Math (8.8) */
void drawBitmapDistorted(BMPdata **bmpData, unsigned int x, unsigned int y, int maskcolor, int angle){
	long i, j;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;

    long angcos, angsin;
    long halfx, halfy;
    unsigned long page_offs = pageOffsets[nextPage];
    
    // Normalize angle
    angle %= 360;
    if (angle < 0) angle += 360;

    if (!trigInitialized) initTrig();

    angcos = costable[angle];
    angsin = sintable[angle];

	halfx = (long)width << 7;  // width / 2 << 8
	halfy = (long)height << 7; // height / 2 << 8

	if (bmp != NULL){
		for (i = 0; i < height; i++){
            long i_fixed = (long)i << 8;
			for (j = 0; j < width; j++){
				color = bmp[i][j];
				if (color != maskcolor){
                    long j_fixed = (long)j << 8;
                    
                    long dx = j_fixed - halfx;
                    long dy = i_fixed - halfy;

                    // 8.8 * 8.8 = 16.16, shift right by 8 to get 8.8
                    long xp = ((angcos * dx) >> 8) + ((angsin * dy) >> 8) + ((long)(x + 160) << 8);
                    long yp = ((-angsin * dx) >> 8) + ((angcos * dy) >> 8) + ((long)(y + 100) << 8);
					
					int nearestX = (int)(xp >> 8);
					int nearestY = (int)(yp >> 8);

					if((nearestX < 320 && nearestX >= 0) && (nearestY < 200 && nearestY >= 0)){
						// Only switch plane if it actually changed to save I/O cycles
                        unsigned char target_plane = 0x01 << (nearestX & 3);
                        static unsigned char last_plane = 0xFF;
                        
                        if (target_plane != last_plane) {
                            outPortb(SEQU_ADDR, 0x02);
                            outPortb(SEQU_ADDR + 1, target_plane);
                            last_plane = target_plane;
                        }
                        putPixelASM(page_offs + (unsigned long)nearestY * 80 + (nearestX >> 2), color);
					}
				}
			}
		}
	}
}

