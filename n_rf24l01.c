#include <linux/module.h>       // needed by all modules
#include <linux/kernel.h>       // needed for KERN_INFO 
#include <linux/init.h>         // needed for the macros 


#define MODULE_NAME "n_rf24l01"

// our kernel variable, that will be exported, via sysfs, to user space,
// so user can read and write it and module will react to these changes
// immediately
static int mode = 11;

// this function is called when user read file with name mode at sysfs
//=======================================================================
static ssize_t sysfs_show( struct kobject* kobj, struct kobj_attribute* attr,
					   char* buf )
{
  if( strcmp( attr->attr.name, "mode" ) == 0 )
	return sprintf( buf, "%d\n", mode );
  return 0;
}

// this function is called when user write file with name mode at sysfs
//=======================================================================
static ssize_t sysfs_store( struct kobject* kobj, struct kobj_attribute* attr,
						const char* buf, size_t count )
{
  if( strcmp( attr->attr.name, "mode" ) == 0 )
  {
	sscanf( buf, "%d", &mode );
	return count;
  }

  return 0;
}

// represent file in sysfs
static struct kobj_attribute mode_attr =
		 __ATTR( mode, 0606, sysfs_show, sysfs_store );

// Create a group of attributes so that we can create and destory them all
// at once.
static struct attribute* attrs[] =
{
  &mode_attr.attr,
  NULL   // need to NULL terminate the list of attributes
};

// An unnamed attribute group will put all of the attributes directly in
// the kobject directory.  If we specify a name, a subdirectory will be
// created for the attributes with the directory being the name of the
// attribute group.
static struct attribute_group attr_group =
{
  .attrs = attrs
};

// represents directory in sysfs
static struct kobject* module_kobj;

// called when module is loaded
//=======================================================================
static int n_rf24l01_init( void )
{
  int ret;

  printk( KERN_INFO "module %s has been loaded.\n", MODULE_NAME );

  // Create a simple kobject with the name of ${MODULE_NAME},
  // located under /sys/kernel/

  // As this is a simple directory, no uevent will be sent to
  // userspace.  That is why this function should not be used for
  // any type of dynamic kobjects, where the name and number are
  // not known ahead of time.
  module_kobj = kobject_create_and_add( MODULE_NAME, kernel_kobj );
  if( !module_kobj )
	return -ENOMEM;

  printk( KERN_INFO "kobject has been created.\n" );

  // create the files associated with this kobject
  // after this we can see files, with names specified by attribs name,
  // under /sys/kernel/${MODULE_NAME}
  ret = sysfs_create_group( module_kobj, &attr_group );
  if( ret )
	kobject_put( module_kobj );

  printk( KERN_INFO "group of sysfs's attribuites has been created.\n" );

  return ret;
}

// called when module is unloaded
//=======================================================================
static void n_rf24l01_exit( void )
{
  kobject_put( module_kobj );
  printk( KERN_INFO "module %s has been unloaded.\n", MODULE_NAME );
}

module_init( n_rf24l01_init );
module_exit( n_rf24l01_exit );
MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "Ila <ivan0ivanov0@mail.ru>" );
