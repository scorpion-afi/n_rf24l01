#include <linux/fs.h>        // for file, node structure declaration
#include <linux/cdev.h>      // character device API
#include <linux/spi/spi.h>
#include <asm-generic/uaccess.h> // for access user's address space
#include <linux/wait.h>		// to be able to sleep and awake process
#include <linux/sched.h>

#define _DEBUG_
#include "n_rf24l01.h"

// create device file itself, for device number watch to /proc/devices/&{MODULE_NAME}

static dev_t n_rf24l01_dev;
static struct cdev* n_rf24l01_cdev;
static n_rf24l01_device_t n_rf24l01_device;

// called when somebody makes systemcall 'open' on device file
//=======================================================================
int n_rf24l01_open( struct inode* inod, struct file* filp )
{
  DEBUG_OUT( "system call on device file.\n" );

  return 0;
}

// called when somebody makes systemcall 'close' on device file
//=======================================================================
int n_rf24l01_release( struct inode* inod, struct file* filp )
{
  DEBUG_OUT( "system call on device file.\n" );

  return 0;
}

// called when somebody makes systemcall 'read' on device file
//=======================================================================
ssize_t n_rf24l01_read( struct file* filp, char __user* buf, size_t count, loff_t* offsset )
{
  long ret;
  unsigned int amount_to_read;
  unsigned int amount_to_read_tail = 0;
  unsigned int right_part;
  unsigned int max_amount_to_read;
  int is_tail;

  DEBUG_OUT( "before sleep: w_idx: %u, r_idx: %u, overtake: %d, count: %u.\n", n_rf24l01_device.rr_buffer.w_idx, n_rf24l01_device.rr_buffer.r_idx,
		  	  n_rf24l01_device.rr_buffer.overtake, count );

  if( !count ) return 0;

  // No data to read !!!
  // sleep process, process will be awaken by another process that write new data
  while( n_rf24l01_device.rr_buffer.w_idx == n_rf24l01_device.rr_buffer.r_idx && !n_rf24l01_device.rr_buffer.overtake )
  {
	  // if process has been interrupted by signal
	  if( wait_event_interruptible( n_rf24l01_device.rr_buffer.write_read_queue, n_rf24l01_device.rr_buffer.write_read_event ) )
	  {
		  DEBUG_OUT( "process has been interrupted by signal.\n" );
		  return -ERESTARTSYS;
	  }

	  n_rf24l01_device.rr_buffer.write_read_event = 0;
  }

  DEBUG_OUT( "after awaken: w_idx: %u, r_idx: %u, overtake: %d.\n", n_rf24l01_device.rr_buffer.w_idx, n_rf24l01_device.rr_buffer.r_idx,
		  	  n_rf24l01_device.rr_buffer.overtake );

  // calculate possible amount of data to read, taking in account w_idx and r_idx positions
  if( n_rf24l01_device.rr_buffer.w_idx == n_rf24l01_device.rr_buffer.r_idx )
	  max_amount_to_read = sizeof(n_rf24l01_device.rr_buffer.write_read);
  else if( n_rf24l01_device.rr_buffer.w_idx > n_rf24l01_device.rr_buffer.r_idx )
 	  max_amount_to_read = n_rf24l01_device.rr_buffer.w_idx - n_rf24l01_device.rr_buffer.r_idx;
  else
 	  max_amount_to_read = sizeof(n_rf24l01_device.rr_buffer.write_read) - (n_rf24l01_device.rr_buffer.r_idx - n_rf24l01_device.rr_buffer.w_idx);

  // calculate real amount of data to read
  count = count > max_amount_to_read ? max_amount_to_read : count;

  // in this point we have @count variable that contains amount of bytes to read (with tail, if tail exists)

  // calculate right part of buffer (relatively to r_idx), read first portion of data from it
  // and read rest data from tail (left part of buffer, relatively to r_idx, up to w_idx position NOT-inclusive)

  right_part = sizeof(n_rf24l01_device.rr_buffer.write_read) - n_rf24l01_device.rr_buffer.r_idx;

  if( count > right_part )
  {
	  amount_to_read = right_part;
	  is_tail = 1;
  }
  else
  {
	  amount_to_read = count;
	  is_tail = 0;
  }

  DEBUG_OUT( "max_amount_to_read: %u, count: %u, right_part: %u, is_tail: %d, amount_to_read: %u.\n", max_amount_to_read, count, right_part,
		  	  is_tail, amount_to_read );

  ret = copy_to_user( buf, n_rf24l01_device.rr_buffer.write_read + n_rf24l01_device.rr_buffer.r_idx, amount_to_read );
  if( ret )
  {
	DEBUG_OUT( "error while copy_to_user call.\n" );
	return -EFAULT;
  }

  n_rf24l01_device.rr_buffer.r_idx += amount_to_read;

  // make round-robin buffer
  if( n_rf24l01_device.rr_buffer.r_idx >= sizeof(n_rf24l01_device.rr_buffer.write_read) )
	  n_rf24l01_device.rr_buffer.r_idx = 0;

  DEBUG_OUT( "data (%u bytes) was copied to user space buffer, r_idx: %u.\n", amount_to_read, n_rf24l01_device.rr_buffer.r_idx );

  if( is_tail )
  {
	  amount_to_read_tail = count - amount_to_read;

	  ret = copy_to_user( buf + amount_to_read, n_rf24l01_device.rr_buffer.write_read + n_rf24l01_device.rr_buffer.r_idx, amount_to_read_tail );
	  if( ret )
	  {
		DEBUG_OUT( "error while copy_to_user call.\n" );
		return -EFAULT;
	  }

	  n_rf24l01_device.rr_buffer.r_idx += amount_to_read_tail;

	  DEBUG_OUT( "tail_data (%u bytes) was copied to user space buffer, r_idx: %u.\n", amount_to_read_tail, n_rf24l01_device.rr_buffer.r_idx );
  }

  n_rf24l01_device.rr_buffer.overtake = 0;

  DEBUG_OUT( "New data can be written, awake write process!!! overtake = 0, return value: %u.\n", amount_to_read + amount_to_read_tail );

  // Data was read, new data can be written!!!
  // awake slept process, so it can write new data
  n_rf24l01_device.rr_buffer.write_read_event = 1;
  wake_up_interruptible( &n_rf24l01_device.rr_buffer.write_read_queue );

  // driver must return amount of bytes actually written into user space buffer
  return amount_to_read + amount_to_read_tail;
}

