#include "SPACE.H"
#include "GAME.H"
#include "MEM.H"

struct List *visGrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];
struct Asset *renderQueue[SP_MAX_RENDER_ASSETS] = {NULL};
struct Camera *globalCamera;
struct Camera *cameras[SP_GRID_SIZE];
 
Coordinates *sp_createCoordinates(long x, long y, int z){
	Coordinates *newCoordinates = (Coordinates*)mem_arena_alloc(sceneArena, sizeof(Coordinates));
    if(newCoordinates){
        newCoordinates->x = x;
        newCoordinates->y = y;
        newCoordinates->z = z;
    }
	return newCoordinates;
}

ScreenCoordinates *sp_createScreenCoordinates(unsigned int x, unsigned int y){
	ScreenCoordinates *newScreenCoordinates = (ScreenCoordinates*)mem_arena_alloc(sceneArena, sizeof(ScreenCoordinates));
    if(newScreenCoordinates){
        newScreenCoordinates->x = x;
        newScreenCoordinates->y = y;
    }
	return newScreenCoordinates;
}
// Helper Functions ============================================================================================
void sp_calculateTranslation(Transformation *transformation, long *totalOffsetX, long *totalOffsetY, unsigned long gametick){
	TranslationTransformation *translation = (TranslationTransformation *)transformation->data;

	if (translation != NULL && translation->dest != NULL) {
		if(*totalOffsetX < translation->dest->x){
			if(translation->loop == true){
				*totalOffsetX = *totalOffsetX + ((gametick | 1) % translation->dest->x);
			} else {
				*totalOffsetX++;
			}
		}

		if(*totalOffsetX > translation->dest->x){
			if(translation->loop == true){
				*totalOffsetX = *totalOffsetX - ((gametick | 1) % translation->dest->x);
			} else {
				*totalOffsetX--;
			}
		}

		if(*totalOffsetY < translation->dest->y){
			if(translation->loop == true){
				*totalOffsetY = *totalOffsetY + ((gametick | 1) % translation->dest->y);
			} else {
				*totalOffsetY++;
			}
		}

		if(*totalOffsetY > translation->dest->y){
			if(translation->loop == true){
				*totalOffsetY = *totalOffsetY - ((gametick | 1) % translation->dest->y);
			} else {
				*totalOffsetY--;
			}
		}

	} else {
			logger("\nError: Translation data or dest is NULL");
	}	
}

// Culling =====================================================================================================

bool sp_addAssetToVisGrid(struct Asset *asset){
	if(!asset){
		logger("\nError: Asset is NULL");
		return false;
	}

	// Normalize coordinates to grid size and check bounds
	if(	asset->coordinates->x + SP_GRID_HALF < 0 ||
		asset->coordinates->x + SP_GRID_HALF > SP_GRID_SIZE ||
		asset->coordinates->y + SP_GRID_HALF < 0 ||
		asset->coordinates->y + SP_GRID_HALF > SP_GRID_SIZE ||
		asset->coordinates->z + SP_GRID_HALF < 0 ||
		asset->coordinates->z + SP_GRID_HALF > SP_GRID_SIZE){
		logger("\nError: Asset coordinates are out of bounds");
		return false;
	}

	addGenericNode(&visGrid[(int)(asset->coordinates->x + SP_GRID_HALF)][(int)(asset->coordinates->y + SP_GRID_HALF)][(int)(asset->coordinates->z + SP_GRID_HALF)], (void *)asset);
	return true;
}

bool sp_removeAssetFromVisGrid(unsigned char vis_x, unsigned char vis_y, unsigned char vis_z, unsigned int index){
	if(vis_x >= SP_GRID_SIZE || vis_y >= SP_GRID_SIZE || vis_z >= SP_GRID_SIZE){
		logger("\nError: Index out of bounds");
		return false;
	}

	if(visGrid[vis_x][vis_y][vis_z] == NULL){
		logger("\nError: Asset list is NULL");
		return false;
	}

	deleteNodeByIndex(&visGrid[vis_x][vis_y][vis_z], index);
	return true;
}

// CAMERA =====================================================================================================

void sp_calculateCameraBounds(Camera *camera){
    int camGridX, camGridY, camGridZ;
	if(!camera) return;

	camGridX = (int)(camera->position->x / SP_GRID_SCALE) + SP_GRID_HALF;
	camGridY = (int)(camera->position->y / SP_GRID_SCALE) + SP_GRID_HALF;
	camGridZ = (int)(camera->position->z / SP_GRID_SCALE) + SP_GRID_HALF;
	
	camera->gridMinX = (camGridX - SP_GRID_VIS_SIZE < 0) ? 0 : camGridX - SP_GRID_VIS_SIZE;
	camera->gridMinY = (camGridY - SP_GRID_VIS_SIZE < 0) ? 0 : camGridY - SP_GRID_VIS_SIZE;
	camera->gridMinZ = (camGridZ - SP_GRID_VIS_SIZE < 0) ? 0 : camGridZ - SP_GRID_VIS_SIZE;
	
	camera->gridMaxX = (camGridX + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridX + SP_GRID_VIS_SIZE;
	camera->gridMaxY = (camGridY + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridY + SP_GRID_VIS_SIZE;
	camera->gridMaxZ = (camGridZ + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridZ + SP_GRID_VIS_SIZE;
}

Camera *sp_createCamera(Coordinates *position, ScreenCoordinates *resolution){
	Camera *newCamera = NULL;
	
	if(!position || !resolution){
		logger("[sp_createCamera]: Error, Position or resolution is NULL");
		return NULL;
	}

	newCamera = (Camera*)malloc(sizeof(Camera));
	newCamera->position = position;
	newCamera->prevPos = NULL;
	newCamera->resolution = resolution;

    sp_calculateCameraBounds(newCamera);

	logger("[sp_createCamera]: Camera initialized with bounds [%d-%d][%d-%d][%d-%d]", 
		newCamera->gridMinX, newCamera->gridMaxX,
		newCamera->gridMinY, newCamera->gridMaxY,
		newCamera->gridMinZ, newCamera->gridMaxZ);
	return newCamera;
}

void sp_setGlobalCamera(Camera *camera){
	if(!camera){
		logger("[sp_setGlobalCamera]: Error, Camera is NULL");
		return;
	}
	globalCamera = camera;
}

void sp_destroyCamera(Camera *camera){
	if(!camera){
		logger("[sp_destroyCamera]: Error, Camera is NULL");
		return;
	}
	free(camera);
}

/* This will make the render queue  based on the global camera*/
void sp_initCameras(){
	int i = 0;
	int j = 0;
	int k = 0;
	int assetListLength = 0;
	int assetListIndex = 0;
    int qIndex = 0;
	List *assetList = NULL;
	Node *node = NULL;
	Asset *asset = NULL;

	if(!globalCamera){
		logger("[sp_initCameras]: Error, Global camera is NULL");
		return;
	}
	
	/* Clear render queue*/
	memset(renderQueue, 0, sizeof(renderQueue));
	
	/* Generate render queue based on global camera by scanning only the visible region of the visGrid */
	for(i = globalCamera->gridMinX; i <= globalCamera->gridMaxX; i++){
		for(j = globalCamera->gridMinY; j <= globalCamera->gridMaxY; j++){
			for(k = globalCamera->gridMinZ; k <= globalCamera->gridMaxZ; k++){
				if(visGrid[i][j][k] == NULL){
					continue;
				}

				assetList = visGrid[i][j][k];

				if(assetList == NULL){
					continue;
				}
				
				node = assetList->firstNode;

				while(node != NULL){
                    asset = (Asset*)node->data;
					
					asset->vis_prevX = asset->vis_currentX;
					asset->vis_prevY = asset->vis_currentY;
					asset->vis_prevZ = asset->vis_currentZ;
	
					asset->vis_currentX = i;
					asset->vis_currentY = j;
					asset->vis_currentZ = k;
					
                    if(qIndex < SP_MAX_RENDER_ASSETS){
					    renderQueue[qIndex] = asset;
					    qIndex++;
                    }
					node = node->next;
				}
			}
		}
	}

	sp_renderQueueApplyZOrdering(qIndex);
}

void sp_checkCameras(){
	if(!globalCamera){
		return;
	}

	if(globalCamera->prevPos == NULL){
		globalCamera->prevPos = sp_createCoordinates(globalCamera->position->x, globalCamera->position->y, globalCamera->position->z);
	}	
	
	if(globalCamera->position->x != globalCamera->prevPos->x || globalCamera->position->y != globalCamera->prevPos->y || globalCamera->position->z != globalCamera->prevPos->z){
		// Recalculate bounds since camera moved
        sp_calculateCameraBounds(globalCamera);
        sp_initCameras();

		globalCamera->prevPos->x = globalCamera->position->x;
		globalCamera->prevPos->y = globalCamera->position->y;
		globalCamera->prevPos->z = globalCamera->position->z;
	}
}




bool sp_isInFrustrum(int screenX, int screenY, struct Sprite *sprite){
	int spriteWidth, spriteHeight;
	if(!sprite || !sprite->bmpData){
		logger("[sp_isInFrustrum]: Error, sprite or bmpData is NULL");
		return false;
	}

	spriteWidth = (int)sprite->bmpData->width;
	spriteHeight = (int)sprite->bmpData->height;

    /* Treat screenX/Y as CENTER */
	if(
		screenX + (spriteWidth / 2) < 0 ||
		screenX - (spriteWidth / 2) > VID_WIDTH ||
		screenY + (spriteHeight / 2) < 0 ||
		screenY - (spriteHeight / 2) > VID_HEIGHT
	) return false;
	
	return true;
}

void sp_renderQueueApplyZOrdering(int length){
	int i, j;
	Asset *temp;
	
	if (!renderQueue || length <= 0){
		return;
	}

	/* Insertion sort algorithm limited to active assets */
	for (i = 0; i < length; i++){
		if (renderQueue[i] == NULL){
			continue;
		}		
		j = i;		
		while(
			j > 0 && 
			renderQueue[j] != NULL && 
			renderQueue[j - 1] != NULL && 
			renderQueue[j]->coordinates->z < renderQueue[j - 1]->coordinates->z){
			
			temp = renderQueue[j];
			renderQueue[j] = renderQueue[j - 1];
			renderQueue[j - 1] = temp;
			j--;
		}
	}
}
