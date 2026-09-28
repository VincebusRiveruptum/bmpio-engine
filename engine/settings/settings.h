#ifndef SETTINGS_H
#define SETTINGS_H

#include "../std.h"
#include "../../deps/log/log.h"

typedef struct Settings {
    bool DEBUG;

    char *PLAYER_NAME;
    char *TEST_ASSETS_PATH;

    bool PAGE_FLIPPING;
} Settings;

/*
 * This reloads the default config file.
 * Returns 1 if successful, 0 if error happened.
 */
extern Settings settings;

bool s_loadSettings(void);

#endif