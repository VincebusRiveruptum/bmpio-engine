#include "ASSETS.H"
#include "MEM.H"

/*
	This module is for file handling the assets and everyhting related with interacting with sprites and animations
*/

/* Animation Methods */
Animation *as_createAnimation(){
	int i;
	Animation *newAnimation = (Animation*)mem_arena_alloc(gameSessionArena, sizeof(Animation));
    if(!newAnimation) return NULL;
    memset(newAnimation, 0, sizeof(Animation));

	for(i = 0; i < GM_ANIMATION_MAX_FRAMES; i++){
		newAnimation->frames[i] = NULL;
	}

	for(i = 0; i < GM_ANIMATION_MAX_TRANSFORMATIONS; i++){
		newAnimation->transformationList[i] = NULL;
	}

	newAnimation->length = 0;
	newAnimation->frameDelay = 0;
	newAnimation->loop = false;
	newAnimation->maskColor = 255;
	return newAnimation;
}

Sprite *as_createSprite(){
	Sprite *newSprite = (Sprite*)mem_arena_alloc(gameSessionArena, sizeof(Sprite));
    if(!newSprite) return NULL;
    memset(newSprite, 0, sizeof(Sprite));
	newSprite->bmpData = NULL;
	newSprite->maskColor = 255;
	return newSprite;
}

bool as_loadSprite(Sprite *sprite, char *fileName, unsigned char maskColor){
	BMPfile *loadedFrame = NULL;
	loadedFrame = as_loadBMPfile(fileName);
	
	if(!loadedFrame){
		logger("[as_loadSprite]: Error loading sprite %s", fileName);
		return false;
	}
	
	sprite->bmpData = loadedFrame->bmpData;
	sprite->maskColor = maskColor;
	
    // loadedFrame itself was a temporary wrapper. 
    // In arena mode, we don't individual free.
	return true;
}

void as_loadAnimationFrames(Animation *animation, char **frameArray, unsigned char maskColor){
	Sprite *sprite = NULL; 
	int i;

	if (!frameArray) return;
	if (frameArray[0] == NULL) return;

	if(animation == NULL){
		animation = as_createAnimation();
	}
	
	for(i = 0; frameArray[i] != NULL && i < GM_ANIMATION_MAX_FRAMES; i++){
		sprite = as_createSprite();
		if(!as_loadSprite(sprite, frameArray[i], maskColor)){
			logger("[as_loadAnimationFrames]: Error loading frame sprite %s", frameArray[i]);
			continue;
		}

		animation->frames[i] = sprite;
		animation->length++;

		logger("[as_loadAnimationFrames]: Loaded frame %s", frameArray[i]);
	}
}

BMPfile *as_loadBMPfile(char *fileName){
	FILE *fp = NULL;
	BMPfile *newFile = NULL;
	char *id = (char *)calloc(3, sizeof(char));
	int padding = 0;
	int y;
	
	fp = fopen(fileName, "rb");
	
	if (!fp){
		logger("[as_loadBMPfile]: Error, file not found!");
		return NULL;
	}

	logger("[as_loadBMPfile]: Loading %s ", fileName);

	newFile = (BMPfile *)mem_arena_alloc(gameSessionArena, sizeof(BMPfile));
	newFile->bmpData = (BMPdata *)mem_arena_alloc(gameSessionArena, sizeof(BMPdata));
	newFile->bmpData->bmp = NULL;
	newFile->bmpData->palette = (Color *)mem_arena_alloc(gameSessionArena, 256 * sizeof(Color));

	if (newFile == NULL || newFile->bmpData == NULL || newFile->bmpData->palette == NULL){
		logger("[as_loadBMPfile]: Memory allocation failed");
		return NULL;
	}
    memset(newFile->bmpData->palette, 0, 256 * sizeof(Color));

	fread(id, 2, 1, fp);

	if (strcmp(id, "BM") != 0){
		logger("[as_loadBMPfile]: Invalid file. %s", id);
		free(newFile);
		return NULL;
	}

	strncpy(newFile->fh.id, id, 2);

	fread(&(newFile->fh), 12, 1, fp);
	fread(&(newFile->ih), 40, 1, fp);

	logger("[as_loadBMPfile]: %s [ X : %ld, Y : %ld ]", fileName, newFile->ih.x, newFile->ih.y);

	newFile->bmpData->width = newFile->ih.x;
	newFile->bmpData->height = newFile->ih.y;

	/* Lectura de paleta */

	fread((newFile->bmpData->palette), 1024, 1, fp);

	/* Lectura de imagen */

	/* Allocate flat buffer */
	newFile->bmpData->bmp = (unsigned char *)mem_arena_alloc(gameSessionArena, (newFile->ih.x + padding) * newFile->ih.y);
	if (newFile->bmpData->bmp == NULL)
	{
		logger("[as_loadBMPfile]: Could not allocate flat pixel buffer");
		return NULL;
	}

	/* Read BMP rows in reverse order so they are right-side up in memory */
	for (y = (int)newFile->ih.y - 1; y >= 0; y--) {
		unsigned char *dest = newFile->bmpData->bmp + (unsigned long)y * (newFile->ih.x + padding);
		fread(dest, newFile->ih.x + padding, 1, fp);
	}

	fclose(fp);
	return newFile;
}

