#include "system_includes.h"
#include "system_debug.h"
#include "libaudio.h"
#include <linux/soundcard.h>


#define AUDIO_DEVICE    "/dev/dsp"
#define MIXER_DEVICE    "/dev/mixer"


static LIBAUDIO_OBJ *libaudio_init(int modeinput)
{
	int i = 0;
	int mode;	

	int channels   = 2;
	int format     = AFMT_S16_LE;
	int sampleRate = 8000;
	int vol        = 100 | (100<<8);

	int recorder;
	int recMask;

	LIBAUDIO_OBJ *ptrhandle_audio = malloc(sizeof(LIBAUDIO_OBJ));
	if (ptrhandle_audio == NULL)
	{
		SYS_ERROR("malloc phandle failed.\r\n");
		return NULL;
	}

#if 1
	ptrhandle_audio->audio_volfd = open(MIXER_DEVICE, O_RDONLY);

//	printf("audio_volfd = %d ..\r\n", ptrhandle_audio->audio_volfd );
	if (ptrhandle_audio->audio_volfd == -1) {
		printf("ERR	Sound_oss.c 	mixer open failed 	\n");
		return NULL;
	}
#endif

	if ( modeinput == AUDIO_MODE_READ )
	{
		//1.audio_rfd
		printf("####	Sound_oss.c 	0	\n");
		mode = O_RDONLY;
		/* Open the sound device for writing */
		ptrhandle_audio->audio_rfd = open(AUDIO_DEVICE, mode);
		if (ptrhandle_audio->audio_rfd == -1) {
			printf("ERR	Sound_oss.c  sound device open failed	\n");
			return NULL;
		}
#if 1
		printf("####	Sound_oss.c 	1	\n");
				/* Set the sound format (only AFMT_S16_LE supported) */
		if (ioctl(ptrhandle_audio->audio_rfd, SNDCTL_DSP_SETFMT, &format) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}
		printf("####	Sound_oss.c 	2       \n");

		/* Set the number of channels */
		if (ioctl(ptrhandle_audio->audio_rfd, SNDCTL_DSP_CHANNELS, &channels) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}
		printf("####	Sound_oss.c 	3	\n");
#endif
		/* Set the sample rate */
		if (ioctl(ptrhandle_audio->audio_rfd, SNDCTL_DSP_SPEED, &sampleRate) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}
		return ptrhandle_audio;	//2011-06-23
	}
	else
	{
		//2.audio_wfd
		printf("####	Sound_oss.c 	0	\n");
		mode = O_WRONLY;
		/* Open the sound device for writing */
		ptrhandle_audio->audio_wfd = open(AUDIO_DEVICE, mode);
		if (ptrhandle_audio->audio_wfd == -1) {
			printf("ERR	Sound_oss.c  sound device open failed	\n");
			return NULL;
		}
		printf("####	Sound_oss.c 	1	\n");
		/* Set the sound format (only AFMT_S16_LE supported) */
		if (ioctl(ptrhandle_audio->audio_wfd, SNDCTL_DSP_SETFMT, &format) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}

		printf("####	Sound_oss.c 	2       \n");
		/* Set the number of channels */
		if (ioctl(ptrhandle_audio->audio_wfd, SNDCTL_DSP_CHANNELS, &channels) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}
		printf("####	Sound_oss.c 	3	\n");
		/* Set the sample rate */
		if (ioctl(ptrhandle_audio->audio_wfd, SNDCTL_DSP_SPEED, &sampleRate) == -1) {
			printf("FILE: %s ,   LINE: %d .", __FILE__, __LINE__);
			return NULL;
		}
	}
	return ptrhandle_audio;
}

static succeed_type libaudio_destroy(LIBAUDIO_OBJ *phandle, int mode)
{
	if (phandle == NULL)
	{
		SYS_ERROR("ptrhandle_gbshm is NULL.\r\n");
		return succeed_type_failed;
	}
	if (mode == AUDIO_MODE_READ)
	{
		close(phandle->audio_rfd);
	}
	else
	{
		close(phandle->audio_wfd);
	}
	close(phandle->audio_volfd);

	free(phandle);
	return succeed_type_succeed;
}

static void stereoToMono(unsigned short *stereoSamples, unsigned short *monoSamples, int numSamples)
{
	int i;
	int val;

	for (i=0; i < numSamples; i++) {
		val = *stereoSamples++;
		*monoSamples++ = (val + *stereoSamples++) / 2;
	}
}

static void monoToStereo(unsigned char *outBuf, unsigned char* inBuf, int size)
{
	int i;

	/* Expand mono to stereo */
	for (i = (size - 1); i >= 0; i--) 
	{
		outBuf[4 * i]     = inBuf[2 * i];
		outBuf[4 * i + 1] = inBuf[2 * i + 1];
		outBuf[4 * i + 2] = inBuf[2 * i];
		outBuf[4 * i + 3] = inBuf[2 * i + 1];
	}
}

