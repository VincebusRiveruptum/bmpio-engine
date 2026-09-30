
/*
    BMP, mode X and doubly linked list test by Vincebus Riveruptum
    2024.

    ASM functions based on santiago romero's vga tutorial 
    and converted them to 32 bit with CHATGPT.

    Compiled with WATCOM C 10.6
    wcl386 bmptest2.c
*/

#include "engine.h"

int main(int argc, char *argv[]){
    log_init(); // OPEN LOG FILE ONCE
    s_loadSettings();
    mem_init(); // CRITICAL: Initialize memory FIRST
    
    t_initTests();

    m_initTrig();
    v_set200pxMode();
    eng_setPalette(testPalette);
    sp_init_cameras();
    hal_inp_initKeyboard();
    
    while (hal_inp_isKeyDown(HAL_KEY_ESC) == false){
        gm_listenEvents();
        sp_check_cameras();
        eng_renderFrame(gameTicks);
        if(ENABLE_PAGE_FLIPPING == 1){
            hal_vid_flipPage(); 
        }
        gameTicks++;
    }
    hal_inp_closeKeyboard();
    
    v_setTXTMode();
    mem_shutdown();

    printf("\n96 Tears...");

    if(globalPalette) free(globalPalette);
    if(sp_globalCamera) sp_destroyCamera(sp_globalCamera);
    log_shutdown();
    return 0;
}