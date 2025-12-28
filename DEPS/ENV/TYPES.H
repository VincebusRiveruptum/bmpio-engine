#ifndef ENV_TYPES_H
#define ENV_TYPES_H

#define STRING "STRING"
#define INT "INT"
#define BOOL "BOOL"
#define FLOAT "FLOAT"
#define PATH "PATH"

typedef struct ConfigEntry{
  char *key;
  char *type;
  void *value;
} ConfigEntry, Config;
#define STRING_MAX_LENGTH 255

#define true 1
#define false 0

typedef unsigned char bool;

#endif
