/*******************************************************
 *Arthur:	Zhang,Li,Feng
 *Date:		2014/11/19
 *Version:	V1.2.2
 *Note:		Defining Register of GPMC	
 *		Defining DataStructure of CharDev_Driver 
 *		Defining file_operation Structure
 *		
 *		TO print CS0-7 offset & size 
 *              Write value to 0x34x
 *******************************************************/
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/list.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <asm/cacheflush.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE >=KERNEL_VERSION(2,6,26)
#warning *** not a warning *** Note: LINUX_VERSION_CODE >=2.6.26

#define USE_CACHE_VOID_ARG
#define USE_CLASS_DEVICE
#define USE_CLASS_SIMPLE

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

#warning *** not a warning *** Note: 2.6.26 > LINUX_VERSION_CODE>=2.6.18

#define USE_CACHE_VOID_ARG
#define USE_CLASS_DEVICE
#define USE_CLASS_SIMPLE

#else /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

#warning *** not a warning *** Note: LINUX_VERSION_CODE <2.6.18

#define USE_CLASS_SIMPLE
#define USE_CLASS_DEVICE
#define USE_CACHE_VOID_ARG

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

/*
 *  Poor man's config params
 */

/*
 * NOCACHE means physical memory block is ioremap()'ed as noncached
 */
#define NOCACHE

/*
 * CHECK_FOR_ALLOCATED_BUFFER means ensure that the passed addr/size block
 * is actually an alocated, CMEM-defined buffer.
 */
//#define CHECK_FOR_ALLOCATED_BUFFER

#define HEAD_ALIGN PAGE_SIZE

#ifdef __DEBUG
#define __D(fmt, args...) printk(KERN_DEBUG "GPIO Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...) printk(KERN_ERR "GPIO Error: " fmt, ##args)

#define MAXTYPE(T) ((T) (((T)1 << ((sizeof(T) * 8) - 1) ^ ((T) -1 ))))

/*
 * Change here for supporting more than 2 blocks. Also change all
 * NBLOCKS-based arrays to have NBLOCKS-worth of initialization values.
 */
#define NBLOCKS 2

/*GPMC register offsets*/
#define GPMC_BASE		0x6E000000 //GPMC Register Base Address

#define GPMC_REVISION 		(0x00+GPMC_BASE)
#define GPMC_SYSCONFIG 		(0x10+GPMC_BASE)
#define GPMC_SYSSTATUS 	        (0x14+GPMC_BASE)
#define GPMC_IRQSTATUS 		(0x18+GPMC_BASE)
#define GPMC_IRQENABLE 		(0x1c+GPMC_BASE)
#define GPMC_TIMEOUT_CONTROL 	(0x40+GPMC_BASE)
#define GPMC_ERR_ADDRESS 	(0x44+GPMC_BASE)
#define GPMC_ERR_TYPE 		(0x48+GPMC_BASE)
#define GPMC_CONFIG 		(0x50+GPMC_BASE)
#define GPMC_STATUS 		(0x54+GPMC_BASE) 
//0x6E00 0078 + (0x0000 0030 * i)
/*GPMC_CONFIG_PHYSICAL_ADDRESS*/
#define GPMC_CONFIG_OFFSET	0x00000030
#define GPMC_CONFIG1(i) 	(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000060))
#define GPMC_CONFIG2(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000064)) 
#define GPMC_CONFIG3(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000068)) 
#define GPMC_CONFIG4(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x0000006C))
#define GPMC_CONFIG5(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000070))
#define GPMC_CONFIG6(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000074))
#define GPMC_CONFIG7(i)		(GPMC_BASE+((GPMC_CONFIG_OFFSET*i)+0x00000078))

#define GPMC_PREFETCH_CONFIG1 	(0x1E0+GPMC_BASE)
#define GPMC_PREFETCH_CONFIG2 	(0x1E4+GPMC_BASE)
#define GPMC_PREFETCH_CONTROL 	(0x1EC+GPMC_BASE)
#define GPMC_PREFETCH_STATUS 	(0x1F0+GPMC_BASE)

#define GPMC_ECC_CONFIG 	0x1F4+GPMC_BASE
#define GPMC_ECC_CONTROL 	0x1F8+GPMC_BASE
#define GPMC_ECC_SIZE_CONFIG 	0x1FC+GPMC_BASE
#define GPMC_ECC1_RESULT 	0x200+GPMC_BASE
#define GPMC_ECC_BCH_RESULT_0 	0x240+GPMC_BASE

#define GPMC_SIZE 0x4


/*Device Name*/
#define DEVICE_NAME	"gpmcCtl"

static void __iomem * io_base3_test;
static void __iomem * io_base[8];
static void __iomem * io_base0;
static unsigned long io_cntl;

unsigned long mem_base;

/*Structure of GPMC Device*/
struct _gpmc_dev {
		dev_t devt;
		struct cdev cdev;
		struct semaphore sem;
		struct class *class;
		char *user_buff;
};
static struct _gpmc_dev* gpmc_dev;

/*Structure of GPMC Device Number*/
struct _gpmc_dev_num{
		dev_t devNum;
		unsigned int major;
		unsigned int minor;
		unsigned int minor_first;
		unsigned int count;
};
static struct _gpmc_dev_num gpmc_dev_num;

