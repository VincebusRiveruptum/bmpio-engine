#include "test.h"
#include "settings/settings.h"

char *renamonJumpingFrames[] = {
    "..\\..\\assets\\jump\\frame1.bmp",
    "..\\..\\assets\\jump\\frame2.bmp",
    "..\\..\\assets\\jump\\frame3.bmp",
    "..\\..\\assets\\jump\\frame4.bmp",
    "..\\..\\assets\\jump\\frame5.bmp",
    "..\\..\\assets\\jump\\frame6.bmp",
    "..\\..\\assets\\jump\\frame7.bmp",
    "..\\..\\assets\\jump\\frame8.bmp",
    "..\\..\\assets\\jump\\frame9.bmp",
    "..\\..\\assets\\jump\\frame10.bmp",
    "..\\..\\assets\\jump\\frame11.bmp",
    "..\\..\\assets\\jump\\frame12.bmp",
    "..\\..\\assets\\jump\\frame13.bmp",
    "..\\..\\assets\\jump\\frame14.bmp",
    "..\\..\\assets\\jump\\frame15.bmp",
    NULL
};

const char *renamonJumpingAltFrames[] = {
    "..\\..\\assets\\jump2\\Frame1.bmp",
    "..\\..\\assets\\jump2\\Frame2.bmp",
    "..\\..\\assets\\jump2\\Frame3.bmp",
    "..\\..\\assets\\jump2\\Frame4.bmp",
    "..\\..\\assets\\jump2\\Frame5.bmp",
    "..\\..\\assets\\jump2\\Frame6.bmp",
    "..\\..\\assets\\jump2\\Frame7.bmp",
    "..\\..\\assets\\jump2\\Frame8.bmp",
    "..\\..\\assets\\jump2\\Frame9.bmp",
    "..\\..\\assets\\jump2\\Frame10.bmp",
    "..\\..\\assets\\jump2\\Frame11.bmp",
    "..\\..\\assets\\jump2\\Frame12.bmp",
    NULL
};

const char *renamonRunningFrames[] = {
    "..\\..\\assets\\run\\Frame1.bmp",
    "..\\..\\assets\\run\\Frame2.bmp",
    "..\\..\\assets\\run\\Frame4.bmp",
    "..\\..\\assets\\run\\Frame5.bmp",
    "..\\..\\assets\\run\\Frame6.bmp",
    "..\\..\\assets\\run\\Frame11.bmp",
    NULL
};

const char *renamonSprintFrames[] = {
    "..\\..\\assets\\sprint\\Frame1.bmp",
    "..\\..\\assets\\sprint\\Frame2.bmp",
    "..\\..\\assets\\sprint\\Frame3.bmp",
    NULL
};

const char *renamonCrouchFrames[] = {
    "..\\..\\assets\\crouch\\Frame1.bmp",
    "..\\..\\assets\\crouch\\Frame2.bmp",
    "..\\..\\assets\\crouch\\Frame3.bmp",
    "..\\..\\assets\\crouch\\Frame4.bmp",
    "..\\..\\assets\\crouch\\Frame5.bmp",
    "..\\..\\assets\\crouch\\Frame6.bmp",
    "..\\..\\assets\\crouch\\Frame7.bmp",
    NULL
};

char *renamonIdleFrames[] = {
    "..\\..\\assets\\idle\\FRAME1.bmp",
    NULL
};

char *skullFrames[] = {
    "..\\..\\assets\\sk256.bmp",
    NULL
};

const char *cokeCanFrames[] = {
    "..\\..\\assets\\coke.BMP",
    NULL
};

struct Color *testPalette = NULL;

bool t_initTests(void)
{
    logger("\n[t_initTests]: Testing initialization");

    t_createRenamon();
    t_testFloor();
    t_testFloor2();
    t_cokeCanTest();
    t_skullBgTest();

    return true;
}

void t_createRenamon(void)
{
    int i = 0;
    char renamonPath[128] = {0};
    Action *renamonActions[GM_MAX_ACTIONS];

    BMPfile *renamonFile = NULL;
    Animation *renamonIdle = NULL;
    Animation *renamonRunning = NULL;
    Animation *renamonJumping = NULL;
    Animation *renamonSprint = NULL;
    Animation *renamonCrouch = NULL;
    Transformation *transformation = NULL;

    Actor *renamonActor = NULL;
    Asset *renamonAsset = NULL;
    Stats *renamonStats = NULL;
    Shape *renamonShape = NULL;
    Box *renamonBox = NULL;
    Coordinates *renamonPos = NULL;

    for (i = 0; i < GM_MAX_ACTIONS; i++) {
        renamonActions[i] = NULL;
    }
    logger("\nAsset path: %s", settings.TEST_ASSETS_PATH);
    
    sprintf(
        renamonPath,
        "%s/jump/frame1.bmp",
        settings.TEST_ASSETS_PATH
    );

    renamonFile = loadBMPfile(renamonPath, gameSessionArena);
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
        "Walk", 
        GM_ACTION_WALK,
        renamonIdle,
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
    floorCoordinates = sp_createCoordinates(-10, 10, 0);

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

    cokeCanCoordinates = sp_createCoordinates(0, 0, 0);

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
        sp_createCoordinates(0, 0, -5)
    );

    logger("[t_skullBgTest]: Inserting asset");
    gm_insertAsset(skullAsset);       
}
