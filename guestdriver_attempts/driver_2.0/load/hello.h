#ifndef _HELLO_
#define _HELLO_
#include <linux/module.h>       /* Необходим для любого модуля ядра */
#include <linux/kernel.h>       /* Здесь находится определение KERN_ALERT */
#include <linux/types.h>
#include <linux/fs.h>         //file operations structure 
//( pointers to open, read, write, close )
#include <asm/uaccess.h>      //copy to/from user addrSpace
#include <linux/cdev.h>       //char driver
#include <linux/semaphore.h>    //

#define DEV_NAME "hello"


#define DEVICE_BUFFER_SIZE  ( 4*1024*1024 )
struct fake_device
{
    char *data;
    struct semaphore sem;
    loff_t len;
    struct cdev mcdev;
};


static int device_open( struct inode * , struct file* );
static ssize_t device_read( struct file* ,
                            char* ,
                            size_t ,
                            loff_t*);
static ssize_t device_write( struct file* ,
                             const char* ,
                             size_t ,
                             loff_t* );
static int device_close( struct inode *, struct file *);


#endif // _HELLO_
