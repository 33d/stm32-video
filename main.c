#include "ch.h"
#include "hal.h"
#include "video.h"
#include "display.h"
#include "vscroll.h"

int main(void) {
    halInit();
    chSysInit();

//    display_init();
//    video_line_data_func = &display_getline;
//    video_init(416, 0);
    vscroll_init(400, 305);
    video_line_data_func = &vscroll_getline;
    video_init(400, 0);

    nvicDisableVector(SysTick_IRQn);

    palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL);
    while (1) {
      palSetPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
      palClearPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
    }
}
