#include "system_includes.h"
#include "system_debug.h"
#include "parameter_offset.h"
#include "libasshm.h"

int main(int argc, char **argv)
{
	LIBASSHM_OBJ *p = ASSHM_INIT();
	if(NULL == p)
	{
		SYS_INFO("ASSHM_INIT\r\n");
	}
	char buf[1024];

	memset(buf, 0x15, 1024);

	ASSHM_PUT(p, 0, FRAME_TYPE_AUDIO_START, buf, 1024);


	//ASSHM_MAGIC_DESTROY(p);
	ASSHM_DESTROY(p);
	return 0;
}
