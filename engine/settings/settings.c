#include "settings.h"
#include "../../deps/env/env.h"

Settings settings;

/* ENV/settings LAYER */

bool s_loadSettings(void)
{
    log_enable = false;

    loadEnv();

    settings.DEBUG = (bool)atoi(getEnv("DEBUG", "0"));
    settings.PLAYER_NAME = (char *)getEnv("PLAYER_NAME", "player");
    settings.TEST_ASSETS_PATH = (char *)getEnv("TEST_ASSETS_PATH", "");
    settings.PAGE_FLIPPING = (bool)atoi(getEnv("PAGE_FLIPPING", "1"));

    log_enable = settings.DEBUG;

    logger("[s_loadSettings]: default.cfg loaded successfully");
    return true;
}
