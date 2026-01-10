#include "GAME.H"
#include "MEM.H"
#include "SPACE.H"
#include "ASSETS.H"

/*
    THIS MODULE HANDLES ALL GAME LOGIC & BUSSINES RULES
*/

unsigned char keyPressed = 0;
Asset *player = NULL;

Asset *gm_createAsset(Actor *actor, Shape *shape, Coordinates *coordinates){
	Asset *newAsset = (Asset*)mem_arena_alloc(sceneArena, sizeof(Asset));
	Coordinates *pointingTo = (Coordinates*)mem_arena_alloc(sceneArena, sizeof(Coordinates));

	if(!newAsset){
		logger("\n[gm_createAsset]: Error: Could not allocate memory for asset");
		return NULL;
	}

    memset(newAsset, 0, sizeof(Asset));
    
    if(!actor){
        logger("\n[gm_createAsset]: Error: Actor is NULL");
        return NULL;
	}

	newAsset->actor = actor;
	newAsset->shape = shape; 
	
	newAsset->coordinates = coordinates;
	newAsset->pointingTo = pointingTo;			// pointing to nothing for now

	newAsset->pointingTo->x = coordinates->x;
	newAsset->pointingTo->y = coordinates->y;
	newAsset->pointingTo->z = coordinates->z;

	newAsset->vis_prevX = 0;
	newAsset->vis_prevY = 0;
	newAsset->vis_prevZ = 0;
	newAsset->vis_currentX = 0;
	newAsset->vis_currentY = 0;
	newAsset->vis_currentZ = 0;

	return newAsset;
}

