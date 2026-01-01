/*

    Audit module    -   Vincebus Riveruptum

    - Error logging
    - Activity registering

*/
#include "LOG.H"

extern bool eng_checkConfig();

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
    printf("%s", outputString);
}

void logger(const char *format, ...){ // Modified signature for variadic arguments
    char *logType = NULL;
    char userMessage[512]; // Buffer for the user's formatted message
    char logString[1024];  // Buffer for the final log string
    char dateString[64];   // Buffer for the formatted date string
    va_list args;          // Variadic argument list
    
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    va_start(args, format);
    vsprintf(userMessage, format, args); // Format user message
    va_end(args);

    logType = (char*)getEnv("LOGS");
    
    // Formatted date string with leading zeros
    sprintf(dateString, "[%04d-%02d-%02d %02d:%02d:%02d]", 
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, 
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    // Construct final string, ensuring no weird leading newline in userMessage breaks the line
    {
        char *p = userMessage;
        while(*p == '\n' || *p == '\r') p++; // Skip leading newlines
        sprintf(logString, "%s %s\n", dateString, p);
    }

    if(eng_checkConfig()){
        // Use config logging
        // Added check for logType being non-NULL
        if(logType && (strcmp(logType, "file") == 0 || strcmp(logType, "fileConsole") == 0)){
            logToFile(logString);
        }
        // Added check for logType being non-NULL
        if(logType && (strcmp(logType, "console") == 0 || strcmp(logType, "fileConsole") == 0)){
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