/*GPMC_Read Operation*/
static ssize_t gpmc_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
		return 0;
}

/*GPMC_Write Operation*/
static ssize_t gpmc_write(struct file *filp, const char __user *buff,size_t count, loff_t *f_pos)
{
		return 0;
}

/*GPMC_Open Operation*/
static ssize_t gpmc_open(struct inode *inode, struct file *filp)
{
		return 0;
}

/*File Operation Structure*/
static const struct file_operations gpmc_fops = {
		.owner=THIS_MODULE,
		.open=gpmc_open,
		.read=gpmc_read,
		.write=gpmc_write,
};

/*cdev_register Func*/
static void gpmc_setup_cdev(struct _gpmc_dev *dev, int index)
{
		int ret;
		int devno = gpmc_dev_num.devNum + index;
		cdev_init(&dev->cdev, &gpmc_fops);
		dev->cdev.owner = THIS_MODULE;
		dev->cdev.ops = &gpmc_fops;

		ret = cdev_add(&dev->cdev, devno, 1);

		if(ret)
				printk(KERN_ERR "error %d : adding gpmcCtl%d",ret,index);
}

typedef struct _GPMC_REG
{
		unsigned int GPMC_CONFIG1;
		unsigned int GPMC_CONFIG2;
		unsigned int GPMC_CONFIG3;
		unsigned int GPMC_CONFIG4;
		unsigned int GPMC_CONFIG5;
		unsigned int GPMC_CONFIG6;
		unsigned int GPMC_CONFIG7;
}GPMC_REG;

#define GPMC_CS0_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*0)
#define GPMC_CS1_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*1)
#define GPMC_CS2_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*2)
#define GPMC_CS3_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*3)
#define GPMC_CS4_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*4)
#define GPMC_CS5_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*5)
#define GPMC_CS6_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*6)
#define GPMC_CS7_CONFIG_BASE_ADDR	(0x6E000060 + 0x30*7)

typedef struct _GPMC_CONFIG_REGS_STRUCT
{
		GPMC_REG	*pGPMC_CS0_CONFIG;
		GPMC_REG	*pGPMC_CS1_CONFIG;
		GPMC_REG	*pGPMC_CS2_CONFIG;
		GPMC_REG	*pGPMC_CS3_CONFIG;
		GPMC_REG	*pGPMC_CS4_CONFIG;
		GPMC_REG	*pGPMC_CS5_CONFIG;
		GPMC_REG	*pGPMC_CS6_CONFIG;
		GPMC_REG	*pGPMC_CS7_CONFIG;
}GPMC_CONFIG_REGS_STRUCT;

GPMC_CONFIG_REGS_STRUCT global_config_regs;


