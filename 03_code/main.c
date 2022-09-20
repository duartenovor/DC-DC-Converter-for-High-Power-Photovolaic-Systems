/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

#include "gpio.h"
#include "pwm.h"
#include "timer.h"
#include "adc.h"
#include "converter.h"

/**< #########################################################################################
 * ###################################     Defines     #######################################
 * #########################################################################################*/

#define ADC_MODCLK 0x3 // HSPCLK = SYSCLKOUT/2*ADC_MODCLK2 = 150/(2*3)   = 25.0 MHz

#define SAMPLES 40.0

//#define vpv_medio   2070
//#define ipv_medio   2060
//#define ipv1_medio  2060
//#define ipv2_medio  1970

/**< #########################################################################################
 * ##############################     Global Variables     ###################################
 * #########################################################################################*/

static pi_t arm1;
static pi_t arm2;

static float i_ref = 400;

static volatile long vpv = 0;
static volatile long ipv1 = 0;
static volatile long ipv2 = 0;

static volatile int ipv = 0;

int vpv_debug = 0;
int ipv1_debug = 0;
int ipv2_debug = 0;

static volatile int vpv_medio = 0;
//static volatile int ipv_medio = 0;
static volatile int ipv1_medio = 0;
static volatile int ipv2_medio = 0;

static char adc_conv = 0;   //Flag que indica o fim de conversão
static char adc_reset = 1;  //Flag que possibilita a mudança dos kp, ki
static char flag_init = 0;

static char count = 0;

//int media[16];
//char i;

/**< #########################################################################################
 * #############################      Protótipo Funções      #################################
 * #########################################################################################*/

// Prototype statements for functions found within this file.
__interrupt void  adc_isr(void);
//__interrupt void cpu_timer0_isr(void);

void main(void)
{
    InitSysCtrl();

    // Define ADCCLK clock frequency ( less than or equal to 25 MHz )
    // Assuming InitSysCtrl() has set SYSCLKOUT to 150 MHz
    EALLOW;
    SysCtrlRegs.HISPCP.all = ADC_MODCLK;
    EDIS;

    DINT;

    InitPieCtrl();

    IER = 0x0000;
    IFR = 0x0000;

    InitPieVectTable();

    EALLOW;  // This is needed to write to EALLOW protected registers
    PieVectTable.ADCINT = &adc_isr;
    //PieVectTable.TINT0 = &cpu_timer0_isr;
    EDIS;    // This is needed to disable write to EALLOW protected registers

    InitCpuTimers();   // For this example, only initialize the Cpu Timers
    InitAdc();

    Setup_GPIO();
    Setup_ADC();
    GPIO_PWM();
    Setup_PWM();
    //Setup_Timer0();


    //CpuTimer0Regs.TCR.all = 0x4000; // Use write-only instruction to set TSS bit = 0

    //PieCtrlRegs.PIEIER1.bit.INTx7 = 1;
    PieCtrlRegs.PIEIER1.bit.INTx6 = 1;

    IER |= M_INT1;

    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    adc_reset = 0;
    while(!adc_conv);
    adc_reset = 1;
    adc_conv = 0;

    vpv_medio =  vpv / SAMPLES;
    ipv1_medio = ipv1 / SAMPLES;
    ipv2_medio = ipv2 / SAMPLES;

//    ipv_medio = ipv  / SAMPLES;

//    for ( i = 0; i < 16; i++)
//        media[i] = 0;
//
//    i = 0;

    while(1)
    {
//        if( i != 16 )
//        {
            if( !adc_reset )
            {
                if( adc_conv )
                {
                    vpv_debug = (vpv     /= SAMPLES);
                    //ipv     /= SAMPLES;
                    ipv1_debug = (ipv1    /= SAMPLES);
                    ipv2_debug = (ipv2    /= SAMPLES);

                    ipv = ipv1 + ipv2;

//                    media[i++] = ipv2;



                    mppt_alg(vpv, ipv, &i_ref);

                    if ( flag_init ) {
                        flag_init = 0;
                        i_ref = 400;
                    }

                    //Braço 1 -> IPV1 -> PWM2
                    arm1.i = (int) ipv1_debug;
                    pi_alg(&arm1, i_ref);
                    EPwm2Regs.CMPA.half.CMPA = arm1.u;

                    // Braço 2 -> IPV2 -> PWM3
                    arm2.i = (int) ipv2_debug;
                    pi_alg(&arm2, i_ref);
                    EPwm3Regs.CMPA.half.CMPA = arm2.u;

                    adc_conv = 0;
                }
            }
            else
            {
                init_struct(&arm1);
                init_struct(&arm2);
                flag_init = 1;
                EPwm2Regs.CMPA.half.CMPA = arm1.u;
                EPwm3Regs.CMPA.half.CMPA = arm2.u;

                vpv = 0;
                ipv = 0;
                ipv1 = 0;
                ipv2 = 0;
            }
//        }
//        else
//            i = 0;
    }
}

__interrupt void  adc_isr(void)
{
    if( !adc_reset )
    {
        vpv  += ( (AdcRegs.ADCRESULT0 >> 4) - vpv_medio  );
        //ipv  += ( (AdcRegs.ADCRESULT1 >> 4) - ipv_medio  );
        ipv1 += ( (AdcRegs.ADCRESULT2 >> 4) - ipv1_medio );
        ipv2 += ( (AdcRegs.ADCRESULT3 >> 4) - ipv2_medio );

        if( ++count == (int) SAMPLES ) {
            count = 0;
            adc_conv = 1;
        }
    }

    // Reinitialize for next ADC sequence
    AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;         // Reset SEQ1
    AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;       // Clear INT SEQ1 bit
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;   // Acknowledge interrupt to PIE

    GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1; // Toggle GPIO11

    return;
}

//__interrupt void cpu_timer0_isr(void)
//{
//   CpuTimer0.InterruptCount++;
//   GpioDataRegs.GPATOGGLE.bit.GPIO11 = 1; // Toggle GPIO32 once per 500 milliseconds
//   // Acknowledge this interrupt to receive more interrupts from group 1
//   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
//}



//===========================================================================
// No more.
//===========================================================================





