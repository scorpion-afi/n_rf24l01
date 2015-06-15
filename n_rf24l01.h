
#ifndef N_RF24l01_H
#define N_RF24l01_H

#include <linux/module.h>    // needed by all modules
#include <linux/kernel.h>    // needed for KERN_INFO
#include <linux/errno.h>

#define MODULE_NAME "n_rf24l01"

#ifdef _DEBUG_
 #define DEBUG_OUT(str, args...) { printk(KERN_INFO "%s: %s: %s", MODULE_NAME, \
   __FUNCTION__, str, ##args); }
#else
 #define DEBUG_OUT(...)
#endif

int n_rf24l01_sysfs_init( void );
void n_rf24l01_sysfs_deinit( void );

#endif
