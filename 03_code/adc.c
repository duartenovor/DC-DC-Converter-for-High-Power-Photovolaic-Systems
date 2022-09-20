/*
 * adc.c
 *
 *  Created on: 01/06/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "adc.h"

/**< ########################################
 * - Setup ADC
 * [Inicializar os 3 ADCs:
 *          - CONV00 -> ADCINA0 (A0) Vpv
 *          - CONV01 -> ADCINA1 (A1) Ipv
 *          - CONV02 -> ADCINA1 (A2) Ipv1
 *          - CONV03 -> ADCINA3 (A3) Ipv2
 * ##########################################*/
void Setup_ADC() {

    // Configure ADC
    AdcRegs.ADCMAXCONV.all = 0x0003;       // Setup 2 conv's on SEQ1
    AdcRegs.ADCCHSELSEQ1.bit.CONV00 = 0x0; // Setup ADCINA0 as 1st SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV01 = 0x1; // Setup ADCINA1 as 2nd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV02 = 0x2; // Setup ADCINA2 as 3rd SEQ1 conv.
    AdcRegs.ADCCHSELSEQ1.bit.CONV03 = 0x3; // Setup ADCINA3 as 4th SEQ1 conv.
    AdcRegs.ADCTRL2.bit.EPWM_SOCA_SEQ1 = 1;// Enable SOCA from ePWM to start SEQ1
    AdcRegs.ADCTRL2.bit.INT_ENA_SEQ1 = 1;  // Enable SEQ1 interrupt (every EOS)

    // Assumes ePWM1 clock is already enabled in InitSysCtrl();
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // Enable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;       // Select SOC from from CPMA on upcount
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;        // Generate pulse on 1st event
    EPwm1Regs.CMPA.half.CMPA = 0;          // Set compare A value
    EPwm1Regs.TBPRD = 3759;                // Set period for ePWM1 (20k)
    //EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;
    //EPwm1Regs.TBPRD = 37500;
    EPwm1Regs.TBCTL.bit.CTRMODE = 0;       // count up and start

}
