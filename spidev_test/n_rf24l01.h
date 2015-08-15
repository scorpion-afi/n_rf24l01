#ifndef TEST_APP_DEF_H
#define TEST_APP_DEF_H

#include <linux/types.h>

#define SPI_DEVICE_FILE "/dev/spidev1.0"

// defines for CE pin (n_rf24l01 specified pin)
#define CE_EXPORT_NAME 	 "199" // J4(IO-Port#1), 1-st pin
#define CE_PIN_FILE 	 "/sys/class/gpio/gpio"CE_EXPORT_NAME"/"
#define CE_PIN_DIRECTION "out"

// commands set
#define R_REGISTER 		0x00
#define W_REGISTER 		0x20
#define W_TX_PAYLOAD	0xa0
#define NOP 			0xff

// registers set
#define CONFIG_RG 		0x00
#define EN_AA_RG		0x01
#define RF_SETUP_RG		0x06
#define STATUS_RG		0x07

//--------- 5-bytes registers ---------
#define RX_ADDR_P0_RG 	0x0A
#define RX_ADDR_P1_RG 	0x0B
#define TX_ADDR_RG 		0x10
//-------------------------------------

#define RX_PW_P0_RG		0x11

// bits definition:

//  CONFIG register
#define PRIM_RX	0x01
#define PWR_UP 	0x02

// each register has 5 bits address in registers map
// used for R_REGISTER and W_REGISTER commands
#define REG_ADDR_BITS 0x1f

// max amount of command's data, in bytes
#define COMMAND_DATA_SIZE 32

int set_up_ce_pin( __u8 value );
int send_command( __u8 cmd, __u8* status_reg, __u64* ptr, __u8 num, __u8 type );
int transmit_byte( __u8 byte );
int prepare_to_transmit( void );
int prepare_to_receive( void );
int write_register( __u8 reg_addr, __u8 byte );
int read_register( __u8 reg_addr, __u8* byte );
int clear_bits( __u8 reg_addr, __u8 bits );
int set_bits( __u8 reg_addr, __u8 bits );
int clear_pending_interrupts( void );
int init_n_rf24l01( void );

#endif // TEST_APP_DEF_H
