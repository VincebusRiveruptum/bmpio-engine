#include "ASSETS.H"

/*
	I've been thinking of making this ASSETS.C file as the 'glue' for everything, however,
	at which point the glue will be so thick that it will be a mess.
*/
List *bmpList = NULL;
Color *globalPalette = NULL;
// This stores all the sprites MEMORY ADDRESSES that have to be rendered on screen
SpriteTable *spriteTable = NULL;

unsigned long gameTicks = 0;
unsigned long index = 0;

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
Animation *createAnimation(Coordinates *coordinates){
	Animation *newAnimation = (Animation*)malloc(sizeof(Animation));
	newAnimation->frames = NULL;
	newAnimation->length = 0;
	newAnimation->frameDelay = 0;
	newAnimation->loop = false;

	if(coordinates){
		newAnimation->coordinates = coordinates;
	} else {
		newAnimation->coordinates = createCoordinates(0, 0, 0);
	}
	newAnimation->maskColor = 255;
	newAnimation->transformationList = NULL;
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
		logger("\nError loading sprite %s", fileName);
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
		animation = createAnimation(createCoordinates(0, 0, 0));
	}
	
	for(i = 0; frameArray[i] != NULL; i++){
		sprite = createSprite();
		if(!loadSprite(sprite, frameArray[i], NULL, 0)){
			logger("\nError loading frame sprite %s", frameArray[i]);
			free(sprite);
			continue;
		}

		addGenericNode(&animation->frames, (void*)sprite);
		animation->length++;

		logger("\nLoaded frame %s", frameArray[i]);
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

void drawAnimations(unsigned long gametick){
	unsigned long frameToRender = 0;
	unsigned long i;
	Animation *animation = NULL;
	Node *animationSpriteNode = NULL;
	Sprite *animationSprite = NULL;
	
	Transformation *transformation = NULL;
	int transformationLength = 0;
	int transformationIndex = 0;
	
	int totalAngle = 0;
	long totalOffsetX = 0;
	long totalOffsetY = 0;
	RotationTransformation *rot = NULL;
	MovementTransformation *mov = NULL;
	
	if(spriteTable == NULL){
		logger("\nSprite table is NULL");
		return;
	}

	// Per each animation
	for(i = 0; i < spriteTable->animationIndex ; i++){
		animation = spriteTable->animations[i];
		
		if(animation == NULL || animation->length <= 0){
			logger("\nAnimation %ld is NULL or empty", i);
			continue;
		}

		frameToRender = gametick % animation->length;
		animationSpriteNode = getNodeByIndex(&(animation->frames), (int)frameToRender);
		
		if(animationSpriteNode == NULL){
			logger("\nAnimation sprite node %ld is NULL", frameToRender);
			continue;
		}

		animationSprite = (Sprite *)animationSpriteNode->data;
		
		if(animationSprite == NULL){
			logger("\nAnimation sprite data %ld is NULL", frameToRender);
			continue;
		}
		
		totalAngle = 0;
		totalOffsetX = animation->coordinates->x + animationSprite->coordinates->x;
		totalOffsetY = animation->coordinates->y + animationSprite->coordinates->y;

		if(animation->transformationList != NULL){
			transformationLength = animation->transformationList->length;

			for(transformationIndex = 0; transformationIndex < transformationLength; transformationIndex++){
				Node *node = getNodeByIndex(&(animation->transformationList), transformationIndex);
				if(node == NULL) continue;
				transformation = (Transformation *)node->data;
				if(transformation == NULL) continue;

				if (strcmp(transformation->type, TR_ROTATION) == 0){
					rot = (RotationTransformation *)transformation->data;
					
					// Every frame we add the 'angle' step to 'current'
					rot->current += rot->angle;
					
					// Keep it bounded 0-359
					if (rot->current >= 360) rot->current %= 360;
					if (rot->current < 0) rot->current = (rot->current % 360) + 360;

					totalAngle += rot->current;
				}
			}
		}

		// Optimization: Use standard draw if effectively not rotated
		if (totalAngle % 360 != 0) {
			drawBitmapDistorted(&animationSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)animation->maskColor, totalAngle);
		} else {
			drawBitmap(&animationSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)animation->maskColor);
		}
	}
}

