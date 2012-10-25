#include "video.h"
#include "display.h"
#include <string.h>
#include <stdint.h>

static uint8_t linedata[416/8];

void display_init(void) {
    int i;
    memset(linedata, 0xFF, sizeof(linedata));
    linedata[0] = 0;
    linedata[sizeof(linedata) - 1] = 0;
    memset(linedata + 20, 0, 10);
}

void* display_getline(int line) {
    if (((line > 100) && (line < 120)) || ((line > 429) && (line < 449)))
        return linedata;
    else
        return NULL;
}
