
/*
    BMP, mode X and doubly linked list test by Vincebus Riveruptum
    2024.

    ASM functions based on santiago romero's vga tutorial 
    and converted them to 32 bit with CHATGPT.

    Compiled with WATCOM C 10.6
    wcl386 bmptest2.c
*/

#include "ENGINE.H"

int main(int argc, char *argv[]){
    loadEnv();
    logger("[main]: ENV loaded!");
   
    t_initTests();

    m_initTrig(); // Log the static load
    v_set200pxMode();
    //eng_setPalette(testPalette);

    while (!checkAppEnd()){
        //fillScreen(0);
        initInput();
        
        eng_render2d(gameTicks);
        if(ENABLE_PAGE_FLIPPING == 1){
            v_flipPage(); 
        }
        gameTicks++;
    }
    
    v_setTXTMode();

    printf("\n96 Tears...");

    if(globalPalette) free(globalPalette);

    return 0;
}