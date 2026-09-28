#include "test.h"
#include "settings/settings.h"

char *renamonJumpingFrames[] = {
    "..\\jump\\frame1.bmp",
    "..\\jump\\frame2.bmp",
    "..\\jump\\frame3.bmp",
    "..\\jump\\frame4.bmp",
    "..\\jump\\frame5.bmp",
    "..\\jump\\frame6.bmp",
    "..\\jump\\frame7.bmp",
    "..\\jump\\frame8.bmp",
    "..\\jump\\frame9.bmp",
    "..\\jump\\frame10.bmp",
    "..\\jump\\frame11.bmp",
    "..\\jump\\frame12.bmp",
    "..\\jump\\frame13.bmp",
    "..\\jump\\frame14.bmp",
    "..\\jump\\frame15.bmp",
    NULL
};

const char *renamonJumpingAltFrames[] = {
    "..\\jump2\\FRAME1.bmp",
    "..\\jump2\\FRAME2.bmp",
    "..\\jump2\\FRAME3.bmp",
    "..\\jump2\\FRAME4.bmp",
    "..\\jump2\\FRAME5.bmp",
    "..\\jump2\\FRAME6.bmp",
    "..\\jump2\\FRAME7.bmp",
    "..\\jump2\\FRAME8.bmp",
    "..\\jump2\\FRAME9.bmp",
    "..\\jump2\\FRAME10.bmp",
    "..\\jump2\\FRAME11.bmp",
    "..\\jump2\\FRAME12.bmp",
    NULL
};

const char *renamonRunningFrames[] = {
    "..\\run\\FRAME1.bmp",
    "..\\run\\FRAME2.bmp",
    "..\\run\\FRAME4.bmp",
    "..\\run\\FRAME5.bmp",
    "..\\run\\FRAME6.bmp",
    "..\\run\\FRAME11.bmp",
    NULL
};

const char *renamonSprintFrames[] = {
    "..\\sprint\\FRAME1.bmp",
    "..\\sprint\\FRAME2.bmp",
    "..\\sprint\\FRAME4.bmp",
    NULL
};

const char *renamonCrouchFrames[] = {
    "..\\crouch\\FRAME1.bmp",
    "..\\crouch\\FRAME2.bmp",
    "..\\crouch\\FRAME3.bmp",
    "..\\crouch\\FRAME4.bmp",
    "..\\crouch\\FRAME5.bmp",
    "..\\crouch\\FRAME6.bmp",
    "..\\crouch\\FRAME7.bmp",
    NULL
};

char *renamonIdleFrames[] = {
    "../assets/idle/frame1.bmp",
    NULL
};

char *skullFrames[] = {
    "../assets/sk256.bmp",
    NULL
};

const char *cokeCanFrames[] = {
    "..\\assets\\coke.BMP",
    NULL
};

struct Color *testPalette = NULL;

bool t_initTests(void)
{
    logger("\n[t_initTests]: Testing initialization");

    t_createRenamon();
    t_skullBgTest();
    t_testFloor();
    t_testFloor2();
    t_cokeCanTest();

    return true;
}

void t_createRenamon(void)
{
    int i = 0;
    char renamonPath[128] = {0};
    Action *renamonActions[GM_MAX_ACTIONS];

    BMPfile *renamonFile = NULL;
    Animation *renamonJumping = NULL;  
    Animation *renamonIdle = NULL;    
    Animation *renamonRunning = NULL;
    Animation *renamonSprint = NULL;
    Animation *renamonCrouch = NULL;

    Actor *renamonActor = NULL;
    Asset *renamonAsset = NULL;
    Stats *renamonStats = NULL;
    Shape *renamonShape = NULL;
    Box *renamonBox = NULL;
    Coordinates *renamonPos = NULL;

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        renamonActions[i] = NULL;
    }

    sprintf(
        renamonPath,
        "%s/jump/frame1.bmp",
        settings.TEST_ASSETS_PATH
    );

    renamonFile = as_loadBMPfile(renamonPath);
    if (!renamonFile) {
        logger("[t_createRenamon]: Error: renamonFile is NULL");
        return;
    }

    /* Renamon's actions */
    renamonIdle = as_createAnimation(); 
    as_loadAnimationFrames(renamonIdle, renamonIdleFrames, 255);

    renamonRunning = as_createAnimation(); 
    as_loadAnimationFrames(renamonRunning, (char **)renamonRunningFrames, 255);

    renamonJumping = as_createAnimation(); 
    as_loadAnimationFrames(renamonJumping, renamonJumpingFrames, 255);

    renamonSprint = as_createAnimation(); 
    as_loadAnimationFrames(renamonSprint, (char **)renamonSprintFrames, 255);

    renamonCrouch = as_createAnimation(); 
    as_loadAnimationFrames(renamonCrouch, (char **)renamonCrouchFrames, 255);

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

    /* Renamon actor */
    renamonStats = gm_createStats(100, 100, 10, 10, 10);
    renamonActor = gm_createActor(
        "Renamon", 
        "A yellow fox-like digimon", 
        renamonStats,
        renamonActions
    );

    renamonBox = as_createBox(45, 52, 10);
    renamonShape = as_createShape(
        renamonBox,
        255,
        GM_SHAPE_TYPE_BOX,
        false
    );
    renamonPos = sp_createCoordinates(0, 0, 0);

    renamonAsset = gm_createAsset(
        renamonActor,
        renamonShape,
        renamonPos
    );

    logger("[t_createRenamon]: Inserting asset");
    gm_insertAsset(renamonAsset);

    testPalette = renamonFile->bmpData->palette;   

    /* Global camera setup */
    sp_setGlobalCamera(
        sp_createCamera(
            sp_createCoordinates(0, 0, 0), 
            sp_createScreenCoordinates(VID_WIDTH, VID_HEIGHT)
        )
    );

    if (renamonAsset) {
        gm_player = renamonAsset;
    }
}

