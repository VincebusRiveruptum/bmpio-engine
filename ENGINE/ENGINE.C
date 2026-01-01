
#include "ENGINE.H"

Color *globalPalette = NULL;

unsigned long gameTicks = 0;
unsigned long index = 0;

// Refactor pending
bool eng_checkConfig(){
	if(config){
		return true;
	}
	return false;
}

void eng_setPalette(Color *palette){
	int i;

	for (i = 0; i < 256; i++){
		v_setPal(i, palette[i].r >> 2, palette[i].g >> 2, palette[i].b >> 2);
	}
}

// ================================================================
// MAIN LOOP'S 2d RENDERING =======================================
// ================================================================

void eng_render2d(unsigned long gametick){
	logger("\n[engine/render2d] Placeholder due that SpriteTable got refactored");
	
	/*
	if(spriteTable == NULL){
		logger("\nSprite table is NULL");
		return;
	}
	
	drawSprites(gametick);  // ISSUE
	drawAnimations(gametick);
	*/
}

/* */

void setVisPlane(){
		
}