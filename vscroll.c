#include "hackadl-logo.xbm"
#include "vscroll.h"
#include <stdlib.h>
#include <math.h>
#include "ch.h"

static int vscroll_height;
static int vscroll_H;
static void* vscroll_line_start;
static int vscroll_line = 0;
static int vscroll_dir = 2;

void vscroll_init(int width, int height) {
    vscroll_height = height;
    vscroll_H = hackadl_logo_height - height;
}

void* vscroll_getline(int line) {
    if (line >= 320)
        line -= 320;
    if (line == 0) {
        if (vscroll_line >= vscroll_H)
            vscroll_dir = -2;
        else if (vscroll_line == 0)
            vscroll_dir = 2;
        vscroll_line += vscroll_dir;
        vscroll_line_start = hackadl_logo_bits + ((hackadl_logo_width/8)*vscroll_line);
    }

    if (line > 28 && line < 297) {
        void* start = vscroll_line_start;
        vscroll_line_start += hackadl_logo_width/8;
        return start;
    }
    return NULL;
}
