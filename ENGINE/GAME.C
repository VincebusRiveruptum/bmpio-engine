#include "GAME.H"
#include "MEM.H"

/*
    THIS MODULE HANDLES ALL GAME LOGIC & BUSSINES RULES
*/

unsigned char keyPressed = 0;
Asset *player = NULL;

Asset *gm_createAsset(Actor *actor, Coordinates *coordinates){
	Asset *newAsset = (Asset*)mem_arena_alloc(sceneArena, sizeof(Asset));

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
	newAsset->coordinates = coordinates;

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
			logger("\n[gm_setCurrentAction]: Action %d: '%s', type: %d, input: %d", (int)i, actor->actions[i]->name, (int)actor->actions[i]->type, (int)actionType);
			if(actor->actions[i]->type == actionType){
				actor->currentAction = actor->actions[i];
				logger("\n[gm_setCurrentAction]: Action set to %s", actor->actions[i]->name);
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
	if(keyboardTable[KEY_A] == true) gm_mainPlayerWalk(-16, 0, 0);
	if(keyboardTable[KEY_D] == true) gm_mainPlayerWalk(16, 0, 0);
	if(keyboardTable[KEY_W] == true) gm_mainPlayerWalk(0, -16, 0);
	if(keyboardTable[KEY_S] == true) gm_mainPlayerWalk(0, 16, 0);

	if(keyboardTable[KEY_SPACE] == true){
		gm_mainPlayerJump();
	}
}

// due to perfomance, we will assume that player is always there
void gm_mainPlayerJump(){
	gm_setCurrentAction(player->actor, GM_ACTION_JUMP);
}

void gm_mainPlayerWalk(int x, int y, int z){
    int oldVisX, oldVisY, oldVisZ;
    int newVisX, newVisY, newVisZ;

	gm_setCurrentAction(player->actor, GM_ACTION_WALK);

    // Track old grid position
    oldVisX = (int)(player->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    oldVisY = (int)(player->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    oldVisZ = (int)(player->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

	player->coordinates->x += x;
	player->coordinates->y += y;
	player->coordinates->z += z;

    // Track new grid position
    newVisX = (int)(player->coordinates->x / SP_GRID_SCALE) + SP_GRID_HALF;
    newVisY = (int)(player->coordinates->y / SP_GRID_SCALE) + SP_GRID_HALF;
    newVisZ = (int)(player->coordinates->z / SP_GRID_SCALE) + SP_GRID_HALF;

    // If we crossed a grid boundary, update the visibility grid
    if(oldVisX != newVisX || oldVisY != newVisY || oldVisZ != newVisZ){
        // TODO: This requires a search in the old list to remove the asset. 
        // For now, we will re-init the cameras because the list is small.
        sp_initCameras(); 
    }
}


void gm_cameraMove(int x, int y, int z){

	globalCamera->prevPos->x = globalCamera->position->x;
	globalCamera->prevPos->y = globalCamera->position->y;
	globalCamera->prevPos->z = globalCamera->position->z;

	globalCamera->position->x += x;
	globalCamera->position->y += y;
	globalCamera->position->z += z;
}