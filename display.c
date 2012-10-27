#include "video.h"
#include "display.h"
#include <string.h>
#include <stdint.h>

static uint8_t linedata[416/8];

void display_init(void) {
    memset(linedata, 0, sizeof(linedata));
    linedata[0] = 0xFF;
    linedata[sizeof(linedata) - 1] = 0x02;
    memset(linedata + 20, 0x01, 10);
}

void* display_getline(int line) {
    if (((line > 100) && (line < 120)) || ((line > 419) && (line < 439)))
        return linedata;
    else
        return NULL;
}
