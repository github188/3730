#ifndef _INTERCMD_RECORD_DELETE_FILE_REQ_H__
#define _INTERCMD_RECORD_DELETE_FILE_REQ_H__

#include "client_inter_comm.h"
#ifdef __cplusplus
extern "C"{
#endif

typedef struct record_delete_file_req
{
    PE_CHANNEL_ID chann_id;
    PE_TIME_FILE time_file;
    unsigned char qos;
}record_delete_file_req;

#ifdef __cplusplus
}
#endif



#endif