// called when somebody makes systemcall 'write' on device file
//=======================================================================
ssize_t n_rf24l01_write( struct file* filp, const char __user* buf, size_t count, loff_t* offsset )
{
  long ret;
  unsigned int amount_to_write;
  unsigned int amount_to_write_tail = 0;
  unsigned int right_part;
  unsigned int max_amount_to_write;
  int is_tail;

  DEBUG_OUT( "before sleep: w_idx: %u, r_idx: %u, overtake: %d, count: %u.\n", n_rf24l01_device.rr_buffer.w_idx, n_rf24l01_device.rr_buffer.r_idx,
		  	  n_rf24l01_device.rr_buffer.overtake, count );

  if( !count ) return 0;

  // Nowhere write new data !!!
  // if w_idx overtook r_idx position, we sleep write process until data will be read
  // when data will be read write process will be awaken
  while( n_rf24l01_device.rr_buffer.overtake )
  {
	  // if process has been interrupted by signal
	  if( wait_event_interruptible( n_rf24l01_device.rr_buffer.write_read_queue, n_rf24l01_device.rr_buffer.write_read_event ) )
	  {
		  DEBUG_OUT( "process has been interrupted by signal.\n" );
		  return -ERESTARTSYS;
	  }

	  n_rf24l01_device.rr_buffer.write_read_event = 0;
  }

  DEBUG_OUT( "after awaken: w_idx: %u, r_idx: %u, overtake: %d.\n", n_rf24l01_device.rr_buffer.w_idx, n_rf24l01_device.rr_buffer.r_idx,
		  	  n_rf24l01_device.rr_buffer.overtake );

  // calculate possible amount of data to write, taking in account w_idx and r_idx positions
  if( n_rf24l01_device.rr_buffer.w_idx == n_rf24l01_device.rr_buffer.r_idx )
	  max_amount_to_write = sizeof(n_rf24l01_device.rr_buffer.write_read);
  else if( n_rf24l01_device.rr_buffer.w_idx > n_rf24l01_device.rr_buffer.r_idx )
 	  max_amount_to_write = sizeof(n_rf24l01_device.rr_buffer.write_read) - (n_rf24l01_device.rr_buffer.w_idx - n_rf24l01_device.rr_buffer.r_idx);
  else
 	  max_amount_to_write = n_rf24l01_device.rr_buffer.r_idx - n_rf24l01_device.rr_buffer.w_idx;

  // calculate real amount of data to write
  count = count > max_amount_to_write ? max_amount_to_write : count;

  // in this point we have @count variable that contains amount of bytes to write (with tail, if tail exists)

  // calculate right part of buffer (relatively to w_idx), write first portion of data in it
  // and write rest data in tail (left part of buffer, relatively w_idx, up to r_idx position NOT-inclusive)

  right_part = sizeof(n_rf24l01_device.rr_buffer.write_read) - n_rf24l01_device.rr_buffer.w_idx;

  if( count > right_part )
  {
	  amount_to_write = right_part;
	  is_tail = 1;
  }
  else
  {
	  amount_to_write = count;
	  is_tail = 0;
  }

  DEBUG_OUT( "max_amount_to_write: %u, count: %u, right_part: %u, is_tail: %d, amount_to_write: %u.\n", max_amount_to_write, count, right_part,
		  	  is_tail, amount_to_write );

  ret = copy_from_user( n_rf24l01_device.rr_buffer.write_read + n_rf24l01_device.rr_buffer.w_idx, buf, amount_to_write );
  if( ret )
  {
	  DEBUG_OUT( "error while copy_from_user call.\n" );
	  return -EFAULT;
  }

  n_rf24l01_device.rr_buffer.w_idx += amount_to_write;

  // make round-robin buffer
  if( n_rf24l01_device.rr_buffer.w_idx >= sizeof(n_rf24l01_device.rr_buffer.write_read) )
	  n_rf24l01_device.rr_buffer.w_idx = 0;

  DEBUG_OUT( "data (%u bytes) was copied from user space buffer, w_idx: %u.\n", amount_to_write, n_rf24l01_device.rr_buffer.w_idx );

  if( is_tail )
  {
	  amount_to_write_tail = count - amount_to_write;

	  ret = copy_from_user( n_rf24l01_device.rr_buffer.write_read + n_rf24l01_device.rr_buffer.w_idx, buf + amount_to_write, amount_to_write_tail );
	  if( ret )
	  {
		DEBUG_OUT( "error while copy_from_user call.\n" );
		return -EFAULT;
	  }

	  n_rf24l01_device.rr_buffer.w_idx += amount_to_write_tail;

	  DEBUG_OUT( "tail_data (%u bytes) was copied from user space buffer, w_idx: %u.\n", amount_to_write_tail, n_rf24l01_device.rr_buffer.w_idx );
  }

  // w_idx overtook r_idx, so we must block next attempts to write new portion of data, until date will be read
  if( n_rf24l01_device.rr_buffer.w_idx == n_rf24l01_device.rr_buffer.r_idx )
	  n_rf24l01_device.rr_buffer.overtake = 1;

  DEBUG_OUT( "New data appeared, awake read process!!! overtake = %d, return value: %u.\n", n_rf24l01_device.rr_buffer.overtake,
		  	  amount_to_write + amount_to_write_tail );

  // New data appears !!!
  // awake slept process, so it can read new data
  n_rf24l01_device.rr_buffer.write_read_event = 1;
  wake_up_interruptible( &n_rf24l01_device.rr_buffer.write_read_queue );

  // driver must return amount of bytes actually read from user space buffer
  return amount_to_write + amount_to_write_tail;
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

  DEBUG_OUT( "module %s has been loaded.\n", MODULE_NAME );

  n_rf24l01_sysfs_init();

  // ask kernel allocate us one device number (x.0)
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

	// release one device number
	unregister_chrdev_region( n_rf24l01_dev, 1 );

	n_rf24l01_sysfs_deinit();

	return -EFAULT;
  }

  init_waitqueue_head( &n_rf24l01_device.rr_buffer.write_read_queue );

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

  DEBUG_OUT( "module %s has been unloaded.\n", MODULE_NAME );
}

module_init( n_rf24l01_init );
module_exit( n_rf24l01_exit );

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Ila <ivan0ivanov0@mail.ru>" );
