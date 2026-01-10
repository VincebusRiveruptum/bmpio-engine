
#include "ENGINE.H"
#include "MEM.H"

Color *globalPalette = NULL;

unsigned long gameTicks = 0;
unsigned long index = 0;

bool hflip = false;
int transformationIndex = 0;
int totalAngle = 0;
unsigned long frameToRender = 0;
unsigned long i;
long totalOffsetX = 0;
long totalOffsetY = 0;

Sprite *actorSprite = NULL;
ScreenCoordinates *screenPos = NULL;
Asset *asset = NULL;
Shape *shape = NULL;
Actor *actor = NULL;
Action *action = NULL;
Animation *actionAnimation = NULL;
RotationTransformation *rot = NULL;
Transformation *transformation = NULL;

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

	if(renderQueue == NULL){
		logger("[eng_renderFrame]: Render queue is NULL");
		return;
	}
	
    mem_arena_reset(frameArena);
	
	//logger("[eng_renderFrame]: Starting render, scanning %d slots", SP_GRID_SIZE);
	
	for(i = 0; i < SP_MAX_RENDER_ASSETS; i++){
		
		
		if(renderQueue[i] == NULL){
			continue;
		}
		
		actor = renderQueue[i]->actor;
		
		if(actor){		
			
			// if the actor walks, it will check for collisions nearby
			gm_checkCollisions(renderQueue[i]);

			action = actor->currentAction;
			
			if(action == NULL){
				logger("[eng_renderFrame]:Render queue %ld actor currentAction is NULL", i);
				continue;
			}

			/* Project world coordinates to screen coordinates via macro (zero overhead) */
			SP_WORLD_TO_SCREEN(
				renderQueue[i]->coordinates->x, 
				renderQueue[i]->coordinates->y, 
				globalCamera->position->x, 
				globalCamera->position->y, 
				globalCamera->resolution->x,
				globalCamera->resolution->y, 
				totalOffsetX,
				totalOffsetY
			);
					
				shape = renderQueue[i]->shape;
				
				// First we render the shape, it could be a bounding box or something else
				if(shape){
					if(shape->type == GM_SHAPE_TYPE_BOX){
						if(sp_isShapeInFrustrum(totalOffsetX, totalOffsetY, shape)){
							as_drawBox(shape, totalOffsetX, totalOffsetY);
						}
					}
					// ...
				}

				actionAnimation = action->animation;

				frameToRender = gametick % actionAnimation->length;
				actorSprite = actionAnimation->frames[frameToRender];
				
				totalAngle = 0;
			
				if(actorSprite){
					if(!sp_isInFrustrum(totalOffsetX, totalOffsetY, actorSprite)){
						continue;
					}
					
					for(transformationIndex = 0; transformationIndex < GM_ANIMATION_MAX_TRANSFORMATIONS; transformationIndex++){
						transformation = actionAnimation->transformationList[transformationIndex];
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
							logger("[eng_renderFrame]: Translation transformation PENDING"); 
						}
					}
					
					hflip = (renderQueue[i]->pointingTo->x < renderQueue[i]->coordinates->x) ? true : false;
					
					// Optimization: Use standard draw if effectively not rotated
					if (totalAngle % 360 != 0) {
						as_drawBitmapTransform(&actorSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)actorSprite->maskColor, totalAngle, hflip);
					} else {
						as_drawBitmap(&actorSprite->bmpData, (unsigned int)totalOffsetX, (unsigned int)totalOffsetY, (int)actorSprite->maskColor, hflip);
					}
				}
			} else {
				logger("[eng_renderFrame]:Render queue %ld actor is NULL", i);
			}
	}
}
