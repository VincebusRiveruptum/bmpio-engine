#include "TEST.H"

const char *renamonJumpingFrames[] = {
    "..\\assets\\jump\\FRAME1.bmp",
    "..\\assets\\jump\\FRAME2.bmp",
    "..\\assets\\jump\\FRAME3.bmp",
    "..\\assets\\jump\\FRAME4.bmp",
    "..\\assets\\jump\\FRAME5.bmp",
    "..\\assets\\jump\\FRAME6.bmp",
    "..\\assets\\jump\\FRAME7.bmp",
    "..\\assets\\jump\\FRAME8.bmp",
    "..\\assets\\jump\\FRAME9.bmp",
    "..\\assets\\jump\\FRAME10.bmp",
    "..\\assets\\jump\\FRAME11.bmp",
    "..\\assets\\jump\\FRAME12.bmp",
    "..\\assets\\jump\\FRAME13.bmp",
    "..\\assets\\jump\\FRAME14.bmp",
    "..\\assets\\jump\\FRAME15.bmp",
    NULL
};
const char *renamonIdleFrames[] = {
    "..\\assets\\idle\\FRAME1.bmp",
    NULL
};

struct Color *testPalette;

bool t_initTests(){

    t_createRenamon();

    logger("\n[engine/test/t_initTests]: Testing initialization");

    return true;
}

void t_createRenamon(){
    char *assetsPath = (char*)getEnv("ASSETS_PATH");
    char renamonPath[128] = {0};
    Action *renamonActions[2] = {NULL, NULL};
    BMPfile *renamonFile = NULL;
    Animation *renamonJumping = NULL;  
    Animation *renamonIdle = NULL;    

    Actor *renamonActor = NULL;
    
    sprintf(&renamonPath, "%s\\jump\\FRAME1.bmp", assetsPath);
    
    renamonFile = as_loadBMPfile(renamonPath);
    
    renamonIdle = as_createAnimation(); 
    as_loadAnimationFrames(renamonIdle, renamonIdleFrames);
    
    renamonJumping = as_createAnimation(); 
    as_loadAnimationFrames(renamonJumping, renamonJumpingFrames);
    
    renamonActions[0] = gm_createAction(
        "Idle", 
        GM_ACTION_IDLE,
        renamonIdle,
        NULL
    );

    renamonActions[1] = gm_createAction(
        "Jump", 
        GM_ACTION_JUMP,
        renamonJumping,
        NULL
    );
    // 'Renamon' actor
    renamonActor = gm_createActor(
        "Renamon", 
        "A hot furry digimon, yellow fox-like", 
        gm_createStats(100, 100, 10, 10, 10),
        renamonActions
    );
    
    gm_createAsset(
        renamonActor,
        sp_createCoordinates(0, 0, 0)
    );
   
    testPalette = renamonFile->bmpData->palette;   
    
}