#include "av_encoder_private.h"

static unsigned int totalcount = 0;
static int bufCurNum = 0;

#define PRINT_STATISTICS
succeed_type av_encoder_vidEncRunMain(AV_ENCODER_PRIVATE_HANDLE  *phandle,FILE *fp)
{
	UNIX_CMD_STRUCT cmd;
	int sem_value;
	ALG_VidEncRunStatus encStatus;
	buffer_new_msg bufnewmsg;
	int size = (OSA_align(phandle->width, 32))*phandle->height*2;


#ifdef PRINT_STATISTICS
	totalcount++;
	if (totalcount % 200 == 0)
	{
		int cpu_load = -1;
		printf("totalcount = %d......\r\n", totalcount);
		CPU_GET_LOAD(phandle->cpu_handle, &cpu_load);
		printf("Device type is %s and cpu is loaded %d%%\n",\
				CPU_GET_DEVICE_NAME(phandle->cpu_device), cpu_load);
	}
#endif

	memset(phandle->inVirtAddr, 0x00, size);
	memset(&encStatus, 0x00, sizeof(encStatus));

#if 1
	switch(phandle->current_state)    //phandle->current_state
	{
		case AVENC_IDLE:
		    
			//added 2012-1-6 zsx
			//判断是否有报警警
			if(ALARM_ALG_RUN(phandle->alg_obj) == ALARM_YES )
			{			
			
				//1. 调用JPEG抓拍函数
				//
				//2. 向PTZ发送消息
				cmd.cmd = PARACMD_AVENC_PTZ_START_PRESET;
				SYS_INFO("Before UNIX_SEND");
				UNIX_SEND(phandle->unix_fd, &cmd);
				bufCurNum = -1;
				if(VSSHM_GET_SEQ(phandle->pvsshm_obj, 1, &bufCurNum, &sem_value)\
						== succeed_type_failed)
				{
					SYS_ERROR("the function VSSHM_GET() failed.\r\n");
					return succeed_type_failed;
				}

				if(VSSHM_PUT(phandle->pvsshm_obj, bufCurNum, FRAME_TYPE_VIDEO_JPEG,phandle->inVirtAddr,\
							encStatus.bytesGenerated) == succeed_type_failed)
				{
					SYS_ERROR("the function VSSHM_PUT() failed.phandle->pvsshm_obj = 0x%x\r\n",phandle->pvsshm_obj);
					return succeed_type_failed;
				}

				//3. 向其他模块发送JPEG图片消息
				bufnewmsg.buffer_id = bufCurNum;
				cmd.cmd = PARACMD_AVENC_BUFFER_NEW_MSG;
				cmd.cmdlen = sizeof(bufnewmsg);
				memcpy(&cmd.cmdinfo, &bufnewmsg, sizeof(bufnewmsg));
				UNIX_SEND(phandle->unix_fd, &cmd);
				
				//4. 改变状态
				phandle->current_state = AVENC_ALARM_BEGIN_DEAL;
			}
			break;
		case AVENC_ALARM_BEGIN_DEAL:
			//放到av_encoder_unix_deal()中处理
			break;
		case AVENC_ALARM_JPEG:
			//	JPEG_RUN();    //抓拍一张小图

			cmd.cmd = PARACMD_AVENC_PTZ_START_SETPRESET;
			UNIX_SEND(phandle->unix_fd, &cmd);

			bufCurNum = -1;
			if(VSSHM_GET_SEQ(phandle->pvsshm_obj, 1, &bufCurNum, &sem_value)\
					== succeed_type_failed)
			{
				SYS_ERROR("the function VSSHM_GET() failed.\r\n");
				return succeed_type_failed;
			}

			if(VSSHM_PUT(phandle->pvsshm_obj, bufCurNum, FRAME_TYPE_VIDEO_JPEG,phandle->inVirtAddr,\
						encStatus.bytesGenerated) == succeed_type_failed)
			{
				SYS_ERROR("the function VSSHM_PUT() failed.phandle->pvsshm_obj = 0x%x\r\n",phandle->pvsshm_obj);
				return succeed_type_failed;
			}
			bufnewmsg.buffer_id = bufCurNum;
			cmd.cmd = PARACMD_AVENC_BUFFER_NEW_MSG;
			cmd.cmdlen = sizeof(bufnewmsg);
			memcpy(&cmd.cmdinfo, &bufnewmsg, sizeof(bufnewmsg));

			UNIX_SEND(phandle->unix_fd, &cmd);
			phandle->current_state = AVENC_ALARM_WAIT;
			break;
		case AVENC_ALARM_WAIT:
			//av_encoder_unix_deal()
			break;
		default:
			phandle->current_state = AVENC_IDLE;
			break;
	}
	//call osd and display
#endif

	return OSA_SOK;

}
