#include "SPACE.H"
#include "GAME.H"

struct Asset *visGrid[SP_GRID_SIZE][SP_GRID_SIZE][SP_GRID_SIZE];

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

