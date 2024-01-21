/**
 * FILENAME :       timer.c
 *
 * AUTHOR :         Ruben Lohberg
 *
 * START DATE :     19 Nov 2023
 *
 * LAST UPDATE :    07 Jan 2024
 *
 * PROJECT :        RubenOS
 *
 * DESCRIPTION :
 *  Onboard timer driver
 */

#include "timer.h"
#include "screen.h"
#include "low_level.h"
#include "bool.h"
#include "irq.h"

/**
 * Rate in hz at which the timer will send IRQs
 */
#define TIMER_RATE 100

/**
 * Timer ports
 */
#define TIMER_CONTROL 0x43
#define TIMER_DATA 0x40

/**
 * This will keep track of how many ticks that the system
 * has been running for
 */
unsigned int timer_ticks = 0;

/** Clock speed in MHZ */
#define CLOCK_SPEED 500
#define MHZ 1048576

/**
 * Uses timer Interrupts to wait
 *
 * @param int time in 10ms. Example: timer_sleep(600) = sleep for 6000ms = 6s
 */
void timer_sleep(unsigned int ticks)
{
    unsigned int i, j, k;
    for (k = 0; k < ticks; k++)
    {
        for (i = 0; i < CLOCK_SPEED / 200; i++)
        {
            for (j = 0; j < MHZ; j++)
            {
                __asm__("nop");
            }
        }
    }
}

/**
 * Function to be called then a timer interrupt occurrs
 *
 * @param regs registers as pushed by the assembly code in interrupt.asm
 */
static void timer_callback(struct regs *regs)
{
    // avoid unused parameter warning
    (void)(regs);
    /* Increment our 'tick count' */
    timer_ticks++;

    /* Every TIMER_RATE clocks (approximately 1 second), we will
     *  display a message on the screen */
    if (timer_ticks % TIMER_RATE == 0)
    {
        print_time(timer_ticks / TIMER_RATE);
    }
}

/**
 * Sets up the timer
 *
 * @param hz rate at which the timer shall sent requests
 */
void timer_phase(int hz)
{
    // timer has a input clock of 1.19MHz
    int divisor = 1193180 / hz;
    /**
     * Set timer settings
     * 2 Bit - Choose counter
     * 2 Bit - Read write mode
     * 3 Bit - Mode
     * 1 Bit - BCD Counter mode
     *
     * We choose 00 11 011 0 = 0x36
     * Timer 0
     * LSB, then MSB
     * Square Wave mode
     * 16-Bit counter
     */
    port_byte_out(TIMER_CONTROL, 0x36);

    // set the divisor
    port_byte_out(TIMER_DATA, divisor & 0xFF); /* Set low byte of divisor */
    port_byte_out(TIMER_DATA, divisor >> 8);   /* Set high byte of divisor */
}

/**
 * Sets up the system clock by installing the timer handler into IRQ0
 */
void timer_install()
{
    // Firstly, register our timer callback.
    irq_install_handler(0, &timer_callback);
    timer_phase(TIMER_RATE);
    /* Installs 'timer_handler' to IRQ0 */
    print_time(0);
}