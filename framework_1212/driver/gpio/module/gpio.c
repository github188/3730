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

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#warning *** not a warning *** Note: LINUX_VERSION_CODE >= 2.6.26

#define USE_CACHE_VOID_ARG
#undef USE_CLASS_DEVICE
#undef USE_CLASS_SIMPLE

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)

#warning *** not a warning *** Note: 2.6.26 > LINUX_VERSION_CODE >= 2.6.18

#define USE_CACHE_VOID_ARG
#define USE_CLASS_DEVICE
#undef USE_CLASS_SIMPLE

#else  /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

#warning *** not a warning *** Note: LINUX_VERSION_CODE < 2.6.18

#define USE_CLASS_SIMPLE
#undef USE_CLASS_DEVICE
#undef USE_CACHE_VOID_ARG

#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */

/*
 * Poor man's config params
 */

/*
 * NOCACHE means physical memory block is ioremap()'ed as noncached
 */
#define NOCACHE

/*
 * CHECK_FOR_ALLOCATED_BUFFER means ensure that the passed addr/size block
 * is actually an allocated, CMEM-defined buffer.
 */
//#define CHECK_FOR_ALLOCATED_BUFFER

#define HEAP_ALIGN PAGE_SIZE


#ifdef __DEBUG
#define __D(fmt, args...) printk(KERN_DEBUG "GPIO Debug: " fmt, ## args)
#else
#define __D(fmt, args...)
#endif

#define __E(fmt, args...) printk(KERN_ERR "GPIO Error: " fmt, ## args)

#define MAXTYPE(T) ((T) (((T)1 << ((sizeof(T) * 8) - 1) ^ ((T) -1))))

/*
 * Change here for supporting more than 2 blocks.  Also change all
 * NBLOCKS-based arrays to have NBLOCKS-worth of initialization values.
 */
#define NBLOCKS 2

static int gpio_major;
static struct proc_dir_entry *gpio_proc_entry;
static atomic_t reference_count = ATOMIC_INIT(0);

static unsigned int USE_UDEV = 1;

#if (USE_UDEV==1)
#ifdef USE_CLASS_SIMPLE
static struct class_simple *gpio_class;
#else
static struct class *gpio_class;
#endif
#endif // USE_UDEV

static DECLARE_MUTEX(cmem_mutex);

/* Forward declaration of system calls */
static int ioctl(struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long args);
static int mmap(struct file *filp, struct vm_area_struct *vma);
static int open(struct inode *inode, struct file *filp);
static int release(struct inode *inode, struct file *filp);

static struct file_operations gpio_fxns = {
ioctl:   ioctl,
		 mmap:    mmap,
		 open:    open,
		 release: release
};
static int cmem_proc_open(struct inode *inode, struct file *file);

static struct file_operations gpio_proc_ops = {
	.owner = THIS_MODULE,
	.open = cmem_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static int cmem_proc_open(struct inode *inode, struct file *file)
{
	return 0;
}


static int ioctl(struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long args)
{
	return 0;
}

static int mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long physp;

	__D("mmap: vma->vm_start     = %#lx\n", vma->vm_start);
	__D("mmap: vma->vm_pgoff     = %#lx\n", vma->vm_pgoff);
	__D("mmap: vma->vm_end       = %#lx\n", vma->vm_end);
	__D("mmap: size              = %#lx\n", vma->vm_end - vma->vm_start);

	physp = vma->vm_pgoff << PAGE_SHIFT;
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
	vma->vm_flags |= VM_RESERVED | VM_IO;
	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
					vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		__E("set_noncached: failed remap_pfn_range\n");
		return -EAGAIN;
	}

	return 0;	
}

static int open(struct inode *inode, struct file *filp)
{
	__D("open: called.\n");

	atomic_inc(&reference_count);

	return 0;
}

static int release(struct inode *inode, struct file *filp)
{
	int last_close = 0;
	__D("close: called.\n");

	/* Force free all buffers owned by the 'current' process */

	if (atomic_dec_and_test(&reference_count)) {
		__D("close: all references closed, force freeing all busy buffers.\n");

		last_close = 1;
	}

	__D("close: returning\n");

	return 0;
}

