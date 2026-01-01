#include "TEST.H"

const char *renamonFrames[] = {
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

struct Color *testPalette;

bool t_initTests(){
    char *assetsPath = (char*)getEnv("ASSETS_PATH");
    char renamonPath[128] = {0};
    BMPfile *renamonFile = NULL;
    Animation *renamonJumping = NULL;

    sprintf(&renamonPath, "%s\\jump\\FRAME1.bmp", assetsPath);
    
    renamonFile = as_loadBMPfile(renamonPath);
    
    renamonJumping = as_createAnimation(sp_createCoordinates(0, 0, 0)); 
    as_loadAnimationFrames(renamonJumping, renamonFrames);
    
    testPalette = renamonFile->bmpData->palette;
    
    logger("\n[engine/test/t_initTests]: Testing initialization");

    return true;
}