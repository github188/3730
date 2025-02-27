/************************************************************************
 * FileName:   libalg.c
 * Description:
 * Date:        2011-04-19
 * Version:     V1.0
 *  *************************************************************************/

#include "system_includes.h"
#include "system_debug.h"
#include "libenc.h"

/******************************************** 
 *    Function    : libalg_init
 *    Author      : 
 *    Date        : 2011-04-19 
 *    Description : 
 *    Para        : 
 *    Ret         : OSA_SOK       -- succeed
 *                  OSA_EFAIL     -- failed
 * **********************************************/

static succeed_type libalg_init()
{
    int status;

    status = ALG_sysInit();
          
    if(status != OSA_SOK)
    {
		SYS_ERROR("ALG_sysInit error!!\r\n");
		return status;
    }
	return status;
}

/******************************************** 
 *    Function    : libalg_exit
 *    Author      : 
 *    Date        : 2011-04-19 
 *    Description : 
 *    Para        : 
 *    Ret         : OSA_SOK       -- succeed
 *                  OSA_EFAIL     -- failed
 * **********************************************/

static succeed_type libalg_exit()
{
    int status;
    status = ALG_sysExit();
          
    if(status != OSA_SOK)
    {
		SYS_ERROR("ALG_sysInit error!!\r\n");
		return status;
    }
    return status;
}


/******************************************** 
 *    Function    : libalg_create
 *    Author      : 
 *    Date        : 2011-04-19 
 *    Description : 
 *    Para        : 
 *    Ret         : 
 * **********************************************/

//static LIBALG_OBJ *libalg_create()
static void *libalg_create(unsigned short width,unsigned short height,unsigned short offsetH,unsigned short offsetV,int bitrate,unsigned short framerate,unsigned short keyFrameInterval)

{
 //   LIBALG_OBJ *ptrhandle_alg;
    void *algEncHndl;
//    ptrhandle_alg = malloc(sizeof(LIBALG_OBJ));
    ALG_VidEncCreate createPrm;
    memset(&createPrm, 0x00, sizeof(createPrm));

    createPrm.enableROI                  = FALSE;
    createPrm.mbMvOutEnable              = FALSE;
    //createPrm.dataFormat                 = DRV_DATA_FORMAT_YUV420;
    createPrm.dataFormat                 = DRV_DATA_FORMAT_YUV422;
    //createPrm.dataFormat                 = 0;
    createPrm.vnfEnable                  = FALSE;

    createPrm.codec                      = ALG_VID_CODEC_H264;
    createPrm.width                      = width;
    createPrm.height                     = height;
    createPrm.offsetH                    = offsetH;
    createPrm.offsetV                    = offsetV;
    createPrm.bitrate                    = bitrate;
    createPrm.mode                       = ALG_VID_ENC_MODE_STANDARD;
    createPrm.keyFrameInterval           = keyFrameInterval;
    createPrm.qValue                     = 50880;
   // createPrm.rateControlType            = 1;
    createPrm.rateControlType            = 2;
    createPrm.framerate                  = framerate;

    algEncHndl = ALG_vidEncCreate(&createPrm);

    if(algEncHndl == NULL)
    {
	SYS_ERROR("vidEncCreate fail~~\r\n");
	return NULL;
    }
    
    return algEncHndl; 

}



/******************************************** 
 *    Function    : libalg_delete
 *    Author      : 
 *    Date        : 2011-04-19 
 *    Description : 
 *    Para        : 
 *    Ret         : OSA_SOK       -- succeed
 *                  OSA_EFAIL     -- failed
 * **********************************************/

static succeed_type libalg_delete(void *algEncHndl)
{
    int status;
    status = ALG_vidEncDelete(algEncHndl);
    if(status != OSA_SOK)
    {
	SYS_ERROR("vidEncDelete failed.\n");
	return status;
    }
    return status;
}


/******************************************** 
 *    Function    : libalg_run
 *    Author      : 
 *    Date        : 2011-04-19 
 *    Description : 
 *    Para        : 
 *    Ret         : 
 * **********************************************/

static int libalg_run(void *algEncHndl, unsigned char *inaddr , unsigned char *outaddr,ALG_VidEncRunStatus *pencStatus) {
    int status;
    ALG_VidEncRunPrm encPrm;

    memset(&encPrm, 0x00, sizeof(encPrm));
    memset(pencStatus, 0x00, sizeof(ALG_VidEncRunStatus));
	     
    encPrm.inAddr                    = inaddr;
    encPrm.inStartX                  = 0;
    encPrm.inStartY                  = 0;	//
    encPrm.outAddr                   = outaddr;
    encPrm.mbMvInfo                  = NULL;
    encPrm.outDataMaxSize            = 4000000;
    encPrm.forceKeyFrame             = FALSE;
    encPrm.roiPrm.numROI             = 0;

    status = ALG_vidEncRun(algEncHndl, &encPrm, pencStatus);
    if(status != OSA_SOK)
    {       
	SYS_ERROR("vidEncRun failed.\n");
	return -1;
    }
    return status;
}



static succeed_type libalg_setbitrate(void *algEncHndl, int bitrate)
{
    int status;
    status = ALG_vidEncSetBitrate(algEncHndl, bitrate);
    if(status != OSA_SOK)
    {
	SYS_ERROR("vidEncSetBitrate failed.\n");
	return status;
    }
    return status;
}

static succeed_type libalg_setframerate(void *algEncHndl, int framerate)
{
    int status;
    status = ALG_vidEncSetFramerate(algEncHndl, framerate);
    if(status != OSA_SOK)
    {
	SYS_ERROR("vidEncSetBitrate failed.\n");
	return status;
    }
    return status;
}

static succeed_type libalg_setkeyframeinterval(void *algEncHndl, int keyframeinterval)
{
    int status;
    status = ALG_vidEncSetKeyFrameInterval(algEncHndl, keyframeinterval);
    if(status != OSA_SOK)
    {
	SYS_ERROR("vidEncSetBitrate failed.\n");
	return status;
    }
    return status;
}

succeed_type ALG_BITRATE(void *algEncHndl, int bitrate)
{
    return libalg_setbitrate(algEncHndl, bitrate);
}

succeed_type ALG_FRAMERATE(void *algEncHndl, int framerate)
{
    return libalg_setframerate(algEncHndl, framerate);
}

succeed_type ALG_KEYFRAMEINTERVAL(void *algEncHndl, int keyframeinterval)
{
    return libalg_setkeyframeinterval(algEncHndl, keyframeinterval);
}


succeed_type ALG_INIT()
{
    return libalg_init();
}

succeed_type ALG_EXIT()
{
    return libalg_exit();
}

void *ALG_CREATE(unsigned short width,unsigned short height,unsigned short offsetH,unsigned short offsetV,int bitrate,unsigned short framerate,unsigned short keyFrameInterval)
{
    return libalg_create(width,height,offsetH,offsetV,bitrate,framerate,keyFrameInterval);
}

succeed_type ALG_DELETE(void *algEncHndl)
{
    return libalg_delete(algEncHndl);
}

int ALG_RUN(void *algEncHndl, unsigned char *inaddr , unsigned char *outaddr,ALG_VidEncRunStatus *pencStatus)
{
    return libalg_run(algEncHndl, inaddr , outaddr, pencStatus);
}


