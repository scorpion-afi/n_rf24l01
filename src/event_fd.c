
#include <linux/fs.h>        // for file, node structure declaration
#include <linux/cdev.h>      // character device API
#include <linux/sched.h>
#include <linux/eventfd.h>

#define _DEBUG_
#include "n_rf24l01.h"

/* the character device file should be created to use this module,
 * look for /proc/devices ${MODULE_NAME} */

static dev_t n_rf24l01_dev;
static struct cdev *n_rf24l01_cdev;
static struct eventfd_ctx *eventfd_obj;

/* am ioctl request to pass event_fd object's fd to the n_rf24l01 device driver
 *
 * 248 - is a major device number allocated by the kernel for the n_rf24l01 device driver
 * 1   - is a number of the ioctl
 * int - is a type of the parameter */
#define PASS_EVENT_FD _IOW( 248, 1, int )


/* called whenever somebody makes systemcall 'open' on device file */
static int n_rf24l01_open( struct inode *inod, struct file *fp )
{
  DEBUG_OUT( "open() syscall on device file.\n" );

  return 0;
}

/* called when somebody makes systemcall 'close' on device file */
static int n_rf24l01_release( struct inode *inod, struct file *fp )
{
  DEBUG_OUT( "close() syscall on device file.\n" );

  return 0;
}

/* called whenever somebody makes ioctl syscall on device file */
static long n_rf24l01_unlocked_ioctl( struct file *fp, unsigned int cmd, unsigned long arg )
{
	DEBUG_OUT( "ioctl() syscall with cmd: %u, arg: %lu.\n", cmd, arg );

	if( cmd == PASS_EVENT_FD )
	{
	  eventfd_obj = eventfd_ctx_fdget( arg );
	  eventfd_signal( eventfd_obj, 1 );
	}


	return 0;
}

static struct file_operations n_rf24l01_fops =
{
  .open = n_rf24l01_open,
  .release = n_rf24l01_release,
  .unlocked_ioctl = n_rf24l01_unlocked_ioctl
};


// called when kernel loaded module
//=======================================================================
static int n_rf24l01_init( void )
{
  int ret;

  DEBUG_OUT( "module %s has been loaded.\n", MODULE_NAME );

  // ask kernel to allocate us one device number (x.0)
  // device number ties device file with device functions set
  // after success call, /proc/devices/${MODULE_NAME} node will appear
  ret = alloc_chrdev_region( &n_rf24l01_dev, 0, 1, MODULE_NAME );
  if( ret )
  {
	DEBUG_OUT( "error while alloc_chrdev_region call.\n" );
	return -EFAULT;
  }

  DEBUG_OUT( "device number: %u.%u.\n", MAJOR(n_rf24l01_dev), MINOR(n_rf24l01_dev) );

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
	DEBUG_OUT( "error while cdev_add call.\n" );

	cdev_del( n_rf24l01_cdev );
	unregister_chrdev_region( n_rf24l01_dev, 1 );

	return -EFAULT;
  }

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

  DEBUG_OUT( "module %s has been unloaded.\n", MODULE_NAME );
}

module_init( n_rf24l01_init );
module_exit( n_rf24l01_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "sergs <ivan0ivanov0@mail.ru>" );
