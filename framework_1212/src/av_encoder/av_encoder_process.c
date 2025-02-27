#include "av_encoder_private.h"
#include "libcapture.h"

//LIBCAP_OBJ *Capphandle = NULL;
extern AV_ENCODER_PRIVATE_HANDLE global_av_encoder_private_handle;
int Isalarm =1;



int main(int argc, char **argv)
{
	CaptureBuffer buffer;
	CaptureAttr attr;
	struct v4l2_buffer v4l2buf;
	FILE *fp =NULL;
	int count = 0;
	int ret;
	void *algEncHndl = NULL;
      
/*******************AV_encoder_init******************************/
	av_encoder_init(&global_av_encoder_private_handle);

	while(1)
	{		//capture data
	     CAPTURE_GET(global_av_encoder_private_handle.pcapture_obj, &buffer);
	     if (ret < 0)
	     {
		 	SYS_ERROR("Capture_get failed ..\r\n");
			return -1;
	     }
	     /**********IDLE1 state***************/

	     if (global_av_encoder_private_handle.current_state == AVENC_IDLE)
	     {
			ALG_INIT();
			algEncHndl = ALG_CREATE(720, 480, 1536000, 7000, ALG_H264);
			ALG_RUN();
			ALG_DELETE(algEncHndl);

			if (Isalarm)
			{
				av_encoder_vidEncRunMain(&global_av_encoder_private_handle, fp);   //scrach picture ,include send msg to storage,PTZ, UNIX.... 
											  //this function change current to AVENC_ALARM_BEGIN_DEAL
				CAPTURE_PUT(global_av_encoder_private_handle.pcapture_obj, &buffer); 

			}
			//else continue;
			continue;
	     }
		
	     if (global_av_encoder_private_handle.current_state == AVENC_ALARM_BEGIN_DEAL)
	     { 	
		 	av_encoder_vidEncRunMain(&global_av_encoder_private_handle, fp)	 		     
			CAPTURE_PUT(global_av_encoder_private_handle.pcapture_obj, &buffer); 
			continue;
	     }
	     if (global_av_encoder_private_handle.current_state == AVENC_ALARM_JPEG)
	     {
 			av_encoder_vidEncRunMain(&global_av_encoder_private_handle, fp);
			global_av_encoder_private_handle.current_state == AVENC_ALARM_WAIT;
			CAPTURE_PUT(global_av_encoder_private_handle.pcapture_obj, &buffer);
			continue;
	     } 

	     if (global_av_encoder_private_handle.current_state == AVENC_ALARM_WAIT)
	     {
	       	av_encoder_vidEncRunMain(&global_av_encoder_private_handle, fp)
			CAPTURE_PUT(global_av_encoder_private_handle.pcapture_obj, &buffer);
	     }
	     else
	     {
			global_av_encoder_private_handle.current_state == AVENC_IDLE;
			CAPTURE_PUT(global_av_encoder_private_handle.pcapture_obj, &buffer);
	     }
	}

	return 1;
}