void gm_insertAsset(Asset *asset){
    unsigned int vis_x;
    unsigned int vis_y;
    unsigned int vis_z;
    
	if(!asset){
		logger("\n[gm_insertAsset]: Error: Asset is NULL");
		return;
	}

    vis_x = (int)(asset->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    vis_y = (int)(asset->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    vis_z = (int)(asset->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

    logger("[gm_insertAsset]: Inserting asset at %d, %d, %d", vis_x, vis_y, vis_z);
   
	addGenericNode(&visGrid[vis_x][vis_y][vis_z], (void *)asset);
	
}

Asset *gm_getAssetByIndex(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z, unsigned int index){
    Node *node = NULL;
	if(vis_x >= SP_GRID_SIZE || vis_y >= SP_GRID_SIZE || vis_z >= SP_GRID_SIZE){
		logger("\n[gm_getAssetByIndex]: Error: Index out of bounds");
		return NULL;
	}
    
    node = getNodeByIndex(&visGrid[vis_x][vis_y][vis_z], index);
    if(node) return (Asset *)node->data;
    
	return NULL;
}

void gm_destroyAsset(Asset *asset){
    // Individual assets are not freed when using Arena allocation.
    // They are released when sceneArena is reset.
}

/* ACTOR METHODS ===========================================================================*/
Action *gm_createAction(char *name, unsigned char type, Animation *animation, void (*update)(struct Asset *self)){
	Action *newAction = (Action*)mem_arena_alloc(sceneArena, sizeof(Action));
    if (!newAction) return NULL;
    memset(newAction, 0, sizeof(Action));
	strncpy(newAction->name, name, 31);
	newAction->type = type;
	newAction->animation = animation;
	newAction->update = update;
	return newAction;
}

Stats *gm_createStats(int health, int maxHealth, int attack, int defense, int speed){
	Stats *newStats = (Stats*)mem_arena_alloc(sceneArena, sizeof(Stats));
    if (!newStats) return NULL;
    memset(newStats, 0, sizeof(Stats));
	newStats->health = health;
	newStats->maxHealth = maxHealth;
	newStats->attack = attack;
	newStats->defense = defense;
	newStats->speed = speed;
	return newStats;
}

Actor *gm_createActor(char *name, char *description, Stats *stats, Action *actions[]){
	int i;	
	Actor *newActor = (Actor*)mem_arena_alloc(sceneArena, sizeof(Actor));
	Action *genericAction;
    if (!newActor) return NULL;
    memset(newActor, 0, sizeof(Actor));
	
    strncpy(newActor->name, name, 31);
    strncpy(newActor->description, description, 255);

    newActor->stats = stats;
    newActor->currentAction = NULL;
	

    if(!actions){
        logger("[gm_createActor]: No actions provided, generic one assigned instead");

		genericAction = gm_createAction("Generic", GM_ACTION_DEFAULT, NULL, NULL);
		newActor->actions[0] = genericAction;

		// TODO: This is a temporary fix, should be refactored because it is not including
		// a default animation to the generic action, making it not renderable.
    }else{	
		for(i = 0; i < GM_MAX_ACTIONS; i++){
			newActor->actions[i] = actions[i];
			if (actions[i]) {
				logger("[gm_createActor]: Assigned action %d: %s (type %d)", i, actions[i]->name, (int)actions[i]->type);
			}
		}
	}
	
	// Set first action as default
	if(newActor->actions[0] != NULL){
		newActor->currentAction = newActor->actions[0];
	}

    if(!stats){
        newActor->stats = gm_createStats(100, 100, 10, 10, 10);
    }
	return newActor;
}

bool gm_setCurrentAction(Actor *actor, unsigned char actionType){
	Action *action;
	int i = 0;
    if(!actor){
        return false;
    }

    if(actor->currentAction && actor->currentAction->type == actionType){
        return true;
    }
    
    for(i = 0; i < GM_MAX_ACTIONS; i++){
		if(actor->actions[i]){
			if(actor->actions[i]->type == actionType){
				actor->currentAction = actor->actions[i];
				return true;
			}
		}
	}
    return false;
}

void gm_listenEvents(){
	// Listen to events such as
	// Key presses
	// Mouse clicks
	// Gamepad inputs
	// Environment events

	gm_kbdInput();
}

void gm_checkCollisions(Asset *asset){
	// gm_checkCollisions() will be in eng_renderFrame loop because it
	// is the main place we have access to the camera's visGrid and assets
	// that are in the renderQueue.
	unsigned int i;
	Asset *otherAsset;
	Shape *hitBox;
	Box *aBox;
	Box *oBox;
	long aLeft;
	long aRight;
	long aTop;
	long aBottom;
	long oLeft;
	long oRight;
	long oTop;
	long oBottom;
	
	// Always clear collisions at the start of the check
	gm_clearCollisions(asset);

	// If current actor has no hitbox, skip it
	if(asset->shape == NULL){
		return;
	}

	// Check for collisions if the actor is moving
	if( asset->actor->currentAction->type == GM_ACTION_WALK ||
		asset->actor->currentAction->type == GM_ACTION_RUN ||
		asset->actor->currentAction->type == GM_ACTION_JUMP
	){
		for(i = 0; i < SP_MAX_RENDER_ASSETS; i++){
			if(renderQueue[i] == NULL || renderQueue[i]->actor == NULL ){
				continue;
			}
			
			otherAsset = renderQueue[i];

			if(asset == otherAsset || otherAsset->shape == NULL){
				continue;
			}

			// IF HITBOX IS A BOX SHAPE
			if(	asset->shape->type == GM_SHAPE_TYPE_BOX &&
				asset->shape->shapeObject != NULL &&
				otherAsset->shape->type == GM_SHAPE_TYPE_BOX &&
				otherAsset->shape->shapeObject != NULL){
				
				aBox = (Box*)asset->shape->shapeObject;
				oBox = (Box*)otherAsset->shape->shapeObject;

				aLeft = asset->coordinates->x - (aBox->width >> 1);
				aRight = asset->coordinates->x + (aBox->width >> 1);
				aTop = asset->coordinates->y - (aBox->height >> 1);
				aBottom = asset->coordinates->y + (aBox->height >> 1);

				oLeft = otherAsset->coordinates->x - (oBox->width >> 1);
				oRight = otherAsset->coordinates->x + (oBox->width >> 1);
				oTop = otherAsset->coordinates->y - (oBox->height >> 1);
				oBottom = otherAsset->coordinates->y + (oBox->height >> 1);

				if( aLeft < oRight && aRight > oLeft &&
					aTop < oBottom && aBottom > oTop){
					
					gm_addCollisions(asset, otherAsset);
				}
			}
		}
	}
}

void gm_bounceBack(Asset *asset, int prevX, int prevY, int prevZ){
	asset->coordinates->x -= prevX;
	asset->coordinates->y -= prevY;
	asset->coordinates->z -= prevZ;
}

void gm_addCollisions(Asset *asset, Asset *otherAsset){
	unsigned char i;
	for(i = 0; i < MAX_COLLISIONS; i++){
		if(asset->collisions[i] == NULL){
			asset->collisions[i] = otherAsset;
			return;
		}
	}
}

void gm_clearCollisions(Asset *asset){
	memset(asset->collisions, 0, sizeof(asset->collisions));
}

bool gm_isColliding(Asset *asset){
	unsigned char i;
	for(i = 0; i < MAX_COLLISIONS; i++){
		if(asset->collisions[i] != NULL){
			return true;
		}
	}
	return false;
}

void gm_kbdInput(){
	// Camera movement (LSHIFT + WASD)
    if(keyboardTable[KEY_LSHIFT] == true){
        if(keyboardTable[KEY_A] == true) gm_cameraMove(-16, 0, 0);
        if(keyboardTable[KEY_D] == true) gm_cameraMove(16, 0, 0);
        if(keyboardTable[KEY_W] == true) gm_cameraMove(0, -16, 0);
        if(keyboardTable[KEY_S] == true) gm_cameraMove(0, 16, 0);
        return; // Don't move player if camera is moving
    }

    // Player movement (WASD)
	if(keyboardTable[KEY_A] == true) {
		gm_mainPlayerWalk(-16, 0, 0);
	}else if(keyboardTable[KEY_D] == true){
		gm_mainPlayerWalk(16, 0, 0);
	}else if(keyboardTable[KEY_W] == true){
		gm_mainPlayerWalk(0, -16, 0);
	}else if(keyboardTable[KEY_S] == true){
		gm_mainPlayerWalk(0, 16, 0);
	}else{
		gm_setCurrentAction(player->actor, GM_ACTION_IDLE);
	}

	if(keyboardTable[KEY_SPACE] == true) gm_mainPlayerJump();
}

// due to perfomance, we will assume that player is always there
void gm_mainPlayerJump(){
	gm_setCurrentAction(player->actor, GM_ACTION_JUMP);
}

void gm_mainPlayerWalk(int x, int y, int z){
    int oldVisX, oldVisY, oldVisZ;
    int newVisX, newVisY, newVisZ;

	gm_setCurrentAction(player->actor, GM_ACTION_RUN);

    // Track old grid position
    oldVisX = (int)(player->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    oldVisY = (int)(player->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    oldVisZ = (int)(player->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

	player->coordinates->x += x;
	player->coordinates->y += y;
	player->coordinates->z += z;

	// Check if the NEW position is colliding
	gm_checkCollisions(player);
	if(gm_isColliding(player)){
		// Revert to old position
		gm_bounceBack(player, x, y, z);
		gm_setCurrentAction(player->actor, GM_ACTION_IDLE);
		return;
	}

    // Track new grid position
    newVisX = (int)(player->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    newVisY = (int)(player->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    newVisZ = (int)(player->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

	// Flip character if moving left
	if(x < 0){
		player->pointingTo->x = player->coordinates->x - 1;
	}else{
		player->pointingTo->x = player->coordinates->x + 1;
	}

    // If we crossed a grid boundary, update the visibility grid
    if(oldVisX != newVisX || oldVisY != newVisY || oldVisZ != newVisZ){
        // TODO: This requires a search in the old list to remove the asset. 
        // For now, we will re-init the cameras because the list is small.
        sp_initCameras(); 
    }
}

void gm_mainPlayerIdle(){
	gm_setCurrentAction(player->actor, GM_ACTION_IDLE);
}	

void gm_cameraMove(int x, int y, int z){

	globalCamera->prevPos->x = globalCamera->position->x;
	globalCamera->prevPos->y = globalCamera->position->y;
	globalCamera->prevPos->z = globalCamera->position->z;

	globalCamera->position->x += x;
	globalCamera->position->y += y;
	globalCamera->position->z += z;
}