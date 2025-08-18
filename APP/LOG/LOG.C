/*

    Audit module    -   Vincebus Riveruptum

    - Error logging
    - Activity registering

*/
#include "LOG.H"

void logToFile(char *message, unsigned long line, unsigned char module){
    time_t now = time(NULL);
    
    FILE *fp = fopen("logs.txt", "wr");
    char *errorMsgLine;
    if(!fp){
        printf("\nError trying to log to file.");
        return;
    }

    sprintf(errorMsgLine, "\n%s [ERROR] : %s \n\t At line %s on \"%s\"", now, message, line, module);
    fputs(errorMsgLine, fp);

    fclose(fp);
}

void logToConsole(char *message, unsigned long line, unsigned char module){
    time_t now = time(NULL);
    char *errorMsgLine;

    printf("\n%s [ERROR] : %s \n\t At line %s on \"%s\"", now, message, line, module);
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

#ifdef STANDALONE
int main(){
    printf("\nThis module is not standalone, but this main placeholder is for testing purposes.");
    return 0;
}
#endif