static succeed_type libaudio_write_file(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int len, FILE *fp)
{
	unsigned char readbuf[2048];
	unsigned short *ptrshort = NULL;
	int i = 0;

	if (phandle == NULL)
	{
		SYS_ERROR("ptrhandle_gbshm is NULL.\r\n");
		return succeed_type_failed;
	}

	memset(readbuf, 0, 2048);
	//1. decoder
	ptrshort = readbuf;
	switch(audiotype)
	{
		case AUDIO_A711:
			for ( i = 0; i < len; i++)
			{
				*(ptrshort+i) = G711_SUN_alaw2linear(outbuf[i] );
			}
			break;
		case AUDIO_U711:
			for ( i = 0; i < len; i++)
			{
				*(ptrshort+i) = G711_SUN_ulaw2linear(outbuf[i] );
			}
			break;
		default:
			break; 
	}

	//2. mono2stereo
	monoToStereo(readbuf, readbuf, len);

	fwrite(readbuf, len*4, 1, fp);

	//3. write
	if ( write(phandle->audio_wfd, readbuf,  len*4) == -1 )
	{
		SYS_ERROR("write failed.\r\n");
		return succeed_type_failed;
	}

	return succeed_type_succeed;
}

static succeed_type libaudio_write(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int len)
{
	unsigned char readbuf[2048];
	unsigned short *ptrshort = NULL;
	int i = 0;

	int ret;
	if (phandle == NULL)
	{
		SYS_ERROR("ptrhandle_gbshm is NULL.\r\n");
		return succeed_type_failed;
	}

	memset(readbuf, 0, 2048);
	//1. decoder
	ptrshort = readbuf;
	switch(audiotype)
	{
		case AUDIO_A711:
			for ( i = 0; i < len; i++)
			{
				*(ptrshort+i) = G711_SUN_alaw2linear(outbuf[i] );
			}
			break;
		case AUDIO_U711:
			for ( i = 0; i < len; i++)
			{
				*(ptrshort+i) = G711_SUN_ulaw2linear(outbuf[i] );
			}
			break;
		default:
			break; 
	}

	//2. mono2stereo
	monoToStereo(readbuf, readbuf, len);
	//3. write

	ret = write(phandle->audio_wfd, readbuf,  len*4); 
	if ( ret == -1 )
	{
		SYS_ERROR("write failed.\r\n");
		return succeed_type_failed;
	}

	return succeed_type_succeed;
}

static succeed_type libaudio_read(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int *retlen)
{
	unsigned char readbuf[2048];
	short *ptrshort = NULL;
	int i ;
	int ret;

	if (phandle == NULL)
	{
		SYS_ERROR("ptrhandle_aduio is NULL.\r\n");
		return succeed_type_failed;
	}
	//1. read
	memset(readbuf, 0, 2048);
	ret  = read(phandle->audio_rfd, readbuf, 640);
//	printf("after read ret = %d..\r\n", ret);

	//2. streo2mono
	stereoToMono( readbuf, readbuf, 160);

	//3. encoder
	ptrshort = readbuf;
	switch(audiotype)
	{
		case AUDIO_A711:
			for ( i = 0; i < 160; i++)
			{
				outbuf[i] = G711_SUN_linear2alaw( *(ptrshort+i));
			}
			break;
		case AUDIO_U711:
			for ( i = 0; i < 160; i++)
			{
				outbuf[i] = G711_SUN_linear2ulaw( *(ptrshort+i));
			}
			break;
		default:
			break; 
	}

	*retlen = 160;
	return succeed_type_succeed;
}

static succeed_type libaudio_read_file(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int *retlen, FILE *fp)
{
	unsigned char readbuf[2048];
	short *ptrshort = NULL;
	int i ;
	if (phandle == NULL)
	{
		SYS_ERROR("ptrhandle_aduio is NULL.\r\n");
		return succeed_type_failed;
	}
	//1. read
	memset(readbuf, 0, 2048);

	fread ( readbuf, 640, 1, fp);

	//2. streo2mono
	stereoToMono( readbuf, readbuf, 160);

	//3. encoder
	ptrshort = readbuf;
	switch(audiotype)
	{
		case AUDIO_A711:
			for ( i = 0; i < 160; i++)
			{
				outbuf[i] = G711_SUN_linear2alaw( *(ptrshort+i));
			}
			break;
		case AUDIO_U711:
			for ( i = 0; i < 160; i++)
			{
				outbuf[i] = G711_SUN_linear2ulaw( *(ptrshort+i));
			}
			break;
		default:
			break; 
	}
	*retlen = 160;	
	return succeed_type_succeed;
}

LIBAUDIO_OBJ *AUDIO_INIT(int mode)
{
	return libaudio_init(mode);
}

succeed_type AUDIO_READ_FILE(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int *retlen, FILE *fp)
{
	return libaudio_read_file(phandle, outbuf, audiotype, retlen, fp);
}

succeed_type AUDIO_READ(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int *retlen)
{
	return libaudio_read(phandle, outbuf, audiotype, retlen);
}

succeed_type AUDIO_WRITE_FILE(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int len, FILE *fp)
{
	return libaudio_write_file(phandle, outbuf, audiotype, len, fp);
}

succeed_type AUDIO_WRITE(LIBAUDIO_OBJ *phandle, unsigned char *outbuf, AUDIO_TYPE audiotype, int len)
{
	return libaudio_write(phandle, outbuf, audiotype, len);
}

succeed_type AUDIO_DESTROY(LIBAUDIO_OBJ *phandle, int mode)
{
	return libaudio_destroy(phandle, mode);
}


