/* ENV/CFG handling by Vincebus - 2025

As in August 2025. the Configuration entries are hardcoded and if you want more
settings you have to modify the Config data structure, displayConf and the
loadEnv method.

*/

#include "ENV.H"

Config *conf;

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

void *parseValue(char *str, int type) {
  void *value = NULL;
  int index = 0;
  char *buffer;

  if (!str)
    return NULL;

  if (((index = findIndex(str)) == -1))
    return NULL;

  buffer = str + (index + 1);

  while (*buffer && isspace(*buffer)) {
    buffer++;
  }

  if (type == STRING) {
    char *v = malloc(STRING_MAX_LENGTH);
    sscanf(buffer, "%s", v);
    value = v;
  }

  if (type == INT) {
    int *v = malloc(sizeof(int));
    sscanf(buffer, "%d", v);
    value = v;
  }

  return value;
}

bool findValue(const char *key, const char *line) {
  int keyLen = 0;

  if (!key || !line)
    return false;

  keyLen = strlen(key);

  return (strncmp(line, key, keyLen) == 0 && line[keyLen] == '=') ? true : false;
}

Config *loadEnv() {
  // Look for a .env file in the current directory
  // Parse the contents of the .env
  // Return the Config type object

  FILE *fp = fopen(".env", "r");
  char tmpBuffer[64];
  Config *newConfig;

  newConfig = (Config *)malloc(sizeof(Config));
  newConfig->assetsPath = (char *)calloc(255, sizeof(char));
  newConfig->playerName = (char *)calloc(32, sizeof(char));

  if (!fp) {
    printf("No config file found.");
    free(newConfig);
    return NULL;
  }

  while (fgets(tmpBuffer, sizeof(tmpBuffer), fp) != NULL) {
    if (findValue("ASSETS_PATH", tmpBuffer)) {
      newConfig->assetsPath = (char *)parseValue(tmpBuffer, STRING);
    }
    if (findValue("PLAYER_NAME", tmpBuffer)) {
      newConfig->playerName = (char *)parseValue(tmpBuffer, STRING);
    }
    if (findValue("LOGS", tmpBuffer)) {
      char *logType = (char *)parseValue(tmpBuffer, STRING);
      
      printf("\n%s", logType);
      if(!strcmp(logType, "file")){
        newConfig->logType = 1;
      }
      if(!strcmp(logType, "console")){
        newConfig->logType = 2;
      }
      if(!strcmp(logType, "both")){
        newConfig->logType = 3;
      }
    }else{
        newConfig->logType = 0;
    }
  }

  fclose(fp);

  return newConfig;
}

void displayConf(Config *conf) {
  printf("\nConfiguration content:\n");

  printf("\nASSET_PATH:%s", conf->assetsPath);
  printf("\nPLAYER_NAME:%s", conf->playerName);
  printf("\nLOGS:%d", conf->logType);
}

#ifdef STANDALONE
int main() {
  printf("\n\n.ENV/CFG File reader");
  printf("\nVincebus Riveruptum, 2025.");

  conf = loadEnv();

  if (conf == NULL) {
    printf("\nNo ENV/CFG file found!.");
    return 0;
  }

  displayConf(conf);

  return 0;
}
#endif