void as_drawBitmap(BMPdata **bmpData, int x, int y, int maskcolor){
	int i, j;
	unsigned char *bmp = (*bmpData)->bmp;
	int width = (int)(*bmpData)->width;
	int height = (int)(*bmpData)->height;
    int x_start = 0, y_start = 0;
    int x_end = width, y_end = height;
 	unsigned char *row_ptr;
	unsigned char color;
	if (bmp == NULL) return;

    /* Adjust for CENTER */
    x = x - (width >> 1);
    y = y - (height >> 1);

    /* Clipping */
    if (y < 0) { y_start = -y; }
    if (y + height > 200) y_end = 200 - y;
    if (y_start >= y_end || y >= 200 || y + height <= 0) return;

    if (x < 0) { x_start = -x; }
    if (x + width > 320) x_end = 320 - x;
    if (x_start >= x_end || x >= 320 || x + width <= 0) return;

    for (i = y_start; i < y_end; i++){
        row_ptr = bmp + (unsigned long)i * width;
        for (j = x_start; j < x_end; j++){
            color = row_ptr[j];
            if (color != (unsigned char)maskcolor){
                v_putPixelX(x + j, y + i, color);
            }
        }
    }
}

/* Optimized Plane-batched drawing */
void as_drawBitmapPlaneBatch(BMPdata **bmpData, int x, int y, int maskcolor){
	int i, j, plane;
	unsigned char *bmp = (*bmpData)->bmp;
	int width = (int)(*bmpData)->width;
	int height = (int)(*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    int x_start = 0, y_start = 0;
    int x_end = width, y_end = height;
    int start_j = 0;
    unsigned char *row_ptr;
    unsigned char color;
	unsigned long current_dest_offs;
    if (bmp == NULL) return;

    /* Adjust for CENTER */
    x = x - (width >> 1);
    y = y - (height >> 1);

    /* Clipping */
    if (y < 0) { y_start = -y; }
    if (y + height > 200) y_end = 200 - y;
    if (y_start >= y_end || y >= 200 || y + height <= 0) return;

    if (x < 0) { x_start = -x; }
    if (x + width > 320) x_end = 320 - x;
    if (x_start >= x_end || x >= 320 || x + width <= 0) return;

    for (plane = 0; plane < 4; plane++) {
        outPortb(SEQU_ADDR, 0x02);
        outPortb(SEQU_ADDR + 1, 0x01 << plane);

        // Find first j >= x_start such that (x + j) % 4 == plane
        start_j = x_start + ((plane - ((x + x_start) % 4) + 4) % 4);

        for (i = y_start; i < y_end; i++) {
            row_ptr = bmp + (unsigned long)i * width;
            current_dest_offs = page_offs + (unsigned long)(y + i) * 80 + (unsigned long)((x + start_j) >> 2);
            
            for (j = start_j; j < x_end; j += 4) {
                color = row_ptr[j];
                if (color != (unsigned char)maskcolor) {
                    v_putPixelASM(current_dest_offs, color);
                }
                current_dest_offs++;
            }
        }
    }
}

/* This will draw an image distorted/rotated using Fixed Point Math (8.8) 
   OPTIMIZED: Inverse Mapping + Plane Batching + Loop Increments */
void as_drawBitmapTransform(BMPdata **bmpData, int x, int y, int maskcolor, int angle){
    unsigned char *bmp = (*bmpData)->bmp;
    unsigned int width = (unsigned int)(*bmpData)->width;
    unsigned int height = (unsigned int)(*bmpData)->height;
    unsigned long page_offs = pageOffsets[nextPage];
    unsigned char color;
    long angcos, angsin;
    long halfw = (long)width << 7;
    long halfh = (long)height << 7;
    int screen_x, screen_y, plane;
    long dx, dy, u_fixed, v_fixed;
    long du, dv;
    int u, v;
    unsigned long dest_offs;
    
    // Bounding Box
    int min_x = (int)x - (int)(width >> 1);
    int max_x = (int)x + (int)width + (int)(width >> 1);
    int min_y = (int)y - (int)(height >> 1);
    int max_y = (int)y + (int)height + (int)(height >> 1);

    if (min_x < 0) min_x = 0;
    if (max_x > 320) max_x = 320;
    if (min_y < 0) min_y = 0;
    if (max_y > 200) max_y = 200;

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
            dy = ((long)screen_y - ((long)y + (int)(height >> 1))) << 8;
            dx = ((long)start_x - ((long)x + (int)(width >> 1))) << 8;
            
            u_fixed = ((dx * angcos + dy * angsin) >> 8) + halfw;
            v_fixed = ((-dx * angsin + dy * angcos) >> 8) + halfh;
            
            dest_offs = page_offs + (unsigned long)screen_y * 80 + (unsigned long)(start_x >> 2);
            
            for (screen_x = start_x; screen_x < max_x; screen_x += 4) {
                u = (int)(u_fixed >> 8);
                v = (int)(v_fixed >> 8);
                
                if (u >= 0 && u < (int)width && v >= 0 && v < (int)height) {
                    color = bmp[(unsigned long)v * width + (unsigned long)u];
                    if (color != (unsigned char)maskcolor) {
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

bool as_addTransformation(Animation *animation, Transformation *transformation){
	int i;

	if(!animation || !transformation) return false;
	
	for(i = 0; i < GM_ANIMATION_MAX_TRANSFORMATIONS; i++){
		if(animation->transformationList[i] == NULL){
			animation->transformationList[i] = transformation;
			logger("\nAdded transformation at slot %d", i);
			return true;
		}
	}

	logger("[as_addTransformation]: Error: Transformation list full");
	return false;
}

bool as_removeTransformation(Animation *animation, int index){
	if(!animation || index < 0 || index >= GM_ANIMATION_MAX_TRANSFORMATIONS) return false;

	animation->transformationList[index] = NULL;
	return true;
}

RotationTransformation *as_createRotationTransformation(int angle, int current){
	RotationTransformation *newRotationTransformation = NULL;

	newRotationTransformation = (RotationTransformation *)malloc(sizeof(RotationTransformation));
	if (!newRotationTransformation){
		logger("[as_createRotationTransformation]: Could not allocate memory for new transformation");
		return NULL;
	}
	newRotationTransformation->angle = angle;
	newRotationTransformation->current = current;

	logger("[as_createRotationTransformation]: Created rotation transformation");
	return newRotationTransformation;
}

bool as_addRotationTransformation(Animation *animation, RotationTransformation *transformation){
	Transformation *newTransformation = NULL;
	RotationTransformation *newRotationTransformation = NULL;

	if (!animation){
		return false;
	}

    /* If no transformation provided, create a default one */
	if (!transformation){
		newRotationTransformation = (RotationTransformation *)malloc(sizeof(RotationTransformation));
		if (!newRotationTransformation){
			logger("[as_addRotationTransformation]: Could not allocate memory for new internal rotation data");
			return false;
		}
        newRotationTransformation->angle = 0;
        newRotationTransformation->current = 0;
	} else {
        newRotationTransformation = transformation;
    }

	newTransformation = (Transformation *)mem_arena_alloc(gameSessionArena, sizeof(Transformation));
	
	if (!newTransformation){
		logger("[as_addRotationTransformation]: Could not allocate memory for new transformation wrapper");
		return false;
	}

	newTransformation->type = TR_ROTATION;
	newTransformation->data = (void *)newRotationTransformation;

	if (!as_addTransformation(animation, newTransformation)){
		logger("[as_addRotationTransformation]: Could not add transformation to animation");
        free(newTransformation);
        if (!transformation) free(newRotationTransformation);
		return false;
	}

	logger("[as_addRotationTransformation]: Added rotation transformation to animation");
	return true;
}