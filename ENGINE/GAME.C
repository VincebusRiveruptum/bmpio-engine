#include "GAME.H"

/*
    THIS MODULE HANDLES ALL GAME LOGIC & BUSSINES RULES
*/

Asset *gm_createAsset(Actor *actor, Coordinates *coordinates){
	Asset *newAsset = (Asset*)malloc(sizeof(Asset));

	if(!coordinates){
		newAsset->coordinates = sp_createCoordinates(0, 0, 0);
	}
    
    if(!actor){
        logger("\n[engine/game/createAsset]: Error: Actor is NULL");
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
    unsigned int vis_x = (int)(asset->coordinates->x + SP_GRID_HALF);
    unsigned int vis_y = (int)(asset->coordinates->y + SP_GRID_HALF);
    unsigned int vis_z = (int)(asset->coordinates->z + SP_GRID_HALF);
	if(!asset){
		logger("\n[engine/game/insertAsset]: Error: Asset is NULL");
		return;
	}

	visGrid[vis_x][vis_y][vis_z] = asset;
}

Asset *gm_getAssetByIndex(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z){
	if(vis_x >= SP_GRID_SIZE || vis_y >= SP_GRID_SIZE || vis_z >= SP_GRID_SIZE){
		logger("\n[engine/game/getAssetByIndex]: Error: Index out of bounds");
		return NULL;
	}
	return visGrid[vis_x][vis_y][vis_z];
}

void gm_destroyAsset(Asset *asset){
	if(!asset){
		logger("\n[engine/game/destroyAsset]: Error: Asset is NULL");
		return;
	}
	free(asset);
}

/* ACTOR METHODS ===========================================================================*/
Action *gm_createAction(char *name, unsigned char type, Animation *animation, void (*update)(struct Asset *self)){
	Action *newAction = (Action*)malloc(sizeof(Action));
	sprintf(newAction->name, name);
	newAction->type = type;
	newAction->animation = animation;
	newAction->update = update;
	return newAction;
}

Stats *gm_createStats(int health, int maxHealth, int attack, int defense, int speed){
	Stats *newStats = (Stats*)malloc(sizeof(Stats));
	newStats->health = health;
	newStats->maxHealth = maxHealth;
	newStats->attack = attack;
	newStats->defense = defense;
	newStats->speed = speed;
	return newStats;
}

Actor *gm_createActor(char *name, char *description, Stats *stats, Action *actions[]){
	Actor *newActor = (Actor*)malloc(sizeof(Actor));
	
    sprintf(newActor->name, name);
    sprintf(newActor->description, description);

    newActor->stats = stats;
    newActor->currentAction = NULL;

    if(!actions){
        logger("[gm_createActor]: No actions provided");
        return NULL;
    }else{
        memset(newActor->actions, 0, sizeof(newActor->actions));
        memcpy(newActor->actions, actions, sizeof(newActor->actions));
        newActor->actionLength = sizeof(actions)/sizeof(actions[0]);
    }
    
    if(!stats){
        newActor->stats = gm_createStats(100, 100, 10, 10, 10);
    }
	return newActor;
}