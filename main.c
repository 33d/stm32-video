#include "ch.h"
#include "hal.h"
#include "video.h"
#include "display.h"

int main(void) {
    halInit();
    chSysInit();

    display_init();
    video_line_data_func = &display_getline;
    video_init(416, 0);

    palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL);
    while (1) {
      palSetPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
      palClearPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
    }
}
