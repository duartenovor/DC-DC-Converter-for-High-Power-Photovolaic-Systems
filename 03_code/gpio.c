/*
 * Peripheral_Setup.c
 *
 *  Created on: 18/04/2022
 *      Author: JoaoMiranda
 */

/**< #########################################################################################
 * ##################################     Includes     #######################################
 * #########################################################################################*/
#include "gpio.h"

/**< F2837xD GPIO Pin Registers
 * GPxDIR -> [0 - Input | 1 - Output]
 * GPxPUD -> [0 - Internal Pull-Up enable | 1 - Internal Pull-Up disable]
 * GPxODR -> [0 - Normal | 1 - Open Drain]
 * GPxMUX -> Define if that pin will be used to GPIO or to (ex: PWM / Serial Pin) -> [tms320f28379d.pdf [49/229]]
 * GPxCSEL -> Define which CPU will be used
 * */
void Setup_GPIO(void)
{
       //Configure GPIO1 as a GPIO output pin
       EALLOW;
       GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
       GpioCtrlRegs.GPADIR.bit.GPIO11 = 1;
       GpioCtrlRegs.GPAPUD.bit.GPIO11 = 0;
       EDIS;
}





