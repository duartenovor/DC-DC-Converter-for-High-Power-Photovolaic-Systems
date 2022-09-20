/**< #########################################################################################
 * ###################################     Includes     ######################################
 * #########################################################################################*/

#include "converter.h"

#include <Stdlib.h>
#include <String.h>
#include <math.h>

/**< #########################################################################################
 * ##############################     Global Variables     ###################################
 * #########################################################################################*/

float kp_h = 3;
float ki_h = 0.5;

float INC = 0.1;

int count_menos1 = 0;
int count_menos2 = 0;
int count_mais1 = 0;
int count_mais2 = 0;

/**< #########################################################################################
 * ##################################     Funções      #######################################
 * #########################################################################################*/

void init_struct(pi_t* arm)
{
    arm->i = 0;
    arm->u = 0;
    arm->error = 0.0;
    arm->prev_error = 0.0;
    arm->sum_errors = 0.0;
    arm->sum_errors_bck = 0.0;
}

void mppt_alg(int vpv, int ipv, float *i_ref)
{
    static volatile int vpv_ant = 0;
    static volatile int ipv_ant = 0;

    static volatile int delta_vpv = 0;
    static volatile int delta_ipv = 0;

    static volatile float delta_ipv_vpv  = 0;
    static volatile float ipv_vpv  = 0;

    delta_vpv = vpv - vpv_ant;
    delta_ipv = ipv - ipv_ant;

    delta_ipv_vpv = (float) delta_ipv / (float) delta_vpv;
    ipv_vpv  = (float) ipv / (float) vpv;

    if( delta_vpv == 0 )
    {
        if(delta_ipv != 0)
        {
            if( delta_ipv > 0 ) {
                *i_ref -= INC;
                count_menos1++;
            }
            else {
                *i_ref += INC;
                count_mais1++;
            }
        }
    }
    else
    {
        if ( delta_ipv_vpv != -ipv_vpv )
        {
            if( delta_ipv_vpv > -ipv_vpv ) {
                *i_ref -=  INC;
                count_menos2++;
            }
            else {
                *i_ref +=  INC;
                count_mais2++;
            }
        }
    }

//	if ( *i_ref > 400 )
//	    *i_ref = 400;
//	if( *i_ref < 0 )
//	    *i_ref = 0;

	vpv_ant = vpv;
	ipv_ant = ipv;
}

void pi_alg( pi_t* arm, float i_ref)
{
    //i_ref = 220;

    arm->error = ( i_ref / 2.0 ) - arm->i;
    arm->sum_errors_bck =  arm->sum_errors;
    arm->sum_errors +=  arm->prev_error;

    arm->u = kp_h * ( arm->error ) + ki_h * ( arm->sum_errors );
    arm->prev_error =  arm->error;

	if(  arm->u > MAX )
	{
        arm->u = MAX;
        arm->sum_errors = arm->sum_errors_bck;
	}
	else if ( arm->u < MIN )
	{
        arm->u = MIN;
        arm->sum_errors = arm->sum_errors_bck;
	}
}