//cs_num: 0-7
//cs_phy_base: 0x00-0x3F
//cs_size:0x00, 0x08, 0x0C, 0x0E, 0x0F
//width: 0--8bit, 1--16bit
//mode: 0--no multiplex, 1 -- data addr multiplex
static void cs_enable(unsigned int cs_num, unsigned int cs_phy_base, unsigned int cs_size, unsigned int width, unsigned int mode)
{
		unsigned int tempvalue = 0x00;
		GPMC_REG *pgpmc_reg = 	NULL;
		switch (cs_num)
		{
				case 0:
						pgpmc_reg = 	global_config_regs.pGPMC_CS0_CONFIG ;
						break;
				case 1:
						pgpmc_reg = 	global_config_regs.pGPMC_CS1_CONFIG ;
						break;
				case 2:
						pgpmc_reg = 	global_config_regs.pGPMC_CS2_CONFIG ;
						break;
				case 3:
						pgpmc_reg = 	global_config_regs.pGPMC_CS3_CONFIG ;
						break;
				case 4:
						pgpmc_reg = 	global_config_regs.pGPMC_CS4_CONFIG ;
						break;
				case 5:
						pgpmc_reg = 	global_config_regs.pGPMC_CS5_CONFIG ;
						break;
				case 6:
						pgpmc_reg = 	global_config_regs.pGPMC_CS6_CONFIG ;
						break;
				case 7:
						pgpmc_reg = 	global_config_regs.pGPMC_CS7_CONFIG ;
						break;
				default:
						break;
		}
		pgpmc_reg->GPMC_CONFIG7 = (cs_size << 8) | cs_phy_base | 0x40;
	if(cs_num==3)	
	{
		pgpmc_reg->GPMC_CONFIG1 = 0x1000;
		pgpmc_reg->GPMC_CONFIG2 = 0x001e1e00;
		pgpmc_reg->GPMC_CONFIG3 = 0x00080300;
		pgpmc_reg->GPMC_CONFIG4 = 0x1c091c09;
		pgpmc_reg->GPMC_CONFIG5 = 0x04181f1f;
		pgpmc_reg->GPMC_CONFIG6 = 0x00000fcf;
	}
#if 0
		tempvalue = pgpmc_reg->GPMC_CONFIG1;
		tempvalue &=~(1<<9 | 3 <<12);
		tempvalue |= (width <<12 | mode << 9);
		pgpmc_reg->GPMC_CONFIG1  = tempvalue;

		if (cs_num == 3)
		{
				pgpmc_reg->GPMC_CONFIG2 = 0x90901;
				pgpmc_reg->GPMC_CONFIG3 = 0x80300;
				pgpmc_reg->GPMC_CONFIG4 = 0x9010901;
				pgpmc_reg->GPMC_CONFIG5 = 0x3080a0a;
				pgpmc_reg->GPMC_CONFIG6 = 0x880002C7;
		}
#endif

}
static void gpmc_regs_map(void)
{
		memset(&global_config_regs, 0x00, sizeof(global_config_regs));
		global_config_regs.pGPMC_CS0_CONFIG = (GPMC_REG *)ioremap (GPMC_CS0_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS1_CONFIG = (GPMC_REG *)ioremap (GPMC_CS1_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS2_CONFIG = (GPMC_REG *)ioremap (GPMC_CS2_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS3_CONFIG = (GPMC_REG *)ioremap (GPMC_CS3_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS4_CONFIG = (GPMC_REG *)ioremap (GPMC_CS4_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS5_CONFIG = (GPMC_REG *)ioremap (GPMC_CS5_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS6_CONFIG = (GPMC_REG *)ioremap (GPMC_CS6_CONFIG_BASE_ADDR, sizeof(GPMC_REG));
		global_config_regs.pGPMC_CS7_CONFIG = (GPMC_REG *)ioremap (GPMC_CS7_CONFIG_BASE_ADDR, sizeof(GPMC_REG));


}
static void gpmc_regs_init(void)
{
		unsigned int i = 0x00;
		printk(" CS0 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG7);
		if  (	global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG7 & 0x40  )
		{
				unsigned int sizevalue = (global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG7  >> 8) & 0x0F;
				printk("CS0 is valid \r\n");
				printk("CS0 base phy addr = %dM\r\n", (global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG7 & 0x3F) * 16);
				switch ( sizevalue )
				{
						case 0x00:
								printk("CS0 size 256M\r\n");
								break;
						case 0x08:
								printk("CS0 size 128M\r\n");
								break;
						case 0x0C:
								printk("CS0 size 64M\r\n");
								break;
						case 0x0E:
								printk("CS0 size 32M\r\n");
								break;
						case 0x0F:
								printk("CS0 size 16M\r\n");
								break;
						default:
								printk("CS0 size invalid, sizevalue = 0x%x\r\n", sizevalue);
								break;
				}
				if (global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG1 >>9 & 0x01 )
				{
						printk("CS0 data and addr is mulpx\r\n");
				}
				else
				{
						printk("CS0 data and addr is no-mulpx\r\n");
				}
				if (global_config_regs.pGPMC_CS0_CONFIG->GPMC_CONFIG1 >>12 & 0x01 )
				{
						printk("CS0 data width is 16\r\n");
				}
				else
				{
						printk("CS0 data width is 8\r\n");
				}

		}
		else
		{
				printk("CS0 is invalid\r\n");
		}

		printk(" CS1 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG7);
		if  (	global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG7 & 0x40  )
		{
				unsigned int sizevalue = (global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG7  >> 8) & 0x0F;
				printk("CS1 is valid \r\n");
				printk("CS1 base phy addr = %dM\r\n", (global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG7 & 0x3F) * 16);
				switch ( sizevalue )
				{
						case 0x0000:
								printk("CS1 size 256M\r\n");
								break;
						case 0x08:
								printk("CS1 size 128M\r\n");
								break;
						case 0x0C:
								printk("CS1 size 64M\r\n");
								break;
						case 0x0E:
								printk("CS1 size 32M\r\n");
								break;
						case 0x0F:
								printk("CS1 size 16M\r\n");
								break;
						default:
								printk("CS1 size invalid\r\n");
								break;
				}
				if (global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG1 >>9 & 0x01 )
				{
						printk("CS1 data and addr is mulpx\r\n");
				}
				else
				{
						printk("CS1 data and addr is no-mulpx\r\n");
				}
				if (global_config_regs.pGPMC_CS1_CONFIG->GPMC_CONFIG1 >>12 & 0x01 )
				{
						printk("CS1 data width is 16\r\n");
				}
				else
				{
						printk("CS1 data width is 8\r\n");
				}

		}
		else
		{
				printk("CS1 is invalid\r\n");
		}

		printk(" CS2 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS2_CONFIG->GPMC_CONFIG7);

		printk(" CS3 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG1);
		global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG1 &= 0xFFFFE7FF ;
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG7);
		if  (	global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG7 & 0x40  )
		{
				unsigned int sizevalue = (global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG7  >> 8) & 0x0F;
				printk("CS3 is valid \r\n");
				printk("CS3 base phy addr = %dM\r\n", (global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG7 & 0x3F) * 16);
				switch ( sizevalue )
				{
						case 0x00:
								printk("CS3 size 256M\r\n");
								break;
						case 0x08:
								printk("CS3 size 128M\r\n");
								break;
						case 0x0C:
								printk("CS3 size 64M\r\n");
								break;
						case 0x0E:
								printk("CS3 size 32M\r\n");
								break;
						case 0x0F:
								printk("CS3 size 16M\r\n");
								break;
						default:
								printk("CS3 size invalid\r\n");
								break;
				}
				if (global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG1 >>9 & 0x01 )
				{
						printk("CS3 data and addr is mulpx\r\n");
				}
				else
				{
						printk("CS3 data and addr is no-mulpx\r\n");
				}
				if (global_config_regs.pGPMC_CS3_CONFIG->GPMC_CONFIG1 >>12 & 0x01 )
				{
						printk("CS3 data width is 16\r\n");
				}
				else
				{
						printk("CS3 data width is 8\r\n");
				}

		}
		else
		{
				printk("CS3 is invalid\r\n");
		}

		printk(" CS4 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG7);
		if  (	global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG7 & 0x40  )
		{
				unsigned int sizevalue = (global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG7  >> 8) & 0x0F;
				printk("CS4 is valid \r\n");
				printk("CS4 base phy addr = %dM\r\n", (global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG7 & 0x3F) * 16);
				switch ( sizevalue )
				{
						case 0x0000:
								printk("CS4 size 256M\r\n");
								break;
						case 0x08:
								printk("CS4 size 128M\r\n");
								break;
						case 0x0C:
								printk("CS4 size 64M\r\n");
								break;
						case 0x0E:
								printk("CS4 size 32M\r\n");
								break;
						case 0x0F:
								printk("CS4 size 16M\r\n");
								break;
						default:
								printk("CS4 size invalid\r\n");
								break;
				}
				if (global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG1 >>9 & 0x01 )
				{
						printk("CS4 data and addr is mulpx\r\n");
				}
				else
				{
						printk("CS4 data and addr is no-mulpx\r\n");
				}
				if (global_config_regs.pGPMC_CS4_CONFIG->GPMC_CONFIG1 >>12 & 0x01 )
				{
						printk("CS4 data width is 16\r\n");
				}
				else
				{
						printk("CS4 data width is 8\r\n");
				}

		}
		else
		{
				printk("CS1 is invalid\r\n");
		}


		printk(" CS5 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS5_CONFIG->GPMC_CONFIG7);

		printk(" CS6 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7);
		if  (	global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7 & 0x40  )
		{
				unsigned int sizevalue = (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7  >> 8) & 0x0F;
				unsigned int *preg = (unsigned int *)(ioremap( (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7 & 0x3F) << 24, 0x30));

				printk("CS6 is valid , *preg = 0x%x\r\n", *preg);
				printk("CS6 base phy addr = %dM\r\n", (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7 & 0x3F) * 16);
				switch ( sizevalue )
				{
						case 0x00:
								printk("CS6 size 256M\r\n");
								break;
						case 0x08:
								printk("CS6 size 128M\r\n");
								break;
						case 0x0C:
								printk("CS6 size 64M\r\n");
								break;
						case 0x0E:
								printk("CS6 size 32M\r\n");
								break;
						case 0x0F:
								printk("CS6 size 16M\r\n");
								break;
						default:
								printk("CS6 size invalid\r\n");
								break;
				}
				if (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG1 >>9 & 0x01 )
				{
						printk("CS6 data and addr is mulpx\r\n");
				}
				else
				{
						printk("CS6 data and addr is no-mulpx\r\n");
				}
				if (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG1 >>12 & 0x01 )
				{
						printk("CS6 data width is 16\r\n");
				}
				else
				{
						printk("CS6 data width is 8\r\n");
				}
		}
		printk(" CS7 CONFIG regs value :\r\n");
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG1 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG1);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG2 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG2);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG3 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG3);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG4 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG4);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG5 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG5);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG6 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG6);
		printk(" global_config_regs.pGMPC_CS0_CONFIG->GPMC_CONFIG7 = 0x%x\r\n", global_config_regs.pGPMC_CS7_CONFIG->GPMC_CONFIG7);


}
static void gpmc_regs_deinit(void)
{
		iounmap(global_config_regs.pGPMC_CS0_CONFIG);
		iounmap(global_config_regs.pGPMC_CS1_CONFIG);
		iounmap(global_config_regs.pGPMC_CS2_CONFIG);
		iounmap(global_config_regs.pGPMC_CS3_CONFIG);
		iounmap(global_config_regs.pGPMC_CS4_CONFIG);
		iounmap(global_config_regs.pGPMC_CS5_CONFIG);
		iounmap(global_config_regs.pGPMC_CS6_CONFIG);
		iounmap(global_config_regs.pGPMC_CS7_CONFIG);

}

typedef struct _COM_REGS
{
		unsigned char reg1;
		unsigned char reg2;
		unsigned char reg3;
		unsigned char reg4;
		unsigned char reg5;
		unsigned char reg6;
		unsigned char reg7;
		unsigned char reg8;
}COM_REGS;

COM_REGS *pcom0_regs;
COM_REGS *pcom1_regs;
COM_REGS *pcom2_regs;
COM_REGS *pcom3_regs;
COM_REGS *pcom4_regs;
COM_REGS *pcom5_regs;
COM_REGS *pcom6_regs;
COM_REGS *pcom7_regs;

#define D16C554_COM0_OFFSET	0x380
#define D16C554_COM1_OFFSET	0x388
#define D16C554_COM2_OFFSET	0x390
#define D16C554_COM3_OFFSET	0x398
#define D16C554_COM4_OFFSET	0x3a0
#define D16C554_COM5_OFFSET	0x3a8
#define D16C554_COM6_OFFSET	0x3b0
#define D16C554_COM7_OFFSET	0x3b8
#define D16C554_STATUS_OFFSET	0x3c0
#define GPMC_CS3_PHY_BASE	0x01
#define GPMC_CS4_PHY_BASE	0x06

static void gpmc_com_test(void)
{
		unsigned int i = 0;
		unsigned int j = 0;
	//	24000000
		unsigned int pcom_regs_base = ioremap(GPMC_CS3_PHY_BASE << 24, 0x400);
		unsigned int pcom_regs_base2 = ioremap( (global_config_regs.pGPMC_CS6_CONFIG->GPMC_CONFIG7 & 0x3F) << 24, 0x400);
		pcom0_regs =(COM_REGS*) (pcom_regs_base + 0x380);
		pcom1_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM1_OFFSET);
		pcom2_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM2_OFFSET);
		pcom3_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM3_OFFSET);
		pcom4_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM4_OFFSET);
		pcom5_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM5_OFFSET);
		pcom6_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM6_OFFSET);
		pcom7_regs = (COM_REGS *)(pcom_regs_base + D16C554_COM7_OFFSET);


		writeb(0x01,&pcom0_regs->reg1);
//		printk("pcom0_regs->reg1=0x%x\n\r",pcom0_regs->reg1);
		writeb(0x02,&pcom0_regs->reg2);
//		printk("pcom0_regs->reg2=0x%x\n\r",pcom0_regs->reg2);
		writeb(0x03,&pcom0_regs->reg3);
//		printk("pcom0_regs->reg3=0x%x\n\r",pcom0_regs->reg3);
		writeb(0x04,&pcom0_regs->reg4);
//		printk("pcom0_regs->reg4=0x%x\n\r",pcom0_regs->reg4);
		writeb(0x05,&pcom0_regs->reg5);
//		printk("pcom0_regs->reg5=0x%x\n\r",pcom0_regs->reg5);
		writeb(0x06,&pcom0_regs->reg6);
//		printk("pcom0_regs->reg6=0x%x\n\r",pcom0_regs->reg6);
		writeb(0x07,&pcom0_regs->reg7);
//		printk("pcom0_regs->reg7=0x%x\n\r",pcom0_regs->reg7);

		writeb(0x11,&pcom1_regs->reg1);
//		printk("pcom1_regs->reg1=0x%x\n\r",pcom1_regs->reg1);
		writeb(0x12,&pcom1_regs->reg2);
//		printk("pcom1_regs->reg2=0x%x\n\r",pcom1_regs->reg2);
		writeb(0x13,&pcom1_regs->reg3);
//		printk("pcom1_regs->reg3=0x%x\n\r",pcom1_regs->reg3);
		writeb(0x14,&pcom1_regs->reg4);
//		printk("pcom1_regs->reg4=0x%x\n\r",pcom1_regs->reg4);
		writeb(0x15,&pcom1_regs->reg5);
//		printk("pcom1_regs->reg5=0x%x\n\r",pcom1_regs->reg5);
		writeb(0x16,&pcom1_regs->reg6);
//		printk("pcom1_regs->reg6=0x%x\n\r",pcom1_regs->reg6);
		writeb(0x17,&pcom1_regs->reg7);
//		printk("pcom1_regs->reg7=0x%x\n\r",pcom1_regs->reg7);


	writeb(0x21,&pcom2_regs->reg1);
//		printk("pcom2_regs->reg1=0x%x\n\r",pcom2_regs->reg1);
		writeb(0x22,&pcom2_regs->reg2);
//		printk("pcom2_regs->reg2=0x%x\n\r",pcom2_regs->reg2);
		writeb(0x23,&pcom2_regs->reg3);
//		printk("pcom2_regs->reg3=0x%x\n\r",pcom2_regs->reg3);
		writeb(0x24,&pcom2_regs->reg4);
//		printk("pcom2_regs->reg4=0x%x\n\r",pcom2_regs->reg4);
		writeb(0x25,&pcom2_regs->reg5);
//		printk("pcom2_regs->reg5=0x%x\n\r",pcom2_regs->reg5);
		writeb(0x26,&pcom2_regs->reg6);
//		printk("pcom2_regs->reg6=0x%x\n\r",pcom2_regs->reg6);
		writeb(0x27,&pcom2_regs->reg7);
//		printk("pcom2_regs->reg7=0x%x\n\r",pcom2_regs->reg7);

	writeb(0x31,&pcom3_regs->reg1);
//		printk("pcom3_regs->reg1=0x%x\n\r",pcom3_regs->reg1);
		writeb(0x32,&pcom3_regs->reg2);
//		printk("pcom3_regs->reg2=0x%x\n\r",pcom3_regs->reg2);
		writeb(0x33,&pcom3_regs->reg3);
//		printk("pcom3_regs->reg3=0x%x\n\r",pcom3_regs->reg3);
		writeb(0x34,&pcom3_regs->reg4);
//		printk("pcom3_regs->reg4=0x%x\n\r",pcom3_regs->reg4);
		writeb(0x35,&pcom3_regs->reg5);
//		printk("pcom3_regs->reg5=0x%x\n\r",pcom3_regs->reg5);
		writeb(0x36,&pcom3_regs->reg6);
//		printk("pcom3_regs->reg6=0x%x\n\r",pcom3_regs->reg6);
		writeb(0x37,&pcom3_regs->reg7);
//		printk("pcom3_regs->reg7=0x%x\n\r",pcom3_regs->reg7);

	writeb(0x41,&pcom4_regs->reg1);
//		printk("pcom4_regs->reg1=0x%x\n\r",pcom4_regs->reg1);
		writeb(0x42,&pcom4_regs->reg2);
//		printk("pcom4_regs->reg2=0x%x\n\r",pcom4_regs->reg2);
		writeb(0x43,&pcom4_regs->reg3);
//		printk("pcom4_regs->reg3=0x%x\n\r",pcom4_regs->reg3);
		writeb(0x44,&pcom4_regs->reg4);
//		printk("pcom4_regs->reg4=0x%x\n\r",pcom4_regs->reg4);
		writeb(0x45,&pcom4_regs->reg5);
//		printk("pcom4_regs->reg5=0x%x\n\r",pcom4_regs->reg5);
		writeb(0x46,&pcom4_regs->reg6);
//		printk("pcom4_regs->reg6=0x%x\n\r",pcom4_regs->reg6);
		writeb(0x47,&pcom4_regs->reg7);
//		printk("pcom4_regs->reg7=0x%x\n\r",pcom4_regs->reg7);

	writeb(0x51,&pcom5_regs->reg1);
//		printk("pcom5_regs->reg1=0x%x\n\r",pcom5_regs->reg1);
		writeb(0x52,&pcom5_regs->reg2);
//		printk("pcom5_regs->reg2=0x%x\n\r",pcom5_regs->reg2);
		writeb(0x53,&pcom5_regs->reg3);
//		printk("pcom5_regs->reg3=0x%x\n\r",pcom5_regs->reg3);
		writeb(0x54,&pcom5_regs->reg4);
//		printk("pcom5_regs->reg4=0x%x\n\r",pcom5_regs->reg4);
		writeb(0x55,&pcom5_regs->reg5);
//		printk("pcom5_regs->reg5=0x%x\n\r",pcom5_regs->reg5);
		writeb(0x56,&pcom5_regs->reg6);
//		printk("pcom5_regs->reg6=0x%x\n\r",pcom5_regs->reg6);
		writeb(0x57,&pcom5_regs->reg7);
//		printk("pcom5_regs->reg7=0x%x\n\r",pcom5_regs->reg7);

	writeb(0x61,&pcom6_regs->reg1);
//		printk("pcom6_regs->reg1=0x%x\n\r",pcom6_regs->reg1);
		writeb(0x62,&pcom6_regs->reg2);
//		printk("pcom6_regs->reg2=0x%x\n\r",pcom6_regs->reg2);
		writeb(0x63,&pcom6_regs->reg3);
//		printk("pcom6_regs->reg3=0x%x\n\r",pcom6_regs->reg3);
		writeb(0x64,&pcom6_regs->reg4);
//		printk("pcom6_regs->reg4=0x%x\n\r",pcom6_regs->reg4);
		writeb(0x65,&pcom6_regs->reg5);
//		printk("pcom6_regs->reg5=0x%x\n\r",pcom6_regs->reg5);
		writeb(0x66,&pcom6_regs->reg6);
//		printk("pcom6_regs->reg6=0x%x\n\r",pcom6_regs->reg6);
		writeb(0x67,&pcom6_regs->reg7);
//		printk("pcom6_regs->reg7=0x%x\n\r",pcom6_regs->reg7);

	writeb(0x71,&pcom7_regs->reg1);
//		printk("pcom7_regs->reg1=0x%x\n\r",pcom7_regs->reg1);
		writeb(0x72,&pcom7_regs->reg2);
//		printk("pcom7_regs->reg2=0x%x\n\r",pcom7_regs->reg2);
		writeb(0x73,&pcom7_regs->reg3);
//		printk("pcom7_regs->reg3=0x%x\n\r",pcom7_regs->reg3);
		writeb(0x74,&pcom7_regs->reg4);
//		printk("pcom7_regs->reg4=0x%x\n\r",pcom7_regs->reg4);
		writeb(0x75,&pcom7_regs->reg5);
//		printk("pcom7_regs->reg5=0x%x\n\r",pcom7_regs->reg5);
		writeb(0x76,&pcom7_regs->reg6);
//		printk("pcom7_regs->reg6=0x%x\n\r",pcom7_regs->reg6);
		writeb(0x77,&pcom7_regs->reg7);
//		printk("pcom7_regs->reg7=0x%x\n\r",pcom7_regs->reg7);




		printk("pcom0_regs->reg1=0x%x\n\r",pcom0_regs->reg1);
		printk("pcom0_regs->reg2=0x%x\n\r",pcom0_regs->reg2);
		printk("pcom0_regs->reg3=0x%x\n\r",pcom0_regs->reg3);
		printk("pcom0_regs->reg4=0x%x\n\r",pcom0_regs->reg4);
		printk("pcom0_regs->reg5=0x%x\n\r",pcom0_regs->reg5);
		printk("pcom0_regs->reg6=0x%x\n\r",pcom0_regs->reg6);
		printk("pcom0_regs->reg7=0x%x\n\r",pcom0_regs->reg7);

		printk("pcom1_regs->reg1=0x%x\n\r",pcom1_regs->reg1);
		printk("pcom1_regs->reg2=0x%x\n\r",pcom1_regs->reg2);
		printk("pcom1_regs->reg3=0x%x\n\r",pcom1_regs->reg3);
		printk("pcom1_regs->reg4=0x%x\n\r",pcom1_regs->reg4);
		printk("pcom1_regs->reg5=0x%x\n\r",pcom1_regs->reg5);
		printk("pcom1_regs->reg6=0x%x\n\r",pcom1_regs->reg6);
		printk("pcom1_regs->reg7=0x%x\n\r",pcom1_regs->reg7);

		printk("pcom2_regs->reg1=0x%x\n\r",pcom2_regs->reg1);
		printk("pcom2_regs->reg2=0x%x\n\r",pcom2_regs->reg2);
		printk("pcom2_regs->reg3=0x%x\n\r",pcom2_regs->reg3);
		printk("pcom2_regs->reg4=0x%x\n\r",pcom2_regs->reg4);
		printk("pcom2_regs->reg5=0x%x\n\r",pcom2_regs->reg5);
		printk("pcom2_regs->reg6=0x%x\n\r",pcom2_regs->reg6);
		printk("pcom2_regs->reg7=0x%x\n\r",pcom2_regs->reg7);

		printk("pcom3_regs->reg1=0x%x\n\r",pcom3_regs->reg1);
		printk("pcom3_regs->reg2=0x%x\n\r",pcom3_regs->reg2);
		printk("pcom3_regs->reg3=0x%x\n\r",pcom3_regs->reg3);
		printk("pcom3_regs->reg4=0x%x\n\r",pcom3_regs->reg4);
		printk("pcom3_regs->reg5=0x%x\n\r",pcom3_regs->reg5);
		printk("pcom3_regs->reg6=0x%x\n\r",pcom3_regs->reg6);
		printk("pcom3_regs->reg7=0x%x\n\r",pcom3_regs->reg7);

		printk("pcom4_regs->reg1=0x%x\n\r",pcom4_regs->reg1);
		printk("pcom4_regs->reg2=0x%x\n\r",pcom4_regs->reg2);
		printk("pcom4_regs->reg3=0x%x\n\r",pcom4_regs->reg3);
		printk("pcom4_regs->reg4=0x%x\n\r",pcom4_regs->reg4);
		printk("pcom4_regs->reg5=0x%x\n\r",pcom4_regs->reg5);
		printk("pcom4_regs->reg6=0x%x\n\r",pcom4_regs->reg6);
		printk("pcom4_regs->reg7=0x%x\n\r",pcom4_regs->reg7);

		printk("pcom5_regs->reg1=0x%x\n\r",pcom5_regs->reg1);
		printk("pcom5_regs->reg2=0x%x\n\r",pcom5_regs->reg2);
		printk("pcom5_regs->reg3=0x%x\n\r",pcom5_regs->reg3);
		printk("pcom5_regs->reg4=0x%x\n\r",pcom5_regs->reg4);
		printk("pcom5_regs->reg5=0x%x\n\r",pcom5_regs->reg5);
		printk("pcom5_regs->reg6=0x%x\n\r",pcom5_regs->reg6);
		printk("pcom5_regs->reg7=0x%x\n\r",pcom5_regs->reg7);

		printk("pcom6_regs->reg1=0x%x\n\r",pcom6_regs->reg1);
		printk("pcom6_regs->reg2=0x%x\n\r",pcom6_regs->reg2);
		printk("pcom6_regs->reg3=0x%x\n\r",pcom6_regs->reg3);
		printk("pcom6_regs->reg4=0x%x\n\r",pcom6_regs->reg4);
		printk("pcom6_regs->reg5=0x%x\n\r",pcom6_regs->reg5);
		printk("pcom6_regs->reg6=0x%x\n\r",pcom6_regs->reg6);
		printk("pcom6_regs->reg7=0x%x\n\r",pcom6_regs->reg7);

		printk("pcom7_regs->reg1=0x%x\n\r",pcom7_regs->reg1);
		printk("pcom7_regs->reg2=0x%x\n\r",pcom7_regs->reg2);
		printk("pcom7_regs->reg3=0x%x\n\r",pcom7_regs->reg3);
		printk("pcom7_regs->reg4=0x%x\n\r",pcom7_regs->reg4);
		printk("pcom7_regs->reg5=0x%x\n\r",pcom7_regs->reg5);
		printk("pcom7_regs->reg6=0x%x\n\r",pcom7_regs->reg6);
		printk("pcom7_regs->reg7=0x%x\n\r",pcom7_regs->reg7);








#if 0
		if (!request_mem_region(mem_base, SZ_4K, "mem_fpga")) {
				printk(KERN_ERR "Request_mem_region failed.\n");
			//	gpmc_cs_free(GPMC_CS);
				return -1;
		}

		fpga_base = ioremap(mem_base, SZ_4K); 


		printk("membase:%x fpgabase:%x \n",mem_base,fpga_base);
#endif

#if 0
		//	for (i = 0; i < 0xFFF; i++)
		{
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg1);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg2);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg3);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg4);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg5);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg6);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg7);
				//for (j = 0; j < 0x100; j++);
				pcom0_regs->reg1 = 0x11;
				pcom0_regs->reg2 = 0x22;
				pcom0_regs->reg3 = 0x33;
				pcom0_regs->reg4 = 0x44;
				pcom0_regs->reg5 = 0x55;
				pcom0_regs->reg6 = 0x66;
				pcom0_regs->reg7 = 0x77;
			//	pcom0_regs->reg8 = 0x88;
				//for (j = 0; j < 0x100; j++);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg1);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg2);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg3);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg4);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg5);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg6);
				printk("cs3 value : 0x%x\r\n", pcom0_regs->reg7);
		}
