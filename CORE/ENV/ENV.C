/* ENV/CFG handling by Vincebus - 2025 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "TYPES.H"

#define STRING 1
#define INT 2
#define UINT 3
#define FLOAT 4

#define STRING_MAX_LENGTH 255

int findIndex(char *str){
    if(!str) return -1;

    int i, index = 0;
    int length;

    length = strlen(str);

    for(i=0; i< length; i++){
        if(str[i] == "=") return i;
    }
    return -1;
}

void *parseValue(char *str, int type){
    void *value = NULL;
    int index = 0;
    
    if(!str) return NULL;
    
    if((index = findIndex(str) == -1)) return NULL;
    
    char *buffer = str + index + 1;

    while (*buffer && isspace(*buffer)) buffer++;

    strncpy(buffer, str + index, strlen(str) + index);

    if(type == STRING){
        value = malloc(STRING_MAX_LENGTH);
        
        sscanf(value, "%s", buffer);
    }
    if(type == INT){;                    
        int *v = malloc(sizeof(int));
        sscanf(value, "%d", buffer);
        value = v;
    }

    return value;

}

int findValue(const char *key, const char *line) {
    if (!key || !line) return 0;
    size_t keyLen = strlen(key);

    return (strncmp(line, key, keyLen) == 0 && line[keyLen] == '=') ? 1 : 0;
}

Config * loadEnv(){
    // Look for a .env file in the current directory
    // Parse the contents of the .env
    // Return the Config type object

    FILE *fp = fopen("../../.ENV", "r");
    char tmpBuffer[64];
    Config *newConfig;

    newConfig = (Config *)malloc(sizeof(Config));
    newConfig->assetsPath = (char**)calloc(255, sizeof(char*));
    newConfig->playerName = (char**)calloc(32, sizeof(char*));

    if(!fp){
        printf("No config file found.");
        free(newConfig);
        return NULL;
    }

    while(fgets(tmpBuffer, sizeof(tmpBuffer), fp) != NULL){
        if(findValue("ASSETS_PATH", tmpBuffer)){
            newConfig->assetsPath = (char*) parseValue(tmpBuffer, STRING);
        }
    }

    fclose(fp);

    return newConfig;
}