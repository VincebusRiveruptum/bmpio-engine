#include "SPACE.H"
#include "GAME.H"

struct Asset *visGrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];
struct Asset *renderQueue[SP_GRID_SIZE] = {NULL};
struct Camera *globalCamera;
struct Camera *cameras[SP_GRID_SIZE];
 
Coordinates *sp_createCoordinates(long x, long y, int z){
	Coordinates *newCoordinates = (Coordinates*)malloc(sizeof(Coordinates));
	newCoordinates->x = x;
	newCoordinates->y = y;
	newCoordinates->z = z;
	return newCoordinates;
}

ScreenCoordinates *sp_createScreenCoordinates(unsigned int x, unsigned int y){
	ScreenCoordinates *newScreenCoordinates = (ScreenCoordinates*)malloc(sizeof(ScreenCoordinates));
	newScreenCoordinates->x = x;
	newScreenCoordinates->y = y;
	return newScreenCoordinates;
}

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
	if(asset->coordinates->x + SP_GRID_HALF < 0 || asset->coordinates->x + SP_GRID_HALF > SP_GRID_SIZE || asset->coordinates->y + SP_GRID_HALF < 0 || asset->coordinates->y + SP_GRID_HALF > SP_GRID_SIZE || asset->coordinates->z + SP_GRID_HALF < 0 || asset->coordinates->z + SP_GRID_HALF > SP_GRID_SIZE){
		logger("\nError: Asset coordinates are out of bounds");
		return false;
	}

	visGrid[(asset->coordinates->x + SP_GRID_HALF)][(asset->coordinates->y + SP_GRID_HALF)][(asset->coordinates->z + SP_GRID_HALF)] = asset;
	return true;
}

bool sp_removeAssetFromVisGrid(struct Asset *asset){
	if(!asset){
		logger("\nError: Asset is NULL");
		return false;
	}

	visGrid[asset->coordinates->x + SP_GRID_HALF][asset->coordinates->y + SP_GRID_HALF][asset->coordinates->z + SP_GRID_HALF] = NULL;
	return true;
}

// CAMERA =====================================================================================================

Camera *sp_createCamera(Coordinates *position, ScreenCoordinates *resolution){
	Camera *newCamera = NULL;
	int camGridX = 0;
	int camGridY = 0;
	int camGridZ = 0;
	
	if(!position || !resolution){
		logger("[sp_createCamera]: Error, Position or resolution is NULL");
		return NULL;
	}

	newCamera = (Camera*)malloc(sizeof(Camera));
	newCamera->position = position;
	newCamera->prevPos = NULL;
	newCamera->resolution = resolution;

	// Calculate grid position of camera
	camGridX = (int)(position->x / SP_GRID_SCALE) + SP_GRID_HALF;
	camGridY = (int)(position->y / SP_GRID_SCALE) + SP_GRID_HALF;
	camGridZ = (int)(position->z / SP_GRID_SCALE) + SP_GRID_HALF;
	
	// Set bounds to 1 unit around camera position
	newCamera->gridMinX = (camGridX - SP_GRID_VIS_SIZE < 0) ? 0 : camGridX - SP_GRID_VIS_SIZE;
	newCamera->gridMinY = (camGridY - SP_GRID_VIS_SIZE < 0) ? 0 : camGridY - SP_GRID_VIS_SIZE;
	newCamera->gridMinZ = (camGridZ - SP_GRID_VIS_SIZE < 0) ? 0 : camGridZ - SP_GRID_VIS_SIZE;
	
	newCamera->gridMaxX = (camGridX + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridX + SP_GRID_VIS_SIZE;
	newCamera->gridMaxY = (camGridY + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridY + SP_GRID_VIS_SIZE;
	newCamera->gridMaxZ = (camGridZ + SP_GRID_VIS_SIZE >= SP_GRID_SIZE) ? SP_GRID_SIZE - 1 : camGridZ + SP_GRID_VIS_SIZE;
	
	logger("[sp_createCamera]: Camera at grid [%d][%d][%d], bounds [%d-%d][%d-%d][%d-%d]", 
		camGridX, camGridY, camGridZ,
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
	int w = 0;
	Asset *asset = NULL;

	if(!globalCamera){
		logger("[sp_initCameras]: Error, Global camera is NULL");
		return;
	}
	
	/* Clear render queue*/
	memset(renderQueue, 0, sizeof(renderQueue));
	
	/* Generate render queue based on global camera by scanning the visGrid*/
	for(i = 0; i < SP_GRID_SIZE; i++){
		for(j = 0; j < SP_GRID_SIZE; j++){
			for(k = 0; k < SP_GRID_SIZE; k++){
				if(visGrid[i][j][k] == NULL){
					continue;
				}

				if(i < globalCamera->gridMinX || i > globalCamera->gridMaxX || j < globalCamera->gridMinY || j > globalCamera->gridMaxY || k < globalCamera->gridMinZ || k > globalCamera->gridMaxZ){
					continue;
				}				

				if(w >= SP_GRID_SIZE){
					logger("[sp_initCameras]: Error, Render queue is full");
					return;
				}

				asset = visGrid[i][j][k];

				if(asset == NULL){
					logger("[sp_initCameras]: Error, Asset is NULL");
					continue;
				}
				
				asset->vis_prevX = asset->vis_currentX;
				asset->vis_prevY = asset->vis_currentY;
				asset->vis_prevZ = asset->vis_currentZ;

				asset->vis_currentX = i + SP_GRID_HALF;
				asset->vis_currentY = j + SP_GRID_HALF;
				asset->vis_currentZ = k + SP_GRID_HALF;
				
				renderQueue[w] = asset;
				w++;
			}
		}
	}
}

void sp_checkCameras(){
	if(!globalCamera){
		logger("[sp_checkCameras]: Error, Global camera is NULL");
		return;
	}

	if(globalCamera->prevPos == NULL){
		globalCamera->prevPos = sp_createCoordinates(0, 0, 0);
	}	
	
	if(globalCamera->position->x != globalCamera->prevPos->x || globalCamera->position->y != globalCamera->prevPos->y || globalCamera->position->z != globalCamera->prevPos->z){
		sp_initCameras();

		globalCamera->prevPos->x = globalCamera->position->x;
		globalCamera->prevPos->y = globalCamera->position->y;
		globalCamera->prevPos->z = globalCamera->position->z;
	}
}

/* Convert world coordinates to screen coordinates relative to camera */
ScreenCoordinates *sp_worldToScreen(Coordinates *worldPos, Camera *camera){
	ScreenCoordinates *screenPos = NULL;
	long offsetX, offsetY;
	
	if(!worldPos || !camera){
		logger("[sp_worldToScreen]: Error, worldPos or camera is NULL");
		return NULL;
	}
	
	screenPos = (ScreenCoordinates*)malloc(sizeof(ScreenCoordinates));
	
	/* Calculate offset from camera position */
	offsetX = worldPos->x - camera->position->x;
	offsetY = worldPos->y - camera->position->y;
	
	/* Center camera on screen and add offset */
	screenPos->x = (camera->resolution->x / 2) + offsetX;
	screenPos->y = (camera->resolution->y / 2) + offsetY;
	
	return screenPos;
}