#endif
#if 0
		j=0;
		short tmp=0;
		//		while(j++<10000)     
		{  
				// sleep(1000);        

				//writew(0xA5,fpga_base+0x3D0);
				tmp=readw (fpga_base + 0x3D0);
				printk("BIST:%4x\n",tmp);
		}
#endif
#if 0
		printk("3d0 is 0x%x....\r\n", (int *)ioremap(0x3d0, 4));
		unsigned int *p = ioremap(0x3d0, 4);
		*p = 0x11;
		printk("3d0 is 0x%x....\r\n", ioremap(0x3d0, 4));
#endif

#if 0
		pcom0_regs->reg1 = 0x11;
		pcom0_regs->reg2 = 0x22;
		pcom0_regs->reg3 = 0x33;
		pcom0_regs->reg4 = 0x44;
		pcom0_regs->reg5 = 0x55;
		pcom0_regs->reg6 = 0x66;
		pcom0_regs->reg7 = 0x77;
		pcom0_regs->reg8 = 0x88;
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg1);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg2);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg3);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg4);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg5);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg6);
		printk("cs3 value : 0x%x\r\n", pcom0_regs->reg7);
#endif
		//printk("cs6 value : 0x%x\r\n", pcom1_regs->reg1);
		iounmap(pcom_regs_base);
		iounmap(pcom_regs_base2);
}