void drawSprites(unsigned long gametick){
	unsigned long i;
	Sprite *sprite = NULL;
	
	if(spriteTable == NULL){
		logger("\nSprite table is NULL");
		return;
	}
	
	/*
		For each sprite in the sprite table, we render it
	*/
	for(i = 0; i < spriteTable->spriteIndex ; i++){
		sprite = spriteTable->sprites[i];
		
		if(sprite == NULL){
			logger("\nSprite %ld is NULL", i);
			continue;
		}
		
		//logger("\nDrawing sprite %ld", i);
		drawBitmap(&(sprite->bmpData), (unsigned int)sprite->coordinates->x, (unsigned int)sprite->coordinates->y, (int)sprite->maskColor);
	}
}
// ================================================================
// MAIN LOOP'S 2d RENDERING =======================================
// ================================================================

void render2d(unsigned long gametick){
	if(spriteTable == NULL){
		logger("\nSprite table is NULL");
		return;
	}
	
	drawAnimations(gametick);
	drawSprites(gametick);  // ISSUE
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
		logger("\nError, file not found!");
		return NULL;
	}

	logger("\nLoading %s ", fileName);

	newFile = (BMPfile *)malloc(sizeof(BMPfile));
	newFile->bmpData = (BMPdata *)malloc(sizeof(BMPdata));
	newFile->bmpData->bmp = NULL;
	newFile->bmpData->palette = (Color *)malloc(256 * sizeof(Color));

	if (newFile == NULL || newFile->bmpData == NULL || newFile->bmpData->palette == NULL){
		logger("Memory allocation failed\n");
		return NULL;
	}

	fread(id, 2, 1, fp);

	logger("%s", id);

	if (strcmp(id, "BM") != 0){
		/* El archivo es invalido no se crea la bmp */
		logger("\nInvalid file. %s", id);
		free(newFile);
		return NULL;
	}

	strncpy(newFile->fh.id, id, 2);

	fread(&(newFile->fh), 12, 1, fp);
	fread(&(newFile->ih), 40, 1, fp);

	logger("[ X : %ld, Y : %ld ]", newFile->ih.x, newFile->ih.y);

	newFile->bmpData->width = newFile->ih.x;
	newFile->bmpData->height = newFile->ih.y;

	/* Lectura de paleta */

	fread((newFile->bmpData->palette), 1024, 1, fp);

	/* Lectura de imagen */

	newFile->bmpData->bmp = (unsigned char **)malloc(sizeof(unsigned char *) * newFile->ih.y);

	if (newFile->bmpData->bmp == NULL){
		logger("\nCould not allocate bmp height.");
		return 0;
	}

	while ((newFile->ih.x + padding) % 4 != 0){
		padding++;
	};

	for (y = (int) newFile->ih.y - 1; y >= 0; y--){
		newFile->bmpData->bmp[y] = (unsigned char *)malloc(sizeof(unsigned char) * (newFile->ih.x + padding));

		if (newFile->bmpData->bmp[y] == NULL)
		{
			logger("\nCould not allocate bitmap width on loop index : %d", y);
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

void drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor){
	long i, j;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;

	if (bmp != NULL){
		for (i = 0; i < height; i++){
			if (y + i < 0 || y + i >= 200) continue; 
			for (j = 0; j < width; j++){
				if (x + j < 0 || x + j >= 320) continue;
				color = bmp[i][j];
				if (color != maskcolor){
					putPixelX(j + x, i + y, color);
				}
			}
		}
	}
}

/* Optimized Plane-batched drawing */
void drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor){
	long i, j, plane;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	unsigned int width = (*bmpData)->width;
	unsigned int height = (*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    unsigned long row_offs;

	if (bmp != NULL){
        for (plane = 0; plane < 4; plane++) {
            outPortb(SEQU_ADDR, 0x02);
            outPortb(SEQU_ADDR + 1, 0x01 << plane);

            for (i = 0; i < height; i++) {
                if (y + i < 0 || y + i >= 200) continue;
                row_offs = page_offs + (unsigned long)(i + y) * 80;
                for (j = plane; j < width; j += 4) {
                    if (x + j < 0 || x + j >= 320) continue;
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


/* This will draw an image distorted/rotated using Fixed Point Math (8.8) 
   OPTIMIZED: Inverse Mapping + Plane Batching + Loop Increments */
void drawBitmapDistorted(BMPdata **bmpData, int x, int y, int maskcolor, int angle){
    unsigned char **bmp = (*bmpData)->bmp;
    unsigned int width = (*bmpData)->width;
    unsigned int height = (*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    
    long angcos, angsin;
    long halfw = (long)width << 7;
    long halfh = (long)height << 7;
    int screen_x, screen_y, plane;
    long dx, dy, u_fixed, v_fixed;
    long du, dv;
    int u, v;
    unsigned char color;
    
    // Bounding Box (A bit loose for rotation safety)
    int min_x = (int)x - (int)(width >> 1);
    int max_x = (int)x + (int)width + (int)(width >> 1);
    int min_y = (int)y - (int)(height >> 1);
    int max_y = (int)y + (int)height + (int)(height >> 1);

    if (min_x < 0) min_x = 0;
    if (max_x > 320) max_x = 320;
    if (min_y < 0) min_y = 0;
    if (max_y > 200) max_y = 200;

    angle %= 360;
    if (angle < 0) angle += 360;
    angcos = costable[angle];
    angsin = sintable[angle];

    if (angcos == -2147483648L || angsin == -2147483648L) return;

    du = angcos << 2;
    dv = -angsin << 2;

    for (plane = 0; plane < 4; plane++) {
        int start_x = min_x + ((plane - (min_x % 4) + 4) % 4);
        outPortb(SEQU_ADDR, 0x02);
        outPortb(SEQU_ADDR + 1, 0x01 << plane);
        
        for (screen_y = min_y; screen_y < max_y; screen_y++) {
            dy = ((long)screen_y - ((long)y + (height >> 1))) << 8;
            dx = ((long)start_x - ((long)x + (width >> 1))) << 8;
            
            // Calculate initial u, v for the start of the row
            u_fixed = ((dx * angcos + dy * angsin) >> 8) + halfw;
            v_fixed = ((-dx * angsin + dy * angcos) >> 8) + halfh;
            
            for (screen_x = start_x; screen_x < max_x; screen_x += 4) {
                u = (int)(u_fixed >> 8);
                v = (int)(v_fixed >> 8);
                
                if (u >= 0 && u < width && v >= 0 && v < height) {
                    color = bmp[v][u];
                    if (color != maskcolor) {
                        putPixelASM(page_offs + (unsigned long)screen_y * 80 + (screen_x >> 2), color);
                    }
                }
                u_fixed += du;
                v_fixed += dv;
            }
        }
    }
}

bool addTransformation(Animation *animation, void *transformation){
	Node *newNode = NULL;

	if(!animation || !transformation) return false;
	
	newNode = (Node *)malloc(sizeof(Node));
	if(!newNode) {
		logger("\nCould not allocate memory for new node");
		return false;
	}
	newNode->data = transformation;
	newNode->next = NULL;
	newNode->prev = NULL;
	
	addToList(&animation->transformationList, newNode);
	logger("\nAdded transformation. New list length: %d", animation->transformationList->length);
	return true;
}

bool removeTransformation(Animation *animation, int index){
	if(!animation || !index) return false;

	deleteNodeByIndex(&animation->transformationList, index);
	return true;
}

