/* Precomputed Trig Tables for performance */

#include "MATH.H"

long sintable[360];
long costable[360];
int trigInitialized = 0;

int round(float x) {
    return (int)(x + 0.5f);
}

void initTrig() {
    int i;
    for (i = 0; i < 360; i++) {
        float rad = (PI * i) / 180.0f;
        sintable[i] = (long)(sin(rad) * 256.0f);
        costable[i] = (long)(cos(rad) * 256.0f);
    }
    trigInitialized = 1;
}
