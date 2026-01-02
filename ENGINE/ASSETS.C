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
		logger("[as_loadSprite]: Error loading sprite %s", fileName);
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
			logger("[as_loadAnimationFrames]: Error loading frame sprite %s", frameArray[i]);
			free(sprite);
			continue;
		}

		addGenericNode(&animation->frames, (void*)sprite);
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

	newFile = (BMPfile *)malloc(sizeof(BMPfile));
	newFile->bmpData = (BMPdata *)malloc(sizeof(BMPdata));
	newFile->bmpData->bmp = NULL;
	newFile->bmpData->palette = (Color *)malloc(256 * sizeof(Color));

	if (newFile == NULL || newFile->bmpData == NULL || newFile->bmpData->palette == NULL){
		logger("[as_loadBMPfile]: Memory allocation failed");
		return NULL;
	}

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

	newFile->bmpData->bmp = (unsigned char **)malloc(sizeof(unsigned char *) * newFile->ih.y);

	if (newFile->bmpData->bmp == NULL){
		logger("[as_loadBMPfile]: Could not allocate bmp height.");
		return NULL;
	}

	while ((newFile->ih.x + padding) % 4 != 0){
		padding++;
	};

	for (y = (int) newFile->ih.y - 1; y >= 0; y--){
		newFile->bmpData->bmp[y] = (unsigned char *)malloc(sizeof(unsigned char) * (newFile->ih.x + padding));

		if (newFile->bmpData->bmp[y] == NULL)
		{
			logger("[as_loadBMPfile]: Could not allocate bitmap width on loop index : %d", y);
			return NULL;
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

bool as_addTransformation(Animation *animation, void *transformation){
	Node *newNode = NULL;

	if(!animation || !transformation) return false;
	
	newNode = (Node *)malloc(sizeof(Node));
	if(!newNode) {
		logger("[Could not allocate memory for new node");
		return false;
	}
	newNode->data = transformation;
	newNode->next = NULL;
	newNode->prev = NULL;
	
	addToList(&animation->transformationList, newNode);
	logger("\nAdded transformation. New list length: %d", animation->transformationList->length);
	return true;
}

bool as_removeTransformation(Animation *animation, int index){
	if(!animation || !index) return false;

	deleteNodeByIndex(&animation->transformationList, index);
	return true;
}
