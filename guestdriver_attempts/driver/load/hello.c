/*  
 *  hello-1.c - Простейший модуль ядра.
 */
#include <linux/module.h>       /* Необходим для любого модуля ядра */
#include <linux/kernel.h>       /* Здесь находится определение KERN_ALERT */
#include <linux/types.h>
#include <linux/fs.h>         //file operations structure 
                              //( pointers to open, read, write, close )
#include <asm/uaccess.h>      //copy to/from user addrSpace
#include <linux/cdev.h>       //char driver
#include <linux/semaphore.h>    //

#define DEV_NAME "hello"

struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;


struct cdev *mcdev;
int major_number;
static dev_t dev_num;

int device_open( struct inode * , struct file* );
static ssize_t device_read( struct file* , 
                            char* , 
                            size_t , 
                            loff_t*);
static ssize_t device_write( struct file* , 
                            const char* , 
                            size_t , 
                            loff_t* );
static int device_close( struct inode *, struct file *);


int device_open( struct inode * inode, struct file* flip){
	if( down_interruptible(&virtual_device.sem) != 0){
		printk(KERN_ALERT "hello: could not lock device during open");
		return -1;
	}
	
	printk(KERN_INFO "hello: opened device");
	return 0;
}

static ssize_t device_read( struct file* flip, 
                            char* bufStoreData, 
                            size_t bufCount, 
                            loff_t* curOffset){
	int ret = 0;
	printk(KERN_INFO "hello: reading from device");
	if ( bufCount <= 100){
		ret = copy_to_user(bufStoreData, virtual_device.data, bufCount);
	}
	return ret;
}


static ssize_t device_write( struct file* flip, 
                            const char* bufSourceData, 
                            size_t bufCount, 
                            loff_t* curOffset){
	int ret = 0;
	printk(KERN_INFO "hello: writing to the device");
	if ( bufCount <= 100){
		ret = copy_from_user(virtual_device.data, bufSourceData , bufCount);
	}
	return ret;
}

static int device_close( struct inode *inode, struct file *filp){

	up(&virtual_device.sem);
	printk(KERN_INFO "hello: closed device");
	return 0;
}


struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = device_open,
	.release = device_close,
	.write = device_write,
	.read = device_read
};

static int driver_entry(void)
{
	int ret = 0;
	unsigned int count = 1, firstminor = 0;
 	ret = alloc_chrdev_region( &dev_num, firstminor, count, DEV_NAME);
	if (ret < 0){
		printk(KERN_ALERT "Hello world 1.\nres = %d\n", ret); 
		return ret;
	}
	major_number = MAJOR(dev_num);
	printk(KERN_INFO "Hello world 1.\nmajor = %u\nminor = %u\n", 
	MAJOR(dev_num), 
	MINOR(dev_num) );
	
	// cdev initialization
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE; // is it MACROS?
	ret = cdev_add(mcdev, dev_num, 1);
	if ( ret < 0){
		printk(KERN_ALERT "hello: umable to add cdev to kernel");
		return ret;
	}

	sema_init( &virtual_device.sem, 1); //semapthre initialization

	return 0;
}

static void driver_exit(void)
{
	cdev_del(mcdev);
	unregister_chrdev_region( dev_num , 1);
        printk(KERN_ALERT "Goodbye world 1.\n");
}


// Macroses which are giving information about INIT and EXIT driver functions
module_init(driver_entry);
module_exit(driver_exit);

