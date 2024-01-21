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
 *  Interface for timer
 */

#ifndef TIMER_H
#define TIMER_H

void timer_install();
void timer_phase(int hz);
void timer_sleep(unsigned int ticks);

#endif