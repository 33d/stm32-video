#include "ch.h"
#include "hal.h"
#include "pal_lld.h"
#include "stm32l1xx.h"
#include "video.h"
#include <stdlib.h>

#define LINES_PER_FIELD 304
/* The number of "virtual" lines for PAL */
#define PAL_LINES_TOTAL 640

#define PAL_SYNC_SHORT (STM32_SYSCLK * 0.0000047) // the short low sync pulse
#define PAL_SYNC_LONG  (STM32_SYSCLK * 0.0000273) // the long low sync pulse
#define PAL_SYNC_FULL (STM32_SYSCLK * 0.000064 ) // the duration of a full line
#define PAL_SYNC_HALF (PAL_SYNC_FULL / 2) // the duration of a half line

typedef struct {
   int line;
   int pulse;
   int duration;
} LineInfo;

const LineInfo lineInfo[] = {
    {   0, PAL_SYNC_LONG , PAL_SYNC_HALF }, // vsync
    {   5, PAL_SYNC_SHORT, PAL_SYNC_HALF }, // post-eq
    {  10, PAL_SYNC_SHORT, PAL_SYNC_FULL }, // normal lines
    { 315, PAL_SYNC_SHORT, PAL_SYNC_HALF }, // pre-eq
    { 320, PAL_SYNC_LONG , PAL_SYNC_HALF }, // vsync
    { 325, PAL_SYNC_SHORT, PAL_SYNC_HALF }, // post-eq
    { 329, PAL_SYNC_SHORT, PAL_SYNC_FULL }, // normal
    { 634, PAL_SYNC_SHORT, PAL_SYNC_HALF }, // pre-eq
    { -1, -1, -1 } // end
};

LineDataFunc video_line_data_func;

static int video_cols;
static int video_rows;
static int video_line = 0;
static const LineInfo* video_next_line_change = &lineInfo[0];
static void* video_next_line = NULL;

CH_IRQ_HANDLER(TIM4_IRQHandler) {
    TIM4->SR &= ~TIM_SR_UIF; // clear the interrupt flag

    // start the DMA transfer for this line, if there's any data
    if (video_next_line) {
        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;
        DMA1_Channel5->CNDTR = video_cols / 8;
        DMA1_Channel5->CCR |= DMA_CCR5_EN;
    }

    // prepare the next line
    ++video_line;
    if (video_line >= PAL_LINES_TOTAL)
        video_line = 0;
    if (video_line == video_next_line_change->line) {
        TIM4->ARR = video_next_line_change->duration;
        TIM4->CCR4 = video_next_line_change->pulse;
        video_next_line_change++;
        if (video_next_line_change->line == -1)
            video_next_line_change = &lineInfo[0];
    }
    // Get the next line of data
    if (video_line_data_func)
        video_next_line = (*video_line_data_func)(video_line);
}

void video_init(int cols, int rows) {
    video_cols = cols;
    video_rows = rows;
    video_line = PAL_LINES_TOTAL;

    rccEnableAPB1(RCC_APB1ENR_TIM4EN, 0); // Enable TIM4 clock, run at SYSCLK
    rccEnableAPB1(RCC_APB1ENR_SPI2EN, 0); // Enable SPI2 clock, run at SYSCLK
    rccEnableAHB(RCC_AHBENR_DMA1EN, 0); // Enable DMA clock, run at SYSCLK

    nvicEnableVector(TIM4_IRQn, CORTEX_PRIORITY_MASK(7));

    // sync output
    palSetPadMode(GPIOB, 9, PAL_MODE_ALTERNATE(2) |
                               PAL_STM32_OSPEED_HIGHEST);

    // This can't be configured with ChibiOS, because it isn't accurate enough
    TIM4->CR1 |= TIM_CR1_ARPE; // buffer ARR, needed for PWM (?)
    TIM4->CCMR2 &= ~(TIM_CCMR2_CC4S); // configure output pin
    TIM4->CCMR2 =
            TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 /*| TIM_CCMR1_OC1M_0*/  // output high on compare match
            | TIM_CCMR2_OC4PE; // preload enable
    TIM4->CCER = TIM_CCER_CC4P // active low output
             | TIM_CCER_CC4E; // enable output
    TIM4->DIER = TIM_DIER_UIE; // enable interrupt on "update" (ie. overflow)
    TIM4->ARR = 1000; // dummy values for now, will be updated in the interrupt
    TIM4->CCR4 = 500;
    TIM4->CR1 |= TIM_CR1_CEN; // enable the counter

    palSetPadMode(GPIOB, 15, PAL_MODE_ALTERNATE(5) |
                             PAL_STM32_OSPEED_HIGHEST);           /* MOSI.    */
    SPI2->CR1 = SPI_CR1_BR_0 // divide clock by 4
            | SPI_CR1_CPOL | SPI_CR1_SSM | SPI_CR1_SSI
            | SPI_CR1_MSTR;  // master mode
    SPI2->CR2 = SPI_CR2_SSOE | SPI_CR2_TXDMAEN;
    SPI2->CR1 |= SPI_CR1_SPE; // Enable SPI
}

int main(void) {
    halInit();
    chSysInit();

    video_init(0, 0);

    palSetPadMode(GPIOB, 7, PAL_MODE_OUTPUT_PUSHPULL);
    while (1) {
      palSetPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
      palClearPad(GPIOB, 7);
      chThdSleepMilliseconds(500);
    }
}