//no used
void printk_reg_isif(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c71000); // EDMACC.QUEPRI

	for (i = 0; i < 114;i++)
	{
		printk("isif: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//in rsz_run set
void printk_reg_ipipeif(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c71200); // EDMACC.QUEPRI

	for (i = 0; i < 23;i++)
	{
		printk("ipipeif: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//in drv_init
void printk_reg_ipipe(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c70800); // EDMACC.QUEPRI

	for (i = 0; i < 256;i++)
	{
		printk("ipipe: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//in drv_init and rsz_run
void printk_reg_rsz(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c70400); // EDMACC.QUEPRI

	for (i = 0; i < 94;i++)
	{
		printk("rsz: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//no used
void printk_reg_h3a(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c71400); // EDMACC.QUEPRI

	for (i = 0; i < 31;i++)
	{
		printk("h3a: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//in drv_init
void printk_reg_isp(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c70000); // EDMACC.QUEPRI

	for (i = 0; i < 12 ;i++)
	{
		printk("isp: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

//no used
void printk_reg_vpss(void)
{
	volatile unsigned int *pReg = NULL;
	unsigned int i = 0;
	pReg = (unsigned int *)IO_ADDRESS(0x1c70200); // EDMACC.QUEPRI

	for (i = 0; i < 1 ;i++)
	{
		printk("vpss: %d value 0x%x.\r\n", i , *(pReg+i));
	}
}

void printk_reg(void)
{
	printk_reg_isif();
	printk_reg_ipipeif();
	printk_reg_ipipe();
	printk_reg_rsz();
	printk_reg_h3a();
	printk_reg_isp();
	printk_reg_vpss();
}

int storage_isr(unsigned int intId, unsigned int isr, void *prm)
{
	return 0;
}

int keydetect_isr(unsigned int intId, unsigned int isr, void *prm)
{
	return 0;
}
#define GPIO_KEY_DETECT	35
#define STORAGE_KEY_DETECT	24
#define PINMUX3_PHYADDR	0x01C4000C
#define PINMUX4_PHYADDR	0x01C40010
#define GPIO_BANK_ADDR_BASE	0x1C67000

static unsigned int irq_gpio_init(void)
{
	unsigned int volatile *ptrpinmux3 = NULL;
	unsigned int volatile *ptrpinmux4 = NULL;
	unsigned int volatile *ptrgpiobase = (unsigned int *)IO_ADDRESS(GPIO_BANK_ADDR_BASE);
	//1. config PINMUX , GPIO_KEY, STORAGE_KEY as input	
	ptrpinmux3 = (unsigned int *)IO_ADDRESS(PINMUX3_PHYADDR);	
	ptrpinmux4 = (unsigned int *)IO_ADDRESS(PINMUX4_PHYADDR);	

	*ptrpinmux3 &= (~(1<<28));	//GPIO24 config as GPIO
	*ptrpinmux4 &= (~( (1<<16) | (1<<17)));	
	//2. input trigger mode ( edge interrupt or level interrupt? )

	*(ptrgpiobase + 0x10) |= (1<<24);	//GPIO 24 as input
	*(ptrgpiobase + 0x38) |= (1<<(38-32));

	//3. request_irq 
}
int __init gpio_init(void)
{
	int err = 0;

	/* cut-and-paste above as part of adding support for more than 2 blocks */

	gpio_major = register_chrdev(0, "gpio", &gpio_fxns);

	if (gpio_major < 0) {
		__E("Failed to allocate major number.\n");
		return -ENODEV;
	}

	__D("Allocated major number: %d\n", gpio_major);

#if (USE_UDEV==1)
#ifdef USE_CLASS_SIMPLE
	gpio_class = class_simple_create(THIS_MODULE, "gpio");
#else
	gpio_class = class_create(THIS_MODULE, "gpio");
#endif
	if (IS_ERR(gpio_class)) {
		__E("Error creating cmem device class.\n");
		err = -EIO;
		return err;
	}

#ifdef USE_CLASS_SIMPLE
	class_simple_device_add(gpio_class, MKDEV(gpio_major, 0), NULL, "gpio");
#else
#ifdef USE_CLASS_DEVICE
	class_device_create(gpio_class, NULL, MKDEV(gpio_major, 0), NULL, "gpio");
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	device_create(gpio_class, NULL, MKDEV(gpio_major, 0), NULL, "gpio");
#else
	device_create(gpio_class, NULL, MKDEV(gpio_major, 0), "gpio");
#endif // LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#endif // USE_CLASS_DEVICE
#endif // USE_CLASS_SIMPLE
#endif // USE_UDEV

	/* Create the /proc entry */

	printk(KERN_INFO "gpio initialized\n");

//	printk_reg();
	return 0;

}

void __exit gpio_exit(void)
{
	__D("In gpio_exit()\n");

	/* Remove the /proc entry */
	remove_proc_entry("gpio", NULL);

#if (USE_UDEV==1)

#ifdef USE_CLASS_SIMPLE
	class_simple_device_remove(MKDEV(gpio_major, 0));
	class_simple_destroy(gpio_class);
#else
#ifdef USE_CLASS_DEVICE
	class_device_destroy(gpio_class, MKDEV(gpio_major, 0));
#else
	device_destroy(gpio_class, MKDEV(gpio_major, 0));
#endif // USE_CLASS_DEVICE
	class_destroy(gpio_class);
#endif // USE_CLASS_SIMPLE
#endif // USE_UDEV

	__D("Unregistering character device mem\n");
	unregister_chrdev(gpio_major, "gpio");
	printk(KERN_INFO "gpio unregistered\n");
}

MODULE_LICENSE("GPL");
module_init(gpio_init);
module_exit(gpio_exit);