/*gpmc device init*/
static int __init gpmc_init(void)
{
		int ret;
		gpmc_dev_num.count = 1;
		gpmc_dev_num.minor_first = 0;
		ret = alloc_chrdev_region(&gpmc_dev_num.devNum, gpmc_dev_num.minor_first, gpmc_dev_num.count, DEVICE_NAME);
		if(ret < 0)
				return ret;

		gpmc_dev_num.major = MAJOR(gpmc_dev_num.devNum);
		gpmc_dev_num.minor = MINOR(gpmc_dev_num.devNum);
		gpmc_dev = kzalloc(sizeof(struct _gpmc_dev),GFP_KERNEL); 
		gpmc_setup_cdev(gpmc_dev, 0); 
		printk(KERN_ERR "gpmc alloc_chrdev_region success, major = %d\n", gpmc_dev_num.major); 

		int i=3;
		/*for(i=0;i<=7;i++)
		  {
		  io_base[i]=ioremap(GPMC_CONFIG7(i),GPMC_SIZE);
		  }*/
#if 0
		io_base[3]=ioremap(GPMC_CONFIG7(i),0x4);
		unsigned long CONFIG7_3_REG;
		CONFIG7_3_REG=readl(io_base[3]);
		printk("CONFIG7_3_REG=%x\r\n",CONFIG7_3_REG);
		writel(0x00000C40 | GPMC_CS3_PHY_BASE,io_base[3]);//configure CS3: size=64M start_addr=0x00000000
		CONFIG7_3_REG=readl(io_base[3]);
		printk("CONFIG7_3_REG=%x\r\n",CONFIG7_3_REG);
		iounmap(io_base[3]);
#endif
		/*unsigned long config7[8];
		  for(i=0;i<=7;i++)
		  {
		  config7[i]=readl(io_base[i]);
		  printk("config%d=%x\n",i,config7[i]);
		  iounmap(io_base[i]);
		  }*/
		/*
		   unsigned long CS3_PHY_ADDR;           
		   writel(0x00000C40 ,io_base[3]);
		   CS3_PHY_ADDR=(unsigned long)ioremap(0x0,0x350);
		   unsigned char* IRQ_REG=(unsigned char*)(CS3_PHY_ADDR+0x340);
		 *IRQ_REG=1<<8;
		 unsigned char data=*IRQ_REG;
		 iounmap((void *)CS3_PHY_ADDR);
		 printk("data_0x340=%x\r\n",data);
		 */

		gpmc_regs_map();
		cs_enable(3, GPMC_CS3_PHY_BASE, 0x0F, 0, 0);		//cs3, base = 16M, size = 16M, width = 8bit, data-addr no multiplex
		cs_enable(4, GPMC_CS4_PHY_BASE, 0x0F, 0, 0);		//cs3, base = 96M, size = 16M, width = 8bit, data-addr no multiplex
		gpmc_regs_init();
		gpmc_com_test();
		return 0;
}

/*gpmc device cleanup*/
static void __exit gpmc_exit(void)
{
		cdev_del(&gpmc_dev->cdev);
		kfree(gpmc_dev);
		unregister_chrdev_region(gpmc_dev_num.devNum,1);
		gpmc_regs_deinit();
		printk(KERN_ERR "gpmc unregister_chrdev_region success,major=%d\n",gpmc_dev_num.major);
}

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("This Module is used to Control GPMC");
MODULE_AUTHOR("ZhangFengLi@520");
module_init(gpmc_init);
module_exit(gpmc_exit);

























