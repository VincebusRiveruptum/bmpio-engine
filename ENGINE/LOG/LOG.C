/*

    Audit module    -   Vincebus Riveruptum

    - Error logging
    - Activity registering

*/
#include "LOG.H"

void logToFile(char *outputString){
    FILE *fp = fopen("logs.txt", "a+");
    if(!fp){
        printf("\nError trying to log to file.");
        return;
    }

    fputs(outputString, fp);
    fclose(fp);
}

void logToConsole(char *outputString){
    printf("\n%s",outputString);
}

void logger(char *outputString){
    char *logType = NULL;
    char *logString = NULL;
    char dateString[255];
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    logType = (char*)getEnv("LOGS");
    
    sprintf(dateString, "[%d-%d-%d %d:%d:%d]", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

    logString = (char *)malloc(strlen(dateString) + strlen(outputString) + 2);
    sprintf(logString, "%s %s\n", dateString, outputString);

    if(checkConfig()){
        // Use config logging
        if(strcmp(logType, "file") == 0 || strcmp(logType, "fileConsole") == 0){
            logToFile(logString);
        }
        if(strcmp(logType, "console") == 0 || strcmp(logType, "fileConsole") == 0){
            logToConsole(logString);
        }       
    }else{
        // Default logging
        logToFile(logString);
    }
}

#ifdef STANDALONE
int main(){
    printf("\nThis module is not standalone, but this main placeholder is for testing purposes.");
    return 0;
}
#endif