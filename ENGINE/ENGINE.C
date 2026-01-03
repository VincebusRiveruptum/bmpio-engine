
#include "ENGINE.H"
#include "MEM.H"

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
	Node *actorSpriteNode = NULL;
	Sprite *actorSprite = NULL;
	ScreenCoordinates *screenPos = NULL;
	Asset *asset = NULL;
	Actor *actor = NULL;
	Action *action = NULL;
	Animation *actionAnimation = NULL;
	
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
	
    mem_arena_reset(frameArena);
	
	//logger("[eng_renderFrame]: Starting render, scanning %d slots", SP_GRID_SIZE);
	
	for(i = 0; i < SP_GRID_SIZE; i++){
		if(renderQueue[i] == NULL){
			continue;
		}
		
		//logger("[eng_renderFrame]: Found asset at queue slot %ld", i);
		actor = renderQueue[i]->actor;

		if(actor == NULL){
			logger("[eng_renderFrame]:Render queue %ld actor is NULL", i);
			continue;
		}

		action = actor->currentAction;

		if(action == NULL){
			logger("[eng_renderFrame]:Render queue %ld actor currentAction is NULL", i);
			continue;
		}

		actionAnimation = action->animation;

		if(actionAnimation == NULL){
			logger("[eng_renderFrame]:Render queue %ld actor currentAction animation is NULL", i);
			continue;
		}
		if(actionAnimation->length == 0){
			logger("[eng_renderFrame]:Render queue %ld actor currentAction animation length is 0", i);
			continue;
		}

		frameToRender = gametick % actionAnimation->length;
		actorSpriteNode = getNodeByIndex(&(actionAnimation->frames), (int)frameToRender);
		
		if(actorSpriteNode == NULL){
			logger("[eng_renderFrame]:Actor sprite node %ld is NULL", frameToRender);
			continue;
		}
		
		actorSprite = (Sprite *)actorSpriteNode->data;
		
		if(actorSprite == NULL){
			logger("[eng_renderFrame]:Actor sprite data %ld is NULL", frameToRender);
			continue;
		}
		
		totalAngle = 0;
		
		/* Project world coordinates to screen coordinates via macro (zero overhead) */
        SP_WORLD_TO_SCREEN(renderQueue[i]->coordinates->x, renderQueue[i]->coordinates->y, 
                           globalCamera->position->x, globalCamera->position->y, 
                           globalCamera->resolution->x, globalCamera->resolution->y, 
                           totalOffsetX, totalOffsetY);

		if(!sp_isInFrustrum(totalOffsetX, totalOffsetY, actorSprite)){
			continue;
		}
		
		if(actionAnimation->transformationList && actionAnimation->transformationList->length > 0){
			transformationLength = actionAnimation->transformationList->length;
			
			for(transformationIndex = 0; transformationIndex < transformationLength; transformationIndex++){
				Node *node = getNodeByIndex(&(actionAnimation->transformationList), transformationIndex);
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

