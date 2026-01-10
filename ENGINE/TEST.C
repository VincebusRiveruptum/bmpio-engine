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
const char *renamonJumpingAltFrames[] = {
    "..\\assets\\jump2\\FRAME1.bmp",
    "..\\assets\\jump2\\FRAME2.bmp",
    "..\\assets\\jump2\\FRAME3.bmp",
    "..\\assets\\jump2\\FRAME4.bmp",
    "..\\assets\\jump2\\FRAME5.bmp",
    "..\\assets\\jump2\\FRAME6.bmp",
    "..\\assets\\jump2\\FRAME7.bmp",
    "..\\assets\\jump2\\FRAME8.bmp",
    "..\\assets\\jump2\\FRAME9.bmp",
    "..\\assets\\jump2\\FRAME10.bmp",
    "..\\assets\\jump2\\FRAME11.bmp",
    "..\\assets\\jump2\\FRAME12.bmp",
    NULL
};

const char *renamonRunningFrames[] = {
    "..\\assets\\run\\FRAME1.bmp",
    "..\\assets\\run\\FRAME2.bmp",
    "..\\assets\\run\\FRAME4.bmp",
    "..\\assets\\run\\FRAME5.bmp",
    "..\\assets\\run\\FRAME6.bmp",
    "..\\assets\\run\\FRAME11.bmp",
    NULL
};

const char *renamonSprintFrames[] = {
    "..\\assets\\sprint\\FRAME1.bmp",
    "..\\assets\\sprint\\FRAME2.bmp",
    "..\\assets\\sprint\\FRAME4.bmp",
    NULL
};

const char *renamonCrouchFrames[] = {
    "..\\assets\\crouch\\FRAME1.bmp",
    "..\\assets\\crouch\\FRAME2.bmp",
    "..\\assets\\crouch\\FRAME3.bmp",
    "..\\assets\\crouch\\FRAME4.bmp",
    "..\\assets\\crouch\\FRAME5.bmp",
    "..\\assets\\crouch\\FRAME6.bmp",
    "..\\assets\\crouch\\FRAME7.bmp",
    NULL
};

const char *renamonIdleFrames[] = {
    "..\\assets\\idle\\FRAME1.bmp",
    NULL
};

const char *skullFrames[] = {
    "..\\assets\\sk256.bmp",
    NULL
};

struct Color *testPalette;

bool t_initTests(){
    logger("\n[t_initTests`]: Testing initialization");

    t_createRenamon();
    t_skullBgTest();
    t_testFloor();

    return true;
}

void t_createRenamon(){
    char *assetsPath = (char*)getEnv("ASSETS_PATH");
    char renamonPath[128] = {0};
    Action *renamonActions[GM_MAX_ACTIONS] = {NULL};
    BMPfile *renamonFile = NULL;
    Animation *renamonJumping = NULL;  
    Animation *renamonIdle = NULL;    
    Animation *renamonRunning = NULL;
    Animation *renamonSprint = NULL;
    Animation *renamonCrouch = NULL;

    Actor *renamonActor = NULL;
    Asset *renamonAsset = NULL;

    sprintf(renamonPath, "%s\\jump\\FRAME1.bmp", assetsPath);
    
    renamonFile = as_loadBMPfile(renamonPath);
    if(!renamonFile){
        logger("[t_createRenamon]: Error: renamonFile is NULL");
        return;
    }
    
    /* Renamon's actions */
    renamonIdle = as_createAnimation(); 
    as_loadAnimationFrames(renamonIdle, renamonIdleFrames, 255);
    
    renamonRunning = as_createAnimation(); 
    as_loadAnimationFrames(renamonRunning, renamonRunningFrames, 255);
    
    renamonJumping = as_createAnimation(); 
    as_loadAnimationFrames(renamonJumping, renamonJumpingFrames, 255);
    
    renamonSprint = as_createAnimation(); 
    as_loadAnimationFrames(renamonSprint, renamonSprintFrames, 255);
    
    renamonCrouch = as_createAnimation(); 
    as_loadAnimationFrames(renamonCrouch, renamonCrouchFrames, 255);
    
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
    renamonActions[2] = gm_createAction(
        "Running", 
        GM_ACTION_RUN,
        renamonRunning,
        NULL
    );
    renamonActions[3] = gm_createAction(
        "Sprinting", 
        GM_ACTION_SPRINT,
        renamonSprint,
        NULL
    );
    renamonActions[4] = gm_createAction(
        "Crouch", 
        GM_ACTION_CROUCH,
        renamonCrouch,
        NULL
    );
    
    // 'Renamon' actor
    renamonActor = gm_createActor(
        "Renamon", 
        "A hot furry digimon, yellow fox-like", 
        gm_createStats(100, 100, 10, 10, 10),
        renamonActions
    );
    
    renamonAsset = gm_createAsset(
        renamonActor,
        as_createShape(as_createBox(45, 52, 10), 250, GM_SHAPE_TYPE_BOX, true),
        sp_createCoordinates(0, 0, 0)
    );

    logger("[t_createRenamon]: Inserting asset");
    
    gm_insertAsset(renamonAsset);
  
    testPalette = renamonFile->bmpData->palette;   

    // Global camera setup
    sp_setGlobalCamera(sp_createCamera(sp_createCoordinates(0, 0, 0), sp_createScreenCoordinates(VID_WIDTH, VID_HEIGHT)));
    
    /* SET RENAMON AS PLAYER*/
    if(renamonAsset) player = renamonAsset;
}

void t_freeRenamonTest(){
    sp_destroyCamera(globalCamera); 
}

void t_testFloor(){
    /*
        This asset will be just a shape of solid color, just for testing
        the collision system
    */    

    Asset *floorAsset = NULL;
    Actor *floorActor = NULL;
    Action *floorAction = NULL;
    Animation *floorAnimation = NULL;
    Shape *floorShape = NULL;
    Coordinates *floorCoordinates = NULL;
    
    floorActor = gm_createActor(
        "Floor", 
        "A floor", 
        gm_createStats(100, 100, 10, 10, 10),
        NULL
    );
    
    floorShape = as_createShape(as_createBox(100, 100, 0), 123, GM_SHAPE_TYPE_BOX, true);
    floorCoordinates = sp_createCoordinates(105, 0, 0);
    
    floorAsset = gm_createAsset(
        floorActor,
        floorShape,
        floorCoordinates
    );
    
    gm_insertAsset(floorAsset);
}

void t_skullBgTest(){
    char *assetsPath = (char*)getEnv("ASSETS_PATH");
    char skullPath[128] = {0};
    Animation *skullAnimation = NULL;
    Action *skullBgActions[GM_MAX_ACTIONS] = {NULL};
    Asset *skullAsset = NULL;
    
    skullAnimation = as_createAnimation(); 
    as_loadAnimationFrames(skullAnimation, skullFrames, 255);  
    
    skullBgActions[0] = gm_createAction(
        "Idle", 
        GM_ACTION_IDLE,
        skullAnimation,
        NULL
    );

    skullAsset = gm_createAsset(
        gm_createActor("Skulls Background", "An impaled skulls background", gm_createStats(100, 100, 10, 10, 10), skullBgActions),
        NULL,
        sp_createCoordinates(0, 0, -100)
    );

    logger("[t_skullBgTest]: Inserting asset");
    gm_insertAsset(skullAsset);       
}