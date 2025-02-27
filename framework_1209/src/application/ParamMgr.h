#ifndef _PARAM_MGR_H_
#define _PARAM_MGR_H_


#ifdef _cplusplus
extern "C" {
#endif
	
#include <queue>
#include <string.h>
using namespace std;

typedef struct _FileNameNode_t
{
    char   v_name[128];
}FileNameNode_T;//参数基本元素

typedef  queue<FileNameNode_T> FileNameList_T;

#include "TypeDefs.h"
#include "CmdDef.h"
#include "SystemConf.h"
#include "AreaConf.h"
#include "MediaConf.h"
#include "Ptz.h"
//#include "AppSV/APPInterface.h"
#include "PtzControl/PtzControlInterface.h"

//服务器列表（通过广播扫描到的服务器信息）
typedef  struct _SERVER_LIST
{
	unsigned char ipaddr[32];//服务器IP地址
	unsigned char servername[128];//服务器名称
	CAM_STATE cam_state;//识别器状态
	unsigned int flagvalid;//服务器有效标志
}SERVER_LIST;


//和服务器相关参数
typedef  struct  _CENTER_PTHREAD_MAIN_PARA
{
	unsigned  char  picfar_name[128];//远景照片名称
	unsigned  char  picnear_name[128];//近景照片名称
	unsigned  int   flag_valid;      //是否有效标志
	unsigned  int   savetime;       //以秒为单位
}CENTER_PTHREAD_MAIN_PARA;//和中心交互线程与主进程动态交互参数

//识别器服务器存储相关参数
typedef  struct  _GLOBAL_FLASH
{
	PTZ_PARA  ptzpara;
	VIDEO_PARA videopara;
	ALG_PARA algpara;
#if 1
	ALG_QUAD_PARA algquadpara[32];//总共64个区域
	ALG_AREA_PARA  algareapara[8];
	ALG_LINE_PARA  alglinepara[4];
	ALG_COUNT_PARA  algcountpara[4];
	ALG_ENTER_PARA  algenterpara[4];
	ALG_OUT_PARA algoutpara[4];
//	ALG_STOP_PARA  algstoppara[2];
	ALG_LOST_PARA  alglostpara[2];
	ALG_DIRECT_PARA  algdirectpara[4];
	ALG_DETECT_PARA  algdetectpara[4];
	ALG_PEOPLEDENSE_PARA algpeopledectpara[4];  
	ALG_CLUSTERDETE_PARA algclusterdectpara[4];
	ALG_MOTIONHISTORY_PARA algmotionhistorypara[4];
	ALG_HOVERDETE_PARA alghoverdectpara[4];
  	ALG_ALARMLINE_PARA algalarmlinepara[4]; //警戒线摸高算法   add by hzc 2012-05-20
	ALG_FINDBAG_PARA algfindbagpara[4]; //遗留物检测算法		add by hzc 2012-05-20
	ALG_RUNNINGOBJ_PARA algrunningpara[4]; //奔跑算法检测		add by hzc 2012-05-20

	SYS_CONFIG_PARA  sysconfigpara;
	SYS_STORAGE_PARA  sysstoragepara;
#endif
}GLOBAL_FLASH;

typedef struct _GLOBAL_FLASH_INI_GROUP
{
	unsigned int bPtzPara:1;
	unsigned int bVideoPara:1;
	unsigned int bAlgPara:1;
	unsigned int bAlgQuadPara:1;
	unsigned int bAlgAreaPara:1;
	unsigned int bAlgLinePara:1;
	unsigned int bAlgCountPara:1;
	unsigned int bAlgEnterPara:1;
	unsigned int bAlgOutPara:1;
//	unsigned int bAlgStopPara:1;
	unsigned int bAlgLostPara:1;
	unsigned int bAlgDirectPara:1;
	unsigned int bAlgDetectPara:1;
	unsigned int bAlgPeopleDensePara:1;
	unsigned int bAlgPeopleClusterPara:1;
	unsigned int bAlgPeopleMotionPara:1;
	unsigned int bAlgHoverPara:1;
	unsigned int bAlgAlarmLinePara:1;
	unsigned int bAlgFindBagPara:1;
	unsigned int bAlgRunningPara:1;
	unsigned int bAlgSysConfigPara:1;
	unsigned int bAlgStoragePara:1;
}GLOBAL_FLASH_INI_GROUP;

//识别器服务器 通信结构
typedef struct _SOCKET_STRUCT_T
{
	int socket_it;//套接字
	char socket_cip[32];//ip地址
	int socket_cport;//端口号
}SOCKET_STRUCT_T;

typedef  struct _PTZ_CTL_HANDLE
{
	PtzControlInterface *ptz_Interface;	//云台控制句柄
	bool                 ptz_initd;     //是否正常
}PTZ_CTL_HANDLE;


//识别器服务器 全局变量
typedef  struct  _GLOBAL_VARIABLE
{
	GLOBAL_FLASH  globalflash; //里面是存放在参数文件中的系统参数
	SOCKET_STRUCT_T  center_tcpsocket;     //和中心连接的TCP套接字句柄
	SOCKET_STRUCT_T  center_udpsocket;     //和中心连接的UDP套接字句柄
	SOCKET_STRUCT_T  client_tcpsocket;     //和客户端连接的TCP套接字句柄
	SOCKET_STRUCT_T  client_udpsocket;     //和客户端连接的UDP套接字句柄
	//CENTER_PTHREAD_MAIN_PARA  center_pthread_main_para[100];//最大预存100个报警记录缓冲，待发送到中心服务器
	//unsigned  char  *ptrrawdata;        //指向原始采集缓冲的指针
	//CRITICAL_SECTION csFileNameList;	//主进程和中心交互线程之间的同步锁 控制报警文件序列的互斥锁
	//Pthread_mutex  pthread_center；       //主进程和中心交互线程之间的同步锁
	//Pthread_mutex  pthread_client；       //主进程和客户端线程之间的同步锁
	//Video_Encoder  h264_handle;     //H264句柄
	//VCA_HANDLE  *pvca_handle；      //VCA句柄，是否三种不同的算法需要三个？
	PTZ_CTL_HANDLE ptz_ctlhandle;	//云台控制句柄
	//ALARM_HANDLE *alarm_handle；
	unsigned  int  storage_alarmflag;    //存储报警标志
	unsigned  int  usedtime;       //是否定义为工作时间还是安装时间？需要动态更新
}GLOBAL_VARIABLE;


//网络套接字相关参数
typedef  struct  _SERVER_SOCKET
{
	int  tcp_socketfd;//和中心相连接的TCP套接字
	int  udp_socketfd;//和中心相连的UDP套接字
	int  connectindex;//和serverlist中的哪个正在进行连接（如果一个都没有，则值为-1）
}SERVER_SOCKET;


//其他相关参数
typedef  struct  _OTHER_PARA
{
	////
}OTHER_PARA;

typedef   struct   _GLOBAL_CLIENT_PARA
{
    unsigned  int  servercount;//有效服务器的数目
	SERVER_LIST serverlist[1024];
	GLOBAL_FLASH globalflashpara;
	SERVER_SOCKET socketpara;
	OTHER_PARA otherpara;
}GLOBAL_CLIENT_PARA;

// typedef struct ParamNode_T
// {
//     string   v_name;
// 	string   v_type;
//     int		 v_len;
//     long int v_offset;
// }ParamNode_T;//参数基本元素

// class CParamMgr
// {
// 	
// private:
// 	static HANDLE						m_hMapFile;				//映射到共享内存中
// 	static LPTSTR						m_psMapAddr;			//映射到共享内存到程序内存的地址
// 	static CRITICAL_SECTION				m_stSection;			//内存访问关键区 写保护
// 	static unsigned long int			m_lMemSize;				//内存大小
// 	static unsigned long int			m_lFixedParamSize;		//固定参数大小 需要保存的信息
// 	static unsigned long int			m_lCurOffset;			//当前偏移处 
// 	static vector<ParamNode_T>			m_vParam;				//参数链表
// 	
// public:
// 	CParamMgr();
// 	CParamMgr(long int MemSize);
// 	virtual ~CParamMgr();
// 	static bool Param_DownLoad();				//将参数保存到文件
// 	static bool Param_UpLoad();					//将参数上传到内存区
// 	static bool Param_MemInit();				//将参数内存分配初始化
// 	static bool Param_MemDestroy();				//将参数内存销毁
// 	//value 为NULL则没有初始值
// 	static bool Param_Define(string name, string type, int len, int flag, void *value=NULL);
// 	static bool Param_Set(string name, void *value=NULL);
// 	static void* Param_Addr_Get(string name);
// 	static bool Param_Get(string name, void *value=NULL);
// };
/************************************************************************/
/* 全局区                                                                     */
/************************************************************************/
extern GLOBAL_FLASH g_stGlobal_Flash;
extern GLOBAL_VARIABLE g_stGlobal_Variable;//运行时使用的文件
/*extern CParamMgr ParamMgrInstance;*/
//extern CRITICAL_SECTION g_csGlobal_Variable_Section;
extern FileNameList_T g_FileNameList;//报警图片一级队列 优先发送 发送失败加入二级队列
extern FileNameList_T g_FileNameListReserved;//报警图片二级队列 当一级队列为空时进行发送 发送失败则丢弃

/************************************************************************/
/* 参数管理                                                                     */
/************************************************************************/
//直接对参数的全局变量进行操作
bool FlashParam_Init();//初始化关键区变量
bool FlashParam_UpLoad();//将文件中的数据拷贝到全局变量中
bool FlashParam_DownLoad();//将全局变量中的数据被分到到文件中


bool FlashParam_SetPtzPara(PTZ_PARA &para);//设置Ptz参数
bool FlashParam_SetVideoPara(VIDEO_PARA &para);//设置Ptz参数
bool FlashParam_SetAlgPara(ALG_PARA &para);//设置Alg参数
bool FlashParam_SetAlgParaConf(ALG_PARA_CONF_UINT *para,UINT num);//设置Alg参数配置
bool FlashParam_SetAlgQuadPara(ALG_QUAD_PARA *para,UINT num);//设置AlgQuad参数
bool FlashParam_SetAlgAreaPara(ALG_AREA_PARA *para,UINT num);//设置AlgArea参数
bool FlashParam_SetAlgLinePara(ALG_LINE_PARA *para,UINT num);//设置AlgLine参数
bool FlashParam_SetAlgCountPara(ALG_COUNT_PARA *para,UINT num);//设置AlgCount参数
bool FlashParam_SetAlgEnterPara(ALG_ENTER_PARA *para,UINT num);//设置AlgEnter参数
bool FlashParam_SetAlgOutPara(ALG_OUT_PARA *para,UINT num);//设置AlgOut参数
//BOOL FlashParam_SetAlgStopPara(ALG_STOP_PARA *para,UINT num);//设置AlgStop参数
bool FlashParam_SetAlgLostPara(ALG_LOST_PARA *para,UINT num);//设置AlgLost参数
bool FlashParam_SetAlgDirectPara(ALG_DIRECT_PARA *para,UINT num);//设置AlgDirect参数
bool FlashParam_SetAlgDetectPara(ALG_DETECT_PARA *para,UINT num);//设置AlgDetect参数
bool FlashParam_SetAlgPeopleDensePara(ALG_PEOPLEDENSE_PARA *para,UINT num);//设置AlgPeopleDete参数
bool FlashParam_SetAlgClusterDetePara(ALG_CLUSTERDETE_PARA *para,UINT num);//设置AlgClusterDete参数
bool FlashParam_SetAlgMotionHistoryPara(ALG_MOTIONHISTORY_PARA *para,UINT num);//设置AlgMotionHistory参数
bool FlashParam_SetAlgHoverDetePara(ALG_HOVERDETE_PARA *para,UINT num);//设置AlgHoverDete参数
bool FlashParam_SetAlgFindbagPara(ALG_FINDBAG_PARA *para,UINT num);//设置AlgFindbag参数
bool FlashParam_SetAlgAlarmLinePara(ALG_ALARMLINE_PARA *para,UINT num);//设置摸高警戒线参数
bool FlashParam_SetAlgRunningPara(ALG_RUNNINGOBJ_PARA *para,UINT num);//设置奔跑参数

bool FlashParam_SetSysStoragePara(SYS_STORAGE_PARA &para);//设置SysStorage参数

#if 0
#endif

bool FlashParam_SetSysConfigPara(SYS_CONFIG_PARA &para);//设置SysConfig参数

bool ParseINIFileToXML(unsigned int flag,string fname);
//向管理系统添加信息
//向系统注册系统参数 启动时使用 需要保存
// #define PARAM_DEFINE(name, type, value) \
// 	do{									\
// 		ParamMgrInstance.Param_Define((name), #type, (sizeof(##type##)),1,##((type *)value));	\
// 	}while(0);
// 
// //向系统注册非系统参数 运行时使用 不需要保存
// #define PARAM_DEFINE_RUN(name, type, value) \
// 	do{									\
// 		ParamMgrInstance.Param_Define((name), #type, (sizeof(##type##)),0,##((type *)value));	\
// 	}while(0);
// 
// //获取特定参数值
// #define PARAM_GET(name, value) \
// 	do{									\
// 		ParamMgrInstance.Param_Get((name), (value));	\
// 	}while(0);
// 
// //获取特定参数指针
// #define PARAM_ADDR_GET(name) ParamMgrInstance.Param_Addr_Get((name))
// 
// //设置特定参数
// #define PARAM_SET(name, value) \
// 	do{									\
// 		ParamMgrInstance.Param_Set((name), (value));	\
// 	}while(0);
// 
// //从配置文件读取默认参数到内存中
// #define PARAM_UPLOAD() \
// 	do{									\
// 		ParamMgrInstance.Param_UpLoad();	\
// 	}while(0);
// 
// //将内存中的参数保存到文件中
// #define PARAM_DOWNLOAD() \
// 	do{									\
// 		ParamMgrInstance.Param_DownLoad();	\
// 	}while(0);

#ifdef _cplusplus
}
#endif

#endif // _PARAM_MGR_H_
