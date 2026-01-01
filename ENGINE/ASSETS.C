#include "ASSETS.H"

/*
	This module is for file handling the assets and everyhting related with interacting with sprites and animations
*/

/* Animation Methods */
Animation *as_createAnimation(){
	Animation *newAnimation = (Animation*)malloc(sizeof(Animation));
	newAnimation->frames = NULL;
	newAnimation->length = 0;
	newAnimation->frameDelay = 0;
	newAnimation->loop = false;
	newAnimation->maskColor = 255;
	newAnimation->transformationList = NULL;
	return newAnimation;
}

Sprite *as_createSprite(){
	Sprite *newSprite = (Sprite*)malloc(sizeof(Sprite));
	newSprite->bmpData = NULL;
	newSprite->maskColor = 255;
	return newSprite;
}

bool as_loadSprite(Sprite *sprite, char *fileName, unsigned char maskColor){
	BMPfile *loadedFrame = NULL;
	loadedFrame = as_loadBMPfile(fileName);
	
	if(!loadedFrame){
		logger("\nError loading sprite %s", fileName);
		return false;
	}
	
	sprite->bmpData = loadedFrame->bmpData;
	sprite->maskColor = maskColor;
	
	free(loadedFrame);
	return true;
}

void as_loadAnimationFrames(Animation *animation, char **frameArray){
	Sprite *sprite = NULL; 
	int i;

	if (!frameArray) return;
	if (frameArray[0] == NULL) return;

	if(animation == NULL){
		animation = as_createAnimation();
	}
	
	for(i = 0; frameArray[i] != NULL; i++){
		sprite = as_createSprite();
		if(!as_loadSprite(sprite, frameArray[i], 15)){
			logger("\nError loading frame sprite %s", frameArray[i]);
			free(sprite);
			continue;
		}

		addGenericNode(&animation->frames, (void*)sprite);
		animation->length++;

		logger("\nLoaded frame %s", frameArray[i]);
	}
}

void as_drawAnimations(unsigned long gameTick){
	logger("\n[engine/assets/drawAnimations] Placeholder due that SpriteTable got refactored");
}

void as_drawSprites(unsigned long gameTick){
	logger("\n[engine/assets/drawSprites] Placeholder due that SpriteTable got refactored");
}
/*
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
	TranslationTransformation *translation = NULL;
	
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
				
				// ROTATION
				if (strcmp(transformation->type, TR_ROTATION) == 0){
					rot = (RotationTransformation *)transformation->data;
					
					// Every frame we add the 'angle' step to 'current'
					rot->current += rot->angle;
					
					// Keep it bounded 0-359
					if (rot->current >= 360) rot->current %= 360;
					if (rot->current < 0) rot->current = (rot->current % 360) + 360;
					
					totalAngle += rot->current;
				}
				
				// TRANSLATION
				if (strcmp(transformation->type, TR_TRANSLATION) == 0){
					// logger("\nTranslation transformation"); 
					sp_calculateTranslation(transformation, &totalOffsetX, &totalOffsetY, gametick);
				}
			}
		}
		
		// Optimization: Use standard draw if effectively not rotated
		if (totalAngle % 360 != 0) {
			drawBitmapTransform(&animationSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)animationSprite->maskColor, totalAngle);
		} else {
			drawBitmap(&animationSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)animationSprite->maskColor);
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
	
	// For each sprite in the sprite table, we render it
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
*/

// ================================================================

BMPfile *as_loadBMPfile(char *fileName){
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

void as_drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor){
	int i, j;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	int width = (int)(*bmpData)->width;
	int height = (int)(*bmpData)->height;

    int x_start = 0, y_start = 0;
    int x_end = width, y_end = height;

	if (bmp == NULL) return;

    // Clipping
    if (y < 0) { y_start = -y; }
    if (y + height > 200) y_end = 200 - y;
    if (y_start >= y_end || y >= 200 || y + height <= 0) return;

    if (x < 0) { x_start = -x; }
    if (x + width > 320) x_end = 320 - x;
    if (x_start >= x_end || x >= 320 || x + width <= 0) return;

    for (i = y_start; i < y_end; i++){
        for (j = x_start; j < x_end; j++){
            color = bmp[i][j];
            if (color != (unsigned char)maskcolor){
                v_putPixelX(x + j, y + i, color);
            }
        }
    }
}

/* Optimized Plane-batched drawing */
void as_drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor){
	int i, j, plane;
	unsigned char color = 0;
	unsigned char **bmp = (*bmpData)->bmp;
	int width = (int)(*bmpData)->width;
	int height = (int)(*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    unsigned long row_offs;

    int x_start = 0, y_start = 0;
    int x_end = width, y_end = height;

	if (bmp == NULL) return;

    // Clipping
    if (y < 0) { y_start = -y; }
    if (y + height > 200) y_end = 200 - y;
    if (y_start >= y_end || y >= 200 || y + height <= 0) return;

    if (x < 0) { x_start = -x; }
    if (x + width > 320) x_end = 320 - x;
    if (x_start >= x_end || x >= 320 || x + width <= 0) return;

    for (plane = 0; plane < 4; plane++) {
        int start_j;
        outPortb(SEQU_ADDR, 0x02);
        outPortb(SEQU_ADDR + 1, 0x01 << plane);

        // Find first j >= x_start such that (x + j) % 4 == plane
        start_j = x_start + ((plane - ((x + x_start) % 4) + 4) % 4);

        for (i = y_start; i < y_end; i++) {
            row_offs = page_offs + (unsigned long)(y + i) * 80;
            for (j = start_j; j < x_end; j += 4) {
                color = bmp[i][j];
                if (color != (unsigned char)maskcolor) {
                    v_putPixelASM(row_offs + ((x + j) >> 2), color);
                }
            }
        }
    }
}

/* This will draw an image distorted/rotated using Fixed Point Math (8.8) 
   OPTIMIZED: Inverse Mapping + Plane Batching + Loop Increments */
void as_drawBitmapTransform(BMPdata **bmpData, int x, int y, int maskcolor, int angle){
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
    unsigned long dest_offs;
    
    // Bounding Box (A bit loose for rotation safety)
    int min_x = (int)x - (int)(width >> 1);
    int max_x = (int)x + (int)width + (int)(width >> 1);
    int min_y = (int)y - (int)(height >> 1);
    int max_y = (int)y + (int)height + (int)(height >> 1);

    if (min_x < 0) min_x = 0;
    if (max_x > 320) max_x = 320;
    if (min_y < 0) min_y = 0;
    if (max_y > 200) max_y = 200;

    // Early exit
    if (min_x >= max_x || min_y >= max_y) return;

    angle %= 360;
    if (angle < 0) angle += 360;
    angcos = m_costable[angle];
    angsin = m_sintable[angle];

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
            
            // Optimization: running destination offset
            dest_offs = page_offs + (unsigned long)screen_y * 80 + (start_x >> 2);
            
            for (screen_x = start_x; screen_x < max_x; screen_x += 4) {
                u = (int)(u_fixed >> 8);
                v = (int)(v_fixed >> 8);
                
                if (u >= 0 && u < width && v >= 0 && v < height) {
                    color = bmp[v][u];
                    if (color != maskcolor) {
                        v_putPixelASM(dest_offs, color);
                    }
                }
                u_fixed += du;
                v_fixed += dv;
                dest_offs++;
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

void as_addAnimationToTable(Animation *animation){
	logger("\nThis method is going to be refactored, THIS IS A PLACEHOLDER");
}
