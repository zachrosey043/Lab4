    .syntax unified
    .cpu cortex-m4
    .thumb

    .global waitHundredMS
    .type waitHundredMS, %function

/*
 * Busy-wait for 100 ms based on CPU cycle counting.  This routine enables the
 * DWT cycle counter (if necessary) and then waits until 400000 core clock
 * cycles have elapsed.  The code assumes a 4 MHz system clock which matches
 * the configuration used in Lab 07.
 */
waitHundredMS:
    LDR     r0, =0xE000EDFC
    LDR     r1, [r0]
    LDR     r2, =0x01000000
    ORR     r1, r1, r2
    STR     r1, [r0]

    LDR     r0, =0xE0001000
    MOVS    r1, #0
    STR     r1, [r0, #4]
    LDR     r1, [r0]
    ORR     r1, r1, #1
    STR     r1, [r0]

    LDR     r1, [r0, #4]
    LDR     r2, =400000
    ADDS    r2, r2, r1

1:
    LDR     r3, [r0, #4]
    CMP     r3, r2
    BLO     1b
    BX      lr

    .size waitHundredMS, .-waitHundredMS
