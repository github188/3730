/*
 * =====================================================================================
 *       Copyright (C), 2008, islab@ZJU
 *       文件名     :  dvs_unixcom.h
 *       文件描述   :  DVR File Header
 *       版本信息   :  1.0
 *       创建日期   :  2008年01月16日 10时05分26秒 CST
 *       版本记录   :  none
 *       编译器     :  mipsel-linux-gcc/g++
 *       作者信息   :  Dr Islab (Islab), islab@zju.edu.cn.
 *       公司机构   :  ZJU Digital ISLab.
 *       功能说明   :
 * =====================================================================================
 */
#ifndef  LIB_UNIX_INC
#define  LIB_UNIX_INC
#ifdef __cplusplus
extern "C"{
#endif

#include "system_debug.h"
#include "intercmdtype.hin"
#include "intercmd.h"
typedef unsigned int BLOCK_BOOL;
#define FALSE 0
#define TRUE  1

#define CMDINFO_LEN 128

#define ACK_REQUIRE	0x55555555
#define ACK_NO_REQUIRE	0xAAAAAAAA
typedef struct _unix_cmd_struct
{
    unsigned int cmd;				//浜や簰鍛戒护瀛�
    unsigned int cmdlen;			//浜や簰鍛戒护淇℃伅闀垮害
    unsigned char cmdinfo[CMDINFO_LEN];		//浜や簰鍛戒护淇℃伅
    unsigned int ret_ack;			//鏄惁闇�瑕佸洖搴旀爣蹇�, 鍦ㄦ瀯閫犳暟鎹寘鍙戦�佺殑鏃跺�欐牴鎹渶姹傝繘琛�
}UNIX_CMD_STRUCT;

#define UNIX_ACK_FLAG	0x55555555
#define UNIX_NACK_FLAG	0xAAAAAAAA
typedef struct _unix_cmdack_struct
{
    unsigned int ack_flag;
    unsigned int ack_resv;
}UNIX_CMDACK_STRUCT;

/*
   鏈湴鍩熷鎺ュ瓧鍒涘缓鍑芥暟
   localname锛氭湰鍦扮粦瀹氱殑鏂囦欢鍚嶏紝姣忎釜妯″潡瀵瑰簲鐨勬枃浠跺悕鍦╥ntercmd.h涓畾涔�,UNIXFILE_SOCKET_XXX
   sockfd锛氬煙濂楁帴瀛楀垱寤鸿繑鍥炵殑fd
   block锛氶樆濉炴爣蹇楀瓧锛岃〃鏄庡姝ocket fd鏄笉鏄樆濉炶繘琛屼娇鐢�
 */
succeed_type UNIX_CREATE(char *localname,int *sockfd,BLOCK_BOOL block);

succeed_type UNIX_DESTROY(char *localname,int sockfd);

/*
   鏈湴鍩熷鎺ュ瓧鎺ユ敹鍑芥暟
   sockfd锛氫箣鍓嶅垱寤鸿繑鍥炵殑sockfd
   remotename锛氭寚閽堬紝鎺ユ敹鍑芥暟杩斿洖鍚庨噷闈㈡斁缃殑鏄帴鏀跺埌璋佺殑淇℃伅鎸囩ず
   pinfo锛氭帴鏀跺埌淇℃伅鐨勬寚閽堬紝閲岄潰瀛樻斁鐨勬槸鎺ユ敹鍒颁俊鎭殑鍛戒护瀛椾互鍙婁俊鎭湰韬�
 */
succeed_type UNIX_RECV(int sockfd, char *remotename, UNIX_CMD_STRUCT *pinfo);

/*
   鏈湴鍩熷鎺ュ瓧鍙戦�佸嚱鏁�
   pinfo锛氬叾涓殑cmd蹇呴』濉啓锛岄噷闈㈢殑cmd瀵瑰簲浜巌ntercmd.h涓殑PARACMD_XXX锛岄噷闈mdinfo瀵瑰簲鐨勬槸瀹冪殑鏁版嵁缁撴瀯
 */
succeed_type UNIX_SEND(int sockfd,  UNIX_CMD_STRUCT *pinfo);



#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef DVS_UNIXCOM_INC  ----- */


