
#ifndef N_RF24l01_H
#define N_RF24l01_H

#include <linux/module.h>    // needed by all modules
#include <linux/kernel.h>    // needed for KERN_INFO
#include <linux/errno.h>

#define MODULE_NAME "n_rf24l01"

#ifdef _DEBUG_
 #define DEBUG_OUT(str, args...) { printk( KERN_INFO MODULE_NAME ": %s: [%d]: "str, __FUNCTION__, current->pid, ##args ); }
#else
 #define DEBUG_OUT(...)
#endif

// describes round-robin buffer
typedef struct rr_buffer_t
{
	uint32_t r_idx;
	uint32_t w_idx;
	int overtake; // is w_idx overtook r_idx position, so we cann't write anymore data

	char write_read[64];
	wait_queue_head_t write_read_queue;
	int write_read_event;
} rr_buffer_t;

typedef struct n_rf24l01_device_t
{
	rr_buffer_t rr_buffer;
} n_rf24l01_device_t;

#define n_rf24l01_get_private( filp ) (n_rf24l01_device_t*) filp->private_data

int n_rf24l01_sysfs_init( void );
void n_rf24l01_sysfs_deinit( void );

#endif
