#include "GAME.H"

/*
    THIS MODULE HANDLES ALL GAME LOGIC & BUSSINES RULES
*/

Asset *createAsset(Actor *actor, Coordinates *coordinates){
	Asset *newAsset = (Asset*)malloc(sizeof(Asset));

	if(!coordinates){
		newAsset->coordinates = createCoordinates(0, 0, 0);
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

void insertAsset(Asset *asset){
    unsigned int vis_x = (int)(asset->coordinates->x + SP_GRID_HALF);
    unsigned int vis_y = (int)(asset->coordinates->y + SP_GRID_HALF);
    unsigned int vis_z = (int)(asset->coordinates->z + SP_GRID_HALF);
	if(!asset){
		logger("\n[engine/game/insertAsset]: Error: Asset is NULL");
		return;
	}

	visGrid[vis_x][vis_y][vis_z] = asset;
}

Asset *getAssetByIndex(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z){
	if(vis_x >= SP_GRID_SIZE || vis_y >= SP_GRID_SIZE || vis_z >= SP_GRID_SIZE){
		logger("\n[engine/game/getAssetByIndex]: Error: Index out of bounds");
		return NULL;
	}
	return visGrid[vis_x][vis_y][vis_z];
}

void destroyAsset(Asset *asset){
	if(!asset){
		logger("\n[engine/game/destroyAsset]: Error: Asset is NULL");
		return;
	}
	free(asset);
}