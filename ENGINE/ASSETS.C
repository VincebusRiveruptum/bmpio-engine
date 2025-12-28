#include "ASSETS.H"

/*
	I've been thinking of making this ASSETS.C file as the 'glue' for everything, however,
	at which point the glue will be so thick that it will be a mess.
*/
List *bmpList = NULL;
Color *globalPalette = NULL;

unsigned long gameTicks = 0;
unsigned long index = 0;

// This stores all the sprites MEMORY ADDRESSES that have to be rendered on screen
typedef struct SpriteTable{
	Animation *animations[65536];
	unsigned long animationIndex;
	
	Sprite *sprites[65536];
	unsigned long spriteIndex;
} SpriteTable;

SpriteTable *spriteTable = NULL;

// Refactor pending
bool checkConfig(){
	if(config){
		return true;
	}
	return false;
}

Coordinates *createCoordinates(long x, long y, int z){
	Coordinates *newCoordinates = (Coordinates*)malloc(sizeof(Coordinates));
	newCoordinates->x = x;
	newCoordinates->y = y;
	newCoordinates->z = z;
	return newCoordinates;
}

ScreenCoordinates *createScreenCoordinates(unsigned int x, unsigned int y){
	ScreenCoordinates *newScreenCoordinates = (ScreenCoordinates*)malloc(sizeof(ScreenCoordinates));
	newScreenCoordinates->x = x;
	newScreenCoordinates->y = y;
	return newScreenCoordinates;
}

/* Animation Methods */
Animation *createAnimation(){
	Animation *newAnimation = (Animation*)malloc(sizeof(Animation));
	newAnimation->frames = NULL;
	newAnimation->length = 0;
	newAnimation->frameDelay = 0;
	newAnimation->loop = false;
	newAnimation->coordinates = createCoordinates(0, 0, 0);
	newAnimation->maskColor = 255;
	return newAnimation;
}

Sprite *createSprite(){
	Sprite *newSprite = (Sprite*)malloc(sizeof(Sprite));
	newSprite->bmpData = NULL;
	newSprite->coordinates = createCoordinates(0, 0, 0);
	newSprite->maskColor = 255;
	return newSprite;
}

bool loadSprite(Sprite *sprite, char *fileName, Coordinates *coordinates, unsigned char maskColor){
	BMPfile *loadedFrame = NULL;
	loadedFrame = loadBMPfile(fileName);
	
	if(!loadedFrame){
		printf("\nError loading sprite %s", fileName);
		return false;
	}

	if(coordinates){
		if(sprite->coordinates) free(sprite->coordinates);
		sprite->coordinates = coordinates;
	} else if (!sprite->coordinates) {
		sprite->coordinates = createCoordinates(0, 0, 0);
	}
	
	sprite->bmpData = loadedFrame->bmpData;
	sprite->maskColor = maskColor;
	
	free(loadedFrame);
	return true;
}

void loadAnimationFrames(Animation *animation, char **frameArray){
	Sprite *sprite = NULL; 
	int i;

	if (!frameArray) return;
	if (frameArray[0] == NULL) return;

	if(animation == NULL){
		animation = createAnimation();
	}
	
	for(i = 0; frameArray[i] != NULL; i++){
		sprite = createSprite();
		if(!loadSprite(sprite, frameArray[i], NULL, 0)){
			printf("\nError loading frame sprite %s", frameArray[i]);
			free(sprite);
			continue;
		}

		addGenericNode(&animation->frames, (void*)sprite);
		animation->length++;

		printf("\nLoaded frame %s", frameArray[i]);
	}
}

SpriteTable *initSpriteTable(){
	SpriteTable *newSpriteTable = (SpriteTable *) malloc(sizeof(SpriteTable));
	newSpriteTable->animationIndex = 0;
	newSpriteTable->spriteIndex = 0;
	return newSpriteTable;
}
 
void addAnimationToTable(Animation *animation){
	if(!animation) return;
	
	if(spriteTable == NULL){
		spriteTable = initSpriteTable();
	}

	spriteTable->animations[spriteTable->animationIndex] = animation;
	spriteTable->animationIndex++;
}

void addSpriteToTable(Sprite *sprite){
	if(!sprite) return;
	
	if(spriteTable == NULL){
		spriteTable = initSpriteTable();
	}

	spriteTable->sprites[spriteTable->spriteIndex] = sprite;
	spriteTable->spriteIndex++;
}

void drawAnimation(SpriteTable *spriteTable, unsigned long gametick){
	unsigned long frameToRender = 0;
	unsigned long i;
	Animation *animation = NULL;
	Node *animationSpriteNode = NULL;
	Sprite *animationSprite = NULL;
	
	if(spriteTable == NULL){
		printf("\nSprite table is NULL");
		return;
	}

	/*
		For each animation in the sprite table, we render the current frame	
	*/
	for(i = 0; i < spriteTable->animationIndex ; i++){
		animation = spriteTable->animations[i];
		
		if(animation == NULL || animation->length <= 0){
			printf("\nAnimation %ld is NULL or empty", i);
			continue;
		}

		frameToRender = gametick % animation->length;
		animationSpriteNode = getNodeByIndex(&(animation->frames), (int)frameToRender);
		
		if(animationSpriteNode == NULL){
			printf("\nAnimation sprite node %ld is NULL", frameToRender);
			continue;
		}

		animationSprite = (Sprite *)animationSpriteNode->data;
		
		if(animationSprite == NULL){
			printf("\nAnimation sprite data %ld is NULL", frameToRender);
			continue;
		}
		
		//printf("\nDrawing animation sprite %ld", frameToRender);
		drawBitmap(&animationSprite->bmpData, (unsigned int)animation->coordinates->x, (unsigned int)animation->coordinates->y, (int)animation->maskColor);
	}
}

void drawSprites(SpriteTable *spriteTable, unsigned long gametick){
	unsigned long i;
	Sprite *sprite = NULL;
	
	if(spriteTable == NULL){
		printf("\nSprite table is NULL");
		return;
	}
	
	/*
		For each sprite in the sprite table, we render it
	*/
	for(i = 0; i < spriteTable->spriteIndex ; i++){
		sprite = spriteTable->sprites[i];
		
		if(sprite == NULL){
			printf("\nSprite %ld is NULL", i);
			continue;
		}
		
		//printf("\nDrawing sprite %ld", i);
		drawBitmap(&(sprite->bmpData), (unsigned int)sprite->coordinates->x, (unsigned int)sprite->coordinates->y, (int)sprite->maskColor);
	}
}

void render2d(unsigned long gametick){
	if(spriteTable == NULL){
		printf("\nSprite table is NULL");
		return;
	}
	
	drawAnimation(spriteTable, gametick);
	drawSprites(spriteTable, gametick);  // ISSUE
}
// ================================================================

BMPfile *loadBMPfile(char *fileName){
	FILE *fp = NULL;
	BMPfile *newFile = NULL;
	char *id = (char *)calloc(3, sizeof(char));
	int padding = 0;
	int y;

	fp = fopen(fileName, "rb");

	if (!fp){
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
		newFile->bmpData->bmp[y] = (unsigned char *)malloc(sizeof(unsigned char) * (newFile->ih.x + padding));

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
			if (y + i >= 200) continue; 
			for (j = 0; j < width; j++){
				if (x + j >= 320) continue;
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

// GENERIC
void addGenericNode(List **list, void *data){
	Node *newNode = (Node *)malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;
	newNode->prev = NULL;

	addToList(list, newNode);
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
    
    long i_fixed, j_fixed, dx, dy, xp, yp;
    int nearestX, nearestY;
    unsigned char target_plane;
    static unsigned char last_plane = 0xFF;

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
            i_fixed = (long)i << 8;
			for (j = 0; j < width; j++){
				color = bmp[i][j];
				if (color != maskcolor){
                    j_fixed = (long)j << 8;
                    
                    dx = j_fixed - halfx;
                    dy = i_fixed - halfy;

                    // 8.8 * 8.8 = 16.16, shift right by 8 to get 8.8
                    xp = ((angcos * dx) >> 8) + ((angsin * dy) >> 8) + ((long)(x + 160) << 8);
                    yp = ((-angsin * dx) >> 8) + ((angcos * dy) >> 8) + ((long)(y + 100) << 8);
					
					nearestX = (int)(xp >> 8);
					nearestY = (int)(yp >> 8);

					if((nearestX < 320 && nearestX >= 0) && (nearestY < 200 && nearestY >= 0)){
						// Only switch plane if it actually changed to save I/O cycles
                        target_plane = 0x01 << (nearestX & 3);
                        
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

