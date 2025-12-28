/* ENV/CFG handling by Vincebus - 2025

As in August 2025. the Configuration entries are hardcoded and if you want more
settings you have to modify the Config data structure, displayConf and the
loadEnv method.

*/

/*
  - Config should be a global variable
  - Should be a 2d

*/
#include "ENV.H"

ConfigEntry *configEntries = NULL;
int configEntriesCount = 0;

int findIndex(char *str) {
  int i = 0;
  int index = 0;
  int length;

  if (!str)
    return -1;

  length = strlen(str);

  for (i = 0; i < length; i++) {
    if (str[i] == '=')
      return i;
  }

  return -1;
}

bool findValue(const char *key, const char *line) {
  int keyLen = 0;

  if (!key || !line)
    return false;

  keyLen = strlen(key);

  return (strncmp(line, key, keyLen) == 0 && line[keyLen] == '=') ? true : false;
}

char *getConfigKey(char *line) {
  int index = 0;
  char *key; 
  if (!line)
    return NULL;

  if (((index = findIndex(line)) == -1))
    return NULL;

  key = (char *)malloc(index + 1);
  if (!key) return NULL;
  strncpy(key, line, index);
  key[index] = '\0';

  return key;
}

char *getConfigValue(char *line) {
  int index = 0;
  char *val; 
  int valLen;
  if (!line)
    return NULL;

  if (((index = findIndex(line)) == -1))
    return NULL;

  valLen = strlen(line) - index - 1;
  val = (char *)malloc(valLen + 1);
  if (!val) return NULL;
  strncpy(val, line + index + 1, valLen);
  val[valLen] = '\0';
  
  // Strip trailing newline if any
  if (valLen > 0 && val[valLen-1] == '\n') val[valLen-1] = '\0';
  if (valLen > 1 && val[valLen-2] == '\r') val[valLen-2] = '\0';

  return val;
}

bool isfloat(const char *str) {
    bool has_digit = false;
    bool has_dot = false;
    int i = 0;

    if (str == NULL || *str == '\0') {
        return false;
    }
    
    // Check for optional sign
    if (str[i] == '+' || str[i] == '-') {
        i++;
    }
    
    // Check digits and decimal point
    while (str[i] != '\0') {
        if (isdigit(str[i])) {
            has_digit = true;
        } else if (str[i] == '.') {
            if (has_dot) {
                return false;  // Multiple dots
            }
            has_dot = true;
        } else {
            return false;  // Invalid character
        }
        i++;
    }
    
    return has_digit;
}

char *getConfigType(char *value) {
  if (!value)
    return NULL;

  if (isdigit(value[0]))
    return INT;

  if (value[0] == 't' || value[0] == 'f')
    return BOOL;

  if (isfloat(value))
    return FLOAT;

  if(isalpha(value[0]))
    return STRING;

  return NULL;
}

bool parseConfigValue(char *key, char *type, char *value) {
  ConfigEntry *entry;

  if (!key || !type || !value)
    return false;

  entry = (ConfigEntry *)malloc(sizeof(ConfigEntry));

  entry->key = strdup(key);
 
  if (strcmp(type, INT) == 0) {
    entry->value = (void *)strdup(value);
  }

  if (strcmp(type, BOOL) == 0) {
    entry->value = (void *)strdup(value);
  }

  if (strcmp(type, FLOAT) == 0) {
    entry->value = (void *)strdup(value);
  }

  if (strcmp(type, STRING) == 0) {
    entry->value = (void *)strdup(value);
  }

  return true;
}

Config *loadEnv() {
  // Look for a .env file in the current directory
  // Parse the contents of the .env
  // Return the Config type object

  FILE *fp = fopen(".env", "r");
  char tmpBuffer[256];
  char *key;
  char *value;
  char *type;
  int i = 0;

  if (!fp) {
    printf("No config file found.");
    return NULL;
  }

  while (fgets(tmpBuffer, sizeof(tmpBuffer), fp) != NULL) {
    if (tmpBuffer[0] == '#' || tmpBuffer[0] == '\n' || tmpBuffer[0] == '\r') continue;
    
    key = getConfigKey(tmpBuffer);
    if (!key) continue;
    
    value = getConfigValue(tmpBuffer);
    if (!value) {
        free(key);
        continue;
    }
    
    type = getConfigType(value);

    // Add to global configEntries
    configEntries = realloc(configEntries, sizeof(ConfigEntry) * (i + 1));
    configEntries[i].key = key;
    configEntries[i].value = value;
    configEntries[i].type = type;
    i++;
  }

  configEntriesCount = i;
  fclose(fp);

  return configEntries;
}

void displayConf(Config *conf) {
  int i=0;

  printf("\nConfiguration content:\n");

  for(i=0; i< configEntriesCount; i++) {
    printf("%s: %s\n", configEntries[i].key, configEntries[i].value); 
  }
}

void freeConf(Config *conf) {
  int i=0;
  if (!conf) return;

  for(i=0; i< configEntriesCount; i++) {
    free(conf[i].key);
    free(conf[i].value);
  }
  free(conf);
  configEntries = NULL;
  configEntriesCount = 0;
}

void *getEnv(char *key){
  int i=0;

  for(i=0; i< configEntriesCount; i++) {
    if (strcmp(configEntries[i].key, key) == 0) {
      return configEntries[i].value;
    }
  }

  return NULL;
}

#ifdef STANDALONE
int main() {
  printf("\n\n.ENV/CFG File reader");
  printf("\nVincebus Riveruptum, 2025.");

  configEntries = loadEnv();

  if (configEntries == NULL) {
    printf("\nNo ENV/CFG file found!.");
    return 0;
  }
 
  /*
  printf("\n\ASSETS_PATH: %s", (char*)getEnv("ASSETS_PATH"));
  printf("\n\ENV_PATH: %s", (char*)getEnv("ENV_PATH"));
  */

  return 0;
}
#endif