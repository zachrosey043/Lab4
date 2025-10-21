#include "leds.h"

#define RCC_AHB2ENR   (*(volatile uint32_t *)0x4002104C)
#define GPIOB_BASE    (0x48000400UL)
#define GPIOB_MODER   (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_OTYPER  (*(volatile uint32_t *)(GPIOB_BASE + 0x04))
#define GPIOB_OSPEEDR (*(volatile uint32_t *)(GPIOB_BASE + 0x08))
#define GPIOB_PUPDR   (*(volatile uint32_t *)(GPIOB_BASE + 0x0C))
#define GPIOB_ODR     (*(volatile uint32_t *)(GPIOB_BASE + 0x14))

void initLEDS(void){
    RCC_AHB2ENR |= (1U << 1);

    GPIOB_MODER &= ~0x0000FFFFUL;
    GPIOB_MODER |=  0x00005555UL;

    GPIOB_OTYPER &= ~0x00FFUL;
    GPIOB_OSPEEDR &= ~0x0000FFFFUL;
    GPIOB_OSPEEDR |=  0x00005555UL;
    GPIOB_PUPDR   &= ~0x0000FFFFUL;

    LEDS(0);
}

void LEDS(uint8_t value){
    GPIOB_ODR = (GPIOB_ODR & ~0xFFUL) | (value & 0xFFU);
}
