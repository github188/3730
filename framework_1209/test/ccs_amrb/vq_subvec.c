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
/* Quantization of a 4 dimensional subvector */

Word16 Vq_subvec (/* o : quantization index,            Q0  */
    Word16 *lsf_r1,      /* i : 1st LSF residual vector,       Q15 */
    Word16 *lsf_r2,      /* i : 2nd LSF residual vector,       Q15 */
    const Word16 *dico,  /* i : quantization codebook,         Q15 */
    Word16 *wf1,         /* i : 1st LSF weighting factors      Q13 */
    Word16 *wf2,         /* i : 2nd LSF weighting factors      Q13 */  
    Word16 dico_size     /* i : size of quantization codebook, Q0  */
)
{
    Word16 index = 0; /* initialization only needed to keep gcc silent */
    Word16 i, temp;
    const Word16 *p_dico;
    Word32 dist_min, dist;

    dist_min = MAX_32;                                   
    p_dico = dico;                                       

    for (i = 0; i < dico_size; i++)
    {
        temp = sub (lsf_r1[0], *p_dico++);
        temp = mult (wf1[0], temp);
        dist = L_mult (temp, temp);

        temp = sub (lsf_r1[1], *p_dico++);
        temp = mult (wf1[1], temp);
        dist = L_mac (dist, temp, temp);

        temp = sub (lsf_r2[0], *p_dico++);
        temp = mult (wf2[0], temp);
        dist = L_mac (dist, temp, temp);

        temp = sub (lsf_r2[1], *p_dico++);
        temp = mult (wf2[1], temp);
        dist = L_mac (dist, temp, temp);

         
        if (dist < dist_min)
        {
            dist_min = dist;                             
            index = i;                                   
        }
    }

    /* Reading the selected vector */

    p_dico = &dico[shl (index, 2)];                      
    lsf_r1[0] = *p_dico++;                               
    lsf_r1[1] = *p_dico++;                               
    lsf_r2[0] = *p_dico++;                               
    lsf_r2[1] = *p_dico++;                               

    return index;

}
