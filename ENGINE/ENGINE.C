
#include "ENGINE.H"

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

void setPalette(Color *palette){
	int i;

	for (i = 0; i < 256; i++){
		setPal(i, palette[i].r >> 2, palette[i].g >> 2, palette[i].b >> 2);
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
	
	drawSprites(gametick);  // ISSUE
	drawAnimations(gametick);
}