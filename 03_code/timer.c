/*
 * timer.c
 *
 *  Created on: 01/06/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "timer.h"

/**< #########################################################################################
 * ##################################     Defines     ########################################
 * #########################################################################################*/
#define CPUFREQ_MHZ 150
#define Timer0_PRD 25

/**< ########################################
 * - TIMER 0
 * ##########################################*/
void Setup_Timer0(void)
{
    EALLOW;

    // Configure CPU-Timer 0 to interrupt every 25 useconds:
    // 150MHz CPU Freq, 25 useconds Period (in uSeconds)
    ConfigCpuTimer(&CpuTimer0, CPUFREQ_MHZ, Timer0_PRD);

    CpuTimer0Regs.TCR.bit.TRB = 1;
    CpuTimer0Regs.TCR.bit.FREE = 0;
    CpuTimer0Regs.TCR.bit.SOFT = 1;
    CpuTimer0Regs.TCR.bit.TIE = 1;
    EDIS;
}
