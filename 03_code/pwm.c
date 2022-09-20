/*
 * pwm.c
 *
 *  Created on: 21/04/2022
 *      Author: JoaoMiranda
 */

#include "pwm.h"

/**< F2837xD GPIO Pin Registers
 * GPxDIR -> [0 - Input | 1 - Output]
 * GPxPUD -> [0 - Internal Pull-Up enable | 1 - Internal Pull-Up disable]
 * GPxODR -> [0 - Normal | 1 - Open Drain]
 * GPxMUX -> Define if that pin will be used to GPIO or to (ex: PWM / Serial Pin) -> [tms320f28379d.pdf [49/229]]
 * GPxCSEL -> Define which CPU will be used
 * */
void GPIO_PWM(void)
{
    EALLOW;
    //Config gpio for ePWM
    InitEPwm2Gpio();
    InitEPwm3Gpio();

    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 0;   // Enable pullup on GPIO0
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 0;   // Enable pullup on GPIO2
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;  // GPIO0 = PWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;  // GPIO2 = PWM2A

    EDIS;
}

void Setup_PWM(void){
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;      // Stop all the TB clocks

    SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1;
    SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1;
    EDIS;

    //################################ PWM2 ################################
    EPwm2Regs.TBPRD = 3750;    //20kHZ
    EPwm2Regs.CMPA.half.CMPA = 0;

    // Setup counter mode
    EPwm2Regs.TBPHS.half.TBPHS = 0;                     // Phase is 0
    EPwm2Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
    EPwm2Regs.TBCTR = 0;                                // Clear counter
    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Count u
    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;             // Disable phase loading
    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Clock ratio to SYSCLKOUT
    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;   // Load on Zero
    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    EPwm2Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm2Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR;                // Set PWM1A on event A, up count
    EPwm2Regs.AQCTLA.bit.CAD = AQ_SET;                  // Clear PWM1A on event A, down count

    //################################ PWM2 ######################EPwm3Regs#

    EPwm3Regs.TBPRD = EPwm2Regs.TBPRD;                  // Set timer period
    EPwm3Regs.CMPA.half.CMPA = 0;

    EPwm3Regs.TBPHS.half.TBPHS = EPwm3Regs.TBPRD;       // Phase is 180
    EPwm3Regs.TBCTL.bit.SYNCOSEL = TB_SYNC_IN;
    EPwm3Regs.TBCTR = 0;                                // Clear counter
    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;      // Count up/down
    EPwm3Regs.TBCTL.bit.PHSEN = TB_ENABLE;              // Disable phase loading
    EPwm3Regs.TBCTL.bit.PHSDIR = TB_DOWN;               // Phase UP/DOWN
    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;            // Clock ratio to SYSCLKOUT
    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV1;

    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;          // Load registers every ZERO
    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;

    EPwm3Regs.AQCTLA.bit.PRD = AQ_NO_ACTION;
    EPwm3Regs.AQCTLA.bit.ZRO = AQ_NO_ACTION;
    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;                 // set actions for EPWM2A
    EPwm3Regs.AQCTLA.bit.CAD = AQ_SET;

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;      // Stop all the TB clocks
    EDIS;
}

/*DBFED, DBRED | TBCLK = EPWMCLK/1 | TBCLK = EPWMCLK/2 | TBCLK = EPWMCLK/4
        1              0.01 uS          0.02 uS           0.04 uS
        5              0.05 uS          0.10 uS           0.20 uS
        10             0.10 uS          0.20 uS           0.40 uS
       100             1.00 uS          2.00 uS           4.00 uS
       200             2.00 uS          4.00 uS           8.00 uS
       400             4.00 uS          8.00 uS           16.00 uS
       500             5.00 uS          10.00 uS          20.00 uS
       600             6.00 uS          12.00 uS          24.00 uS
       700             7.00 uS          14.00 uS          28.00 uS
       800             8.00 uS          16.00 uS          32.00 uS
       900             9.00 uS          18.00 uS          36.00 uS
       1000            10.00 uS         20.00 uS          40.00 uS*/
