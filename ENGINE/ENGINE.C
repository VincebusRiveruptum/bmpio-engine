
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

void eng_renderFrame(unsigned long gametick){
	unsigned long frameToRender = 0;
	unsigned long i;
	Animation *actorAnimation = NULL;
	Node *actorSpriteNode = NULL;
	Sprite *actorSprite = NULL;
	ScreenCoordinates *screenPos = NULL;
	
	Transformation *transformation = NULL;
	int transformationLength = 0;
	int transformationIndex = 0;
	
	int totalAngle = 0;
	long totalOffsetX = 0;
	long totalOffsetY = 0;
	RotationTransformation *rot = NULL;
	TranslationTransformation *translation = NULL;
	
	if(renderQueue == NULL){
		logger("[eng_renderFrame]: Render queue is NULL");
		return;
	}
	
	logger("[eng_renderFrame]: Starting render, scanning %d slots", SP_GRID_SIZE);
	
	for(i = 0; i < SP_GRID_SIZE; i++){
		if(renderQueue[i] == NULL){
			continue;
		}
		
		logger("[eng_renderFrame]: Found asset at queue slot %ld", i);
		
		if(renderQueue[i]->actor == NULL){
			logger("[eng_renderAssets]:Render queue %ld actor is NULL", i);
			continue;
		}

		if(	renderQueue[i]->actor->currentAction == NULL || 
			renderQueue[i]->actor->currentAction->animation == NULL || 
			renderQueue[i]->actor->currentAction->animation->length == 0){
			logger("[eng_renderAssets]:Render queue %ld actor animation is NULL", i);
			continue;
		}
		
		actorAnimation = renderQueue[i]->actor->currentAction->animation;
		
		frameToRender = gametick % actorAnimation->length;
		actorSpriteNode = getNodeByIndex(&(actorAnimation->frames), (int)frameToRender);
		
		if(actorSpriteNode == NULL){
			logger("[eng_renderAssets]:Actor sprite node %ld is NULL", frameToRender);
			continue;
		}
		
		actorSprite = (Sprite *)actorSpriteNode->data;
		
		if(actorSprite == NULL){
			logger("[eng_renderAssets]:Actor sprite data %ld is NULL", frameToRender);
			continue;
		}
		
		
		totalAngle = 0;
		
		/* Project world coordinates to screen coordinates */
		screenPos = sp_worldToScreen(renderQueue[i]->coordinates, globalCamera);
		if(!screenPos){
			logger("[eng_renderFrame]: Failed to project world to screen");
			continue;
		}
		
		if(!sp_isInFrustrum(screenPos, actorSprite)){
			free(screenPos);
			continue;
		}

		totalOffsetX = screenPos->x;
		totalOffsetY = screenPos->y;
		
		if(actorAnimation->transformationList && actorAnimation->transformationList->length > 0){
			transformationLength = actorAnimation->transformationList->length;
			
			for(transformationIndex = 0; transformationIndex < transformationLength; transformationIndex++){
				Node *node = getNodeByIndex(&(actorAnimation->transformationList), transformationIndex);
				if(node == NULL) continue;
				transformation = (Transformation *)node->data;
				if(transformation == NULL) continue;
				
				// ROTATION
				if (transformation->type == TR_ROTATION){
					rot = (RotationTransformation *)transformation->data;
					
					// Every frame we add the 'angle' step to 'current'
					rot->current += rot->angle;
					
					// Keep it bounded 0-359
					if (rot->current >= 360) rot->current %= 360;
					if (rot->current < 0) rot->current = (rot->current % 360) + 360;
					
					totalAngle += rot->current;
				}
				
				// TRANSLATION
				if (transformation->type == TR_TRANSLATION){
					//sp_calculateTranslation(transformation, &totalOffsetX, &totalOffsetY, gametick);
					//TODO: Implement asset translation in space, instead of fake sprite translation
					logger("[eng_renderAssets]: Translation transformation PENDING"); 
				}
			}
		}
		
		// Optimization: Use standard draw if effectively not rotated
		if (totalAngle % 360 != 0) {
			as_drawBitmapTransform(&actorSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)actorSprite->maskColor, totalAngle);
		} else {
			as_drawBitmap(&actorSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)actorSprite->maskColor);
		}
	}
}