void t_freeRenamonTest(void)
{
    sp_destroyCamera(sp_globalCamera); 
}

void t_testFloor(void)
{
    Asset *floorAsset = NULL;
    Actor *floorActor = NULL;
    Shape *floorShape = NULL;
    Box *floorBox = NULL;
    Coordinates *floorCoordinates = NULL;
    Stats *floorStats = NULL;

    floorStats = gm_createStats(100, 100, 10, 10, 10);
    floorActor = gm_createActor(
        "Floor", 
        "A floor", 
        floorStats,
        NULL
    );

    floorBox = as_createBox(100, 100, 0);
    floorShape = as_createShape(
        floorBox,
        123,
        GM_SHAPE_TYPE_BOX,
        true
    );
    floorCoordinates = sp_createCoordinates(105, 0, 0);

    floorAsset = gm_createAsset(
        floorActor,
        floorShape,
        floorCoordinates
    );

    gm_insertAsset(floorAsset);
}

void t_testFloor2(void)
{
    Asset *floorAsset = NULL;
    Actor *floorActor = NULL;
    Shape *floorShape = NULL;
    Box *floorBox = NULL;
    Coordinates *floorCoordinates = NULL;
    Stats *floorStats = NULL;

    floorStats = gm_createStats(100, 100, 10, 10, 10);
    floorActor = gm_createActor(
        "Floor 2", 
        "A floor 2", 
        floorStats,
        NULL
    );

    floorBox = as_createBox(100, 50, 0);
    floorShape = as_createShape(
        floorBox,
        55,
        GM_SHAPE_TYPE_BOX,
        true
    );
    floorCoordinates = sp_createCoordinates(-60, 70, 0);

    floorAsset = gm_createAsset(
        floorActor,
        floorShape,
        floorCoordinates
    );

    gm_insertAsset(floorAsset);
}

void t_cokeCanTest(void)
{
    int i = 0;
    Action *cokeCanActions[GM_MAX_ACTIONS];
    Asset *cokeCanAsset = NULL;
    Actor *cokeCanActor = NULL;
    Animation *cokeCanAnimation = NULL;
    Shape *cokeCanShape = NULL;
    Box *cokeCanBox = NULL;
    Coordinates *cokeCanCoordinates = NULL;
    Stats *cokeCanStats = NULL;
    RotationTransformation *rotTr = NULL;

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        cokeCanActions[i] = NULL;
    }

    cokeCanAnimation = as_createAnimation(); 
    as_loadAnimationFrames(
        cokeCanAnimation,
        (char **)cokeCanFrames,
        255
    );  

    rotTr = as_createRotationTransformation(10, 10);
    as_addRotationTransformation(cokeCanAnimation, rotTr);

    cokeCanActions[0] = gm_createAction(
        "Idle", 
        GM_ACTION_IDLE,
        cokeCanAnimation,
        NULL
    );

    cokeCanStats = gm_createStats(100, 100, 10, 10, 10);
    cokeCanActor = gm_createActor(
        "Coke Can", 
        "A coke can", 
        cokeCanStats,
        cokeCanActions
    );

    cokeCanCoordinates = sp_createCoordinates(-60, 70, 0);

    cokeCanBox = as_createBox(16, 32, 10);
    cokeCanShape = as_createShape(
        cokeCanBox,
        255,
        GM_SHAPE_TYPE_BOX,
        false
    );

    cokeCanAsset = gm_createAsset(
        cokeCanActor,
        cokeCanShape,
        cokeCanCoordinates
    );

    gm_insertAsset(cokeCanAsset);    
}

void t_skullBgTest(void)
{
    int i = 0;
    Animation *skullAnimation = NULL;
    Action *skullBgActions[GM_MAX_ACTIONS];
    Asset *skullAsset = NULL;
    Actor *skullActor = NULL;
    Stats *skullStats = NULL;

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        skullBgActions[i] = NULL;
    }

    skullAnimation = as_createAnimation(); 
    as_loadAnimationFrames(
        skullAnimation, 
        skullFrames, 
        255
    );  

    skullBgActions[0] = gm_createAction(
        "Idle", 
        GM_ACTION_IDLE,
        skullAnimation,
        NULL
    );

    skullStats = gm_createStats(100, 100, 10, 10, 10);
    skullActor = gm_createActor(
        "Skulls Background", 
        "An impaled skulls background", 
        skullStats, 
        skullBgActions
    );

    skullAsset = gm_createAsset(
        skullActor,
        NULL,
        sp_createCoordinates(0, 0, -100)
    );

    logger("[t_skullBgTest]: Inserting asset");
    gm_insertAsset(skullAsset);       
}