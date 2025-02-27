/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.5.0   March 2, 2001
*                                R99   Version 3.2.0                
*                                REL-4 Version 4.0.0                
*
********************************************************************************
*/
#include "typedef.h"
#include "basic_op.h"
#include "cnst.h"


#define NB_PULSE21 2           /* number of pulses */
 
/*
********************************************************************************
*                         PUBLIC PROGRAM CODE
********************************************************************************
*/
/*************************************************************************
 *
 *  FUNCTION:  decode_2i40_11bits (decod_ACELP())
 *
 *  PURPOSE:   Algebraic codebook decoder
 *
 *************************************************************************/

void decode_2i40_11bits(
    Word16 sign,   /* i : signs of 2 pulses.                       */
    Word16 index,  /* i : Positions of the 2 pulses.               */
    Word16 cod[]   /* o : algebraic (fixed) codebook excitation    */
)
{
    Word16 i, j;
    Word16 pos[NB_PULSE21];

    /* Decode the positions */

    j = index & 1;                                       
    index = shr(index, 1);
    i = index & 7;                                       

    i = add(i, shl(i, 2));        /* pos0 =i*5+1+j*2 */
    i = add(i, 1);
    j = shl(j, 1);  
    pos[0] = add(i, j);                                  
       
    index = shr(index, 3);
    j = index & 3;                                       
    index = shr(index, 2);
    i = index & 7;                                       

    
	if (sub(j, 3) == 0)
    {       
       i = add(i, shl(i, 2));        /* pos1 =i*5+4 */
       pos[1] = add(i, 4);                               
    }
    else
    {       
       i = add(i, shl(i, 2));        /* pos1 =i*5+j */
       pos[1] = add(i, j);                               
    }
       
    /* decode the signs  and build the codeword */

    for (i = 0; i < L_SUBFR; i++) {
        cod[i] = 0;                                      
    }

    for (j = 0; j < NB_PULSE21; j++) {
        i = sign & 1;                                    
        sign = shr(sign, 1);

        
        if (i != 0) {
            cod[pos[j]] = 8191;                           /* +1.0 */
        } else {
            cod[pos[j]] = -8192;                          /* -1.0 */
        }
    }

    return;
}
