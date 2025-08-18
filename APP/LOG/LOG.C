/*

    Audit module    -   Vincebus Riveruptum

    - Error logging
    - Activity registering

*/

#import "TYPES.H"

void logError(char *message, unsigned long line, unsigned char module){
    time_t now = time(NULL);
    
    FILE *fp = fopen("logs.txt", "wr");

    if(!fp){
        printf("\nError trying to log to file.");
        return;
    }

    char *errorMsgLine;

    sprintf(&errorMsgLine, "\n%s [ERROR] : %s \n\t At line %s on \"%s\"", now, message, line, module);
    fputs(errorMsgLine, fp);

    fclose(fp);
}

void logError(char *message, unsigned long line, unsigned char module){

    if(checkConfig()){
        // Use config logging
        if(gameConfig->logType == FILE_LOGGING || gameConfig->logType == FILE_CONSOLE_LOGGING ){
            logToFile(message,line,module);
        }
        if(gameConfig->logType == CONSOLE_LOGGING || gameConfig->logType == FILE_CONSOLE_LOGGING ){
            logToConsole(message,line,module);
        }
    }else{
        logToFile(message,line,module);
        // Default logging
    }
}