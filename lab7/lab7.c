#include <stdint.h>

#include "leds.h"

/*
 * LAB 07: Timers and PWM
 *
 * Three different techniques are provided to generate a 100 ms period for
 * blinking LEDs on the STM32L4 Nucleo board:
 *  1. Busy-waiting for 100 ms worth of CPU cycles using waitHundredMS() which
 *     is implemented in functions.s.
 *  2. Using the SysTick timer to generate a periodic interrupt.
 *  3. Using PWM output on TIM2 channel 1 (PA0).
 *
 * Exactly one technique should be enabled at a time by defining one of the
 * feature switches below.  The default is to use the assembly busy-wait.
 */

#define CYCLE_COUNTING 1
/* #define SYSTICK 1 */
/* #define PWM 1 */

#define CORE_CLOCK_HZ 4000000UL

extern void waitHundredMS(void);

static void initSysTickTimer(uint32_t reload_value);
static void PWM_Init(uint32_t duty, uint32_t period);

#ifdef SYSTICK
static volatile uint8_t led_state = 0;
#endif

int main(void){
#ifndef PWM
    initLEDS();
#endif

#ifdef CYCLE_COUNTING
    while(1){
        LEDS(0xFF);
        waitHundredMS();
        LEDS(0x00);
        waitHundredMS();
    }
#elif defined(SYSTICK)
    initSysTickTimer((CORE_CLOCK_HZ / 10U) - 1U);
    while(1){
        /* The LED toggling occurs inside the SysTick interrupt handler. */
    }
#elif defined(PWM)
    PWM_Init(CORE_CLOCK_HZ / 10U, CORE_CLOCK_HZ / 5U);
    while(1){
        /* PWM hardware drives the LED; no work required in the main loop. */
    }
#endif

    return 0;
}

#ifdef SYSTICK
void SysTick_Handler(void){
    led_state ^= 0xFF;
    LEDS(led_state);
    (void)*(volatile uint32_t *)0xE000E010;
}
#endif

static void initSysTickTimer(uint32_t reload_value){
    volatile uint32_t * const STK_CTRL = (uint32_t *)0xE000E010;
    volatile uint32_t * const STK_LOAD = (uint32_t *)0xE000E014;
    volatile uint32_t * const STK_VAL  = (uint32_t *)0xE000E018;

    *STK_CTRL = 0;
    *STK_LOAD = reload_value;
    *STK_VAL  = 0;
    *STK_CTRL = (1U << 2) | (1U << 1) | 1U;
}

static void PWM_Init(uint32_t duty, uint32_t period){
    volatile uint32_t * const RCC_AHB2ENR  = (uint32_t *)0x4002104C;
    volatile uint32_t * const RCC_APB1ENR1 = (uint32_t *)0x40021058;
    volatile uint32_t * const GPIOA_MODER  = (uint32_t *)0x48000000;
    volatile uint32_t * const GPIOA_AFRL   = (uint32_t *)0x48000020;
    volatile uint32_t * const TIM2_CR1     = (uint32_t *)0x40000000;
    volatile uint32_t * const TIM2_EGR     = (uint32_t *)0x40000014;
    volatile uint32_t * const TIM2_CCMR1   = (uint32_t *)0x40000018;
    volatile uint32_t * const TIM2_CCER    = (uint32_t *)0x40000020;
    volatile uint32_t * const TIM2_PSC     = (uint32_t *)0x40000028;
    volatile uint32_t * const TIM2_ARR     = (uint32_t *)0x4000002C;
    volatile uint32_t * const TIM2_CCR1    = (uint32_t *)0x40000034;

    *RCC_AHB2ENR  |= (1U << 0);

    *GPIOA_MODER &= ~(0x3U << (0 * 2));
    *GPIOA_MODER |=  (0x2U << (0 * 2));

    *GPIOA_AFRL  &= ~(0xFU << (0 * 4));
    *GPIOA_AFRL  |=  (0x1U << (0 * 4));

    *RCC_APB1ENR1 |= (1U << 0);

    *TIM2_CR1 = 0;
    *TIM2_PSC = 0;
    *TIM2_ARR = period - 1U;
    *TIM2_CCR1 = duty;

    *TIM2_CCMR1 &= ~0xFFU;
    *TIM2_CCMR1 |= (6U << 4) | (1U << 3);

    *TIM2_CCER &= ~(1U << 1);
    *TIM2_CCER |= (1U << 0);

    *TIM2_EGR = 1U;

    *TIM2_CR1 |= (1U << 7);
    *TIM2_CR1 |= 1U;
}
