/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.5.0   March 2, 2001
*                                R99   Version 3.2.0                
*                                REL-4 Version 4.0.0                
*
********************************************************************************
*/
#ifndef g_code_h
#define g_code_h "$Id $"
 
#include "typedef.h"
 
Word16 G_code (        /* out      : Gain of innovation code.               */
    Word16 xn[],       /* in       : target vector                          */
    Word16 y2[]        /* in       : filtered inovation vector              */
);
 
#endif
