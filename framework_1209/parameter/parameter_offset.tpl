[+ AutoGen5 template h default +]
[+ CASE (suffix) +]
[+ == h +]#ifndef _[+ (string-upcase!(base-name)) +]_AUTOGEN_
#define _[+ (string-upcase!(base-name)) +]_AUTOGEN_
/*  This file is a autogenerate file .
 *  DONOT EDIT THIS FILE!!
 *  Edit [+ (def-file) +] and [+ (tpl-file) +] instead
 */
#include "parametertype.hin"
#ifdef __cplusplus
extern "C"{
#endif

enum para_cmd_index_array{
	[+ FOR parameter "," +]
   PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +]_INDEX [+ ENDFOR parameter +]
   ,PARA_CMD_INDEX_END
};
static unsigned int para_cmd_size_array[]={[+ FOR parameter "," +]
   sizeof([+ (get "cat_name") +]_[+ (get "para_name") +]_paratype) * ([+ (get "argument_count") +])[+ ENDFOR parameter +]
};

typedef struct para_cmd_to_para_index_
{
	unsigned int para_index;
	unsigned int para_cmd;
	unsigned int para_argument_count;
}para_cmd_to_para_index;

static para_cmd_to_para_index localparacmd_to_paraindex[] = { [+ FOR parameter "," +]
	{ PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +]_INDEX, 
	  PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +],
	  [+ (get "argument_count") +]
	}
	[+ ENDFOR parameter +]
};

static inline int  parameter_indexfind(unsigned int cmd)
{
	int index = 0;
	for ( index = 0; index < PARA_CMD_INDEX_END; index++ )
	{
		if ( localparacmd_to_paraindex[index].para_cmd == cmd )
		{
			return localparacmd_to_paraindex[index].para_index;
		}
	}
	return -1;
}

static inline int  parameter_argument_count(unsigned int cmd)
{
	int index = 0;
	for ( index = 0; index < PARA_CMD_INDEX_END; index++ )
	{
		if ( localparacmd_to_paraindex[index].para_cmd == cmd )
		{
			return localparacmd_to_paraindex[index].para_argument_count;
		}
	}
	return -1;
}

static inline int  parameter_offset( unsigned int cmd )	{	
    unsigned int total = 0,i = 0;	
	int index = 0;
    if ( (cmd) > PARA_CMD_END )
    {	
        return -1;	
    }	
	index = parameter_indexfind(cmd);	
	if ( index == -1 )
	{
		return -1;
	}
    for ( i = 0; i < (index); i++ )	
    {	
        total += para_cmd_size_array[i];	
    }	
    return total;	
}

static inline int  parameter_size( unsigned int cmd )	{	
	int index = 0;
    if ( (cmd) > PARA_CMD_END )
    {	
        return -1;	
    }	
	index = parameter_indexfind(cmd);	
	if ( index == -1 )
	{
		return -1;
	}
	return para_cmd_size_array[index] / localparacmd_to_paraindex[index].para_argument_count;
}


[+ FOR parameter  +]
#define	 PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +]_OFFSET	parameter_offset(PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +] )
[+ ENDFOR parameter +]

#ifdef __cplusplus
}
#endif

#endif // _[+ (string-upcase!(base-name)) +]_AUTOGEN_


[+ == default +]#ifndef _[+ (string-upcase!(base-name)) +]_DEFAULT_AUTOGEN_
#define _[+ (string-upcase!(base-name)) +]_DEFAULT_AUTOGEN_ 
/*  This file is a autogenerate file .
 *  DONOT EDIT THIS FILE!!
 *  Edit [+ (def-file) +] and [+ (tpl-file) +] instead
 */
#include "parametertype.hin"
#include "parameter_offset.h"
#include "parameter.h"
#ifdef __cplusplus
extern "C"{
#endif 
[+ FOR parameter ";" +]
static [+ (string-downcase! (get "cat_name")) +]_[+ (string-downcase! (get "para_name")) +]_paratype  [+  (get "para_name") +]_default = [+ (get "defaultval") +]
[+ ENDFOR parameter +];

typedef struct _paracmd_default_struct
{
	unsigned int para_cmd;
	unsigned int para_default_addr;
	unsigned int para_size;
}paracmd_default_struct;


static paracmd_default_struct paracmd_defaultaddr[] = 
{
[+ FOR parameter "," +]
{ PARA_CMD_[+ (string-upcase! (get "cat_name")) +]_[+ (string-upcase! (get "para_name")) +],
  (unsigned int)&[+  (get "para_name") +]_default,
  sizeof([+ (string-downcase! (get "cat_name")) +]_[+ (string-downcase! (get "para_name")) +]_paratype)
}
[+ ENDFOR parameter +]
};

static inline void *find_default_paracmd( unsigned int para_cmd, unsigned int *retsize)
{
	unsigned int index = 0;
	for ( index = 0; index < PARA_CMD_INDEX_END; index++ )
	{
		if ( paracmd_defaultaddr[index].para_cmd == para_cmd )
		{
			*retsize = paracmd_defaultaddr[index].para_size;
			return (void *)paracmd_defaultaddr[index].para_default_addr;
		}
	}
	return NULL;
}

typedef struct _paracmd_to_paraindex
{
	unsigned int paraindex;
	unsigned int paracmd;
}paracmd_to_paraindex;

static inline void do_flashpara_default( void *ptr_gbshm_handle )
{
	unsigned int retsize = 0;
	unsigned int index = 0;
	unsigned indexcmdindex = 0;
	unsigned int channelcount = 0x00;
	void *ptrcmddefault = NULL;

	//According index to find para_cmd
	for ( indexcmdindex = 0; indexcmdindex< PARA_CMD_INDEX_END; indexcmdindex++ )
	{
		void *pdest = NULL;
		unsigned char *pchar = NULL;
		unsigned int channelindex = 0x00;
		
		ptrcmddefault = find_default_paracmd( localparacmd_to_paraindex[indexcmdindex].para_cmd, &retsize);
	
		GBSHM_PARA_SET(ptr_gbshm_handle, localparacmd_to_paraindex[indexcmdindex].para_cmd, ptrcmddefault, retsize);
#if 0		
		pdest = malloc(retsize);
		if( pdest <= 0 )
		{
			DVS_DEBUG( "pdest <= 0!!!" );
			return;
		}
		channelcount = parameter_argument_count(localparacmd_to_paraindex[indexcmdindex].para_cmd);	
		for ( channelindex = 0x00; channelindex < channelcount; channelindex++ )
		{	
			GBSHM_GET(ptr_gbshm_handle, localparacmd_to_paraindex[indexcmdindex].para_cmd, pdest, retsize, channelindex);
			pchar = (unsigned char *)pdest;
			if ( ptrcmddefault != NULL )
			{
				for ( index = 0; index < retsize; index++ )
				{
					if ( *(pchar + index) != 0xFF )
					{
						//DVS_DEBUG("parameter cmd 0x%x not default.\n", localparacmd_to_paraindex[indexcmdindex].para_cmd);
						//continue;
						goto OUT;
					}
				}
				GBSHM_SET(ptr_gbshm_handle, localparacmd_to_paraindex[indexcmdindex].para_cmd, ptrcmddefault, retsize, channelindex);
			}
OUT:
			channelindex = channelindex;
		}
		free(pdest);
#endif
	}
}
#ifdef __cplusplus
}
#endif
#endif


[+ ESAC +]
