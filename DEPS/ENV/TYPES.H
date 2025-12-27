#ifndef ENV_TYPES_H
#define ENV_TYPES_H

#define STRING 1
#define INT 2
#define UINT 3
#define FLOAT 4

#define STRING_MAX_LENGTH 255

typedef struct Config{
    char *assetsPath;
    char *playerName;
    unsigned char logType;
} Config;

#endif
