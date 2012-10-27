#include "hackadl-logo.xbm"
#include "lissajous.h"
#include <stdlib.h>
#include <math.h>
#include "ch.h"

static int lissajous_width;
static int lissajous_height;
static int lissajous_W;
static int lissajous_H;
static int lissajous_t;
static void* lissajous_line_start;
static struct Thread* lissajous_thread = NULL;

static WORKING_AREA(myThreadWorkingArea, 128);

static msg_t lissajous_calculate_origin(void* p) {
    while(TRUE) {
        chSysLock();
        lissajous_thread = chThdSelf();
        chSchGoSleepS(THD_STATE_SUSPENDED);
        chSysUnlock();

        ++lissajous_t;
        int x = lissajous_W/2 * sin(7.0*lissajous_t*0.005) + lissajous_W/2;
        int y = lissajous_H/2 * sin(6.0*lissajous_t*0.005) + lissajous_H/2;
        lissajous_line_start = hackadl_logo_bits
                + (hackadl_logo_width/8 * y)
                + x;
    }
    return 0;
}

void lissajous_init(int width, int height) {
    lissajous_width = width/8;
    lissajous_height = height;
    lissajous_W = hackadl_logo_width/8 - lissajous_width;
    lissajous_H = hackadl_logo_height - height;

    (void)chThdCreateStatic(myThreadWorkingArea,
            sizeof(myThreadWorkingArea),
            NORMALPRIO, lissajous_calculate_origin, NULL);
}

void* lissajous_getline(int line) {
    if (line >= 320)
        line -= 320;
    if (line == 0) {
        chSysLockFromIsr();
        if (lissajous_thread != NULL)
            chSchReadyI(lissajous_thread);
        chSysUnlockFromIsr();
    }

    if (line > 28 && line < 297) {
        void* start = lissajous_line_start;
        lissajous_line_start += hackadl_logo_width/8;
        return start;
    }
    return NULL;
}
