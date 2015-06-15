#include <linux/fs.h>        // for file, node structure declaration
#include <linux/cdev.h>      // character device API
#include <linux/spi/spi.h>
#include <asm-generic/uaccess.h> // for access user's address space

#include "n_rf24l01.h"

// create device file itself, for device number watch to /proc/devices/&{MODULE_NAME}

static dev_t n_rf24l01_dev;
static struct cdev* n_rf24l01_cdev;
static char write_read[64];

// called when somebody makes systemcall 'open' on device file
//=======================================================================
int n_rf24l01_open( struct inode* inod, struct file* filp )
{
  DEBUG_OUT( "open system call on device file.\n" );

  return 0;
}

// called when somebody makes systemcall 'close' on device file
//=======================================================================
int n_rf24l01_release( struct inode* inod, struct file* filp )
{
  DEBUG_OUT( "close system call on device file.\n" );

  return 0;
}

// called when somebody makes systemcall 'read' on device file
//=======================================================================
ssize_t n_rf24l01_read( struct file* filp, char __user* buf, size_t count, loff_t* offsset )
{
  long ret;
  unsigned int amount_to_read;

  DEBUG_OUT( "read system call on device file.\n" );

  amount_to_read = count > sizeof(write_read) ? sizeof(write_read) : count;

  ret = copy_to_user( buf, write_read, amount_to_read );
  if( ret )
  {
	DEBUG_OUT( "error while copy_to_user call.\n" );
	return -EFAULT;
  }

  // driver must return amount of bytes actually written into
  // user space buffer
  return amount_to_read;
}

// called when somebody makes systemcall 'write' on device file
//=======================================================================
ssize_t n_rf24l01_write( struct file* filp, const char __user* buf, size_t count, loff_t* offsset )
{
  long ret;
  unsigned int amount_to_write;

  DEBUG_OUT( "write system call on device file.\n" );

  amount_to_write = count > sizeof(write_read) ? sizeof(write_read) : count;

  ret = copy_from_user( write_read, buf, amount_to_write );
  if( ret )
  {
	DEBUG_OUT( "error while copy_from_user call.\n" );
	return -EFAULT;
  }


  // driver must return amount of bytes actually read from
  // user space buffer
  return amount_to_write;
}

static struct file_operations n_rf24l01_fops =
{
  .open = n_rf24l01_open,
  .release = n_rf24l01_release,
  .read = n_rf24l01_read,
  .write = n_rf24l01_write
};

// called when kernel loaded module
//=======================================================================
static int n_rf24l01_init( void )
{
  int ret;

  printk( KERN_INFO "module %s has been loaded.\n", MODULE_NAME );

  n_rf24l01_sysfs_init();

  // ask kernel allocate us one device number (x.0)
  // device number ties device file with device functions set
  // after success call, /proc/devices/${ODULE_NAME} node will appear
  ret = alloc_chrdev_region( &n_rf24l01_dev, 0, 1, MODULE_NAME );
  if( ret )
  {
	printk( KERN_INFO "error while alloc_chrdev_region call.\n" );
	return -EFAULT;
  }

  // cdev structure represents character device in kernel
  n_rf24l01_cdev = cdev_alloc();
  n_rf24l01_cdev->owner = THIS_MODULE;
  n_rf24l01_cdev->ops = &n_rf24l01_fops;

  // register our character device in kernel,
  // after success call, file operations on device file, with device number 'n_rf24l01_dev',
  // will be redirected to functions in 'n_rf24l01_fops' structure
  cdev_add( n_rf24l01_cdev, n_rf24l01_dev, 1 );
  if( ret )
  {
	printk( KERN_INFO "error while cdev_add call.\n" );

	// release one device number
	unregister_chrdev_region( n_rf24l01_dev, 1 );

	n_rf24l01_sysfs_deinit();

	return -EFAULT;
  }

  /*spi_read(NULL, NULL, 1);
  spi_register_board_info*/

  return 0;
}

// called when kernel unloaded module
//=======================================================================
static void n_rf24l01_exit( void )
{
  // unregister character device
  cdev_del( n_rf24l01_cdev );

  // release one device number
  unregister_chrdev_region( n_rf24l01_dev, 1 );

  n_rf24l01_sysfs_deinit();

  printk( KERN_INFO "module %s has been unloaded.\n", MODULE_NAME );
}

module_init( n_rf24l01_init );
module_exit( n_rf24l01_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Ila <ivan0ivanov0@mail.ru>" );
