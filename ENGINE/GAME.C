#include "GAME.H"

/*
    THIS MODULE HANDLES ALL GAME LOGIC & BUSSINES RULES
*/

unsigned char keyPressed = 0;
Asset *player = NULL;

Asset *gm_createAsset(Actor *actor, Coordinates *coordinates){
	Asset *newAsset = (Asset*)calloc(1, sizeof(Asset));

	if(!newAsset){
		logger("\n[gm_createAsset]: Error: Could not allocate memory for asset");
		return NULL;
	}
    
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
	if(!asset){
		logger("\n[gm_destroyAsset]: Error: Asset is NULL");
		return;
	}
	free(asset);
}

/* ACTOR METHODS ===========================================================================*/
Action *gm_createAction(char *name, unsigned char type, Animation *animation, void (*update)(struct Asset *self)){
	Action *newAction = (Action*)calloc(1, sizeof(Action));
    if (!newAction) return NULL;
	strncpy(newAction->name, name, 31);
	newAction->type = type;
	newAction->animation = animation;
	newAction->update = update;
	return newAction;
}

Stats *gm_createStats(int health, int maxHealth, int attack, int defense, int speed){
	Stats *newStats = (Stats*)calloc(1, sizeof(Stats));
    if (!newStats) return NULL;
	newStats->health = health;
	newStats->maxHealth = maxHealth;
	newStats->attack = attack;
	newStats->defense = defense;
	newStats->speed = speed;
	return newStats;
}

Actor *gm_createActor(char *name, char *description, Stats *stats, Action *actions[]){
	int i;	
	Actor *newActor = (Actor*)calloc(1, sizeof(Actor));
	Action *genericAction;
    if (!newActor) return NULL;
	
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
        logger("\n[gm_setCurrentAction]: Error: Actor is NULL");
        return false;
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
	keyPressed = listenKeys();

	if(keyPressed){
		logger("[gm_kbdInput]: Key pressed: %c, %d", keyPressed, keyPressed);
	}

	if(keyPressed == SPACE_KEY){
		gm_mainPlayerJump();
	}
}

void gm_mainPlayerJump(){
	if (!player){
		logger("\n[gm_mainPlayerJump]: Error: Player is NULL");
		return;
	}
	logger("\n[gm_mainPlayerJump]: Player jumped");
	gm_setCurrentAction(player->actor, GM_ACTION_JUMP);
}