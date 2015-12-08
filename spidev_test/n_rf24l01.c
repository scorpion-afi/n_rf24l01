// This file contains functions to work with n_rf24l01 transceiver via /dev/spidevA.B device file (for odroid U3)

// n_rf24l01 commands have 8 bits.
// Every new command must be started by a high to low transition on CSN.
// In parallel to the SPI command word applied on the MOSI pin, the STATUS register is shifted serially out on
// the MISO pin.

// The serial shifting SPI commands is in the following format:
//  <Command word: MSBit to LSBit (one byte)>
//  <Data bytes: LSByte to MSByte, MSBit in each byte first>

// spi on n_rf24l01 works with next settings:
//  CPOL = 0, CPHA = 0
//  msbit first
//  8 bits per word
//  spi speed up to 8MHz (but now we use only 50kHz)
//  CSN - is active low

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <linux/spi/spidev.h>

#include "n_rf24l01.h"

// spidev device file fd
static int spi_fd;

// ce pin control file fd
static int ce_pin_fd;


// this function set/clear CE pin
// value - value to set onto pin
//======================================================================================================
int set_up_ce_pin( __u8 value )
{
	int ret;
	char str[2];	// snprintf appends string by '\0' symbol

	// write or 1 or 0
	snprintf( str, sizeof str, "%u", !!value );

	// pin_fd file requires only '1' or '0', no 1 or 0
	ret = write( ce_pin_fd, str, 1 );
	if( ret < 0 || ret != 1 )
	{
		printf( "error while set_up_pin call.\n" );
		return -1;
	}

	return 0;
}

// send a command to n_rf24l01
// cmd - command to send
// status_reg - pointer n_rf24l01 status register will be written to
// ptr - pointer to data to be written to or to be read from n_rf24l01, depends on @type
//       data must be right aligned, only maximum COMMAND_DATA_SIZE bytes will be used/meaningful,
//		 real amount of data to read from/write to is depends on @num parameter
// type - type of operation: 1 - write, 0 - read
// num - amount of bytes to read or write (except command byte) (max amount is COMMAND_DATA_SIZE)
// return -1 if failed, 0 otherwise
// if you want to read only n_rf24l01 status register you may pass @cmd as NOP and
// after function execution in memory @status_reg points to will be status register
// Note: if you pass @num as 0, you just ask to execute you command and return status register.
//       if you want just execute command you can do this: send_command( you_cmd, NULL, NULL, 0, 0 )
//======================================================================================================
int send_command( __u8 cmd, __u8* status_reg, __u64* ptr, __u8 num, __u8 type )
{
	struct spi_ioc_transfer transfers[COMMAND_DATA_SIZE + 1];
	__u8 nop = 0xff;
	int ret, i;

	// TODO: currently we can't transmit more then 8 bytes to n_rf24l01
	if( num > 8 ) return -1;

	// we can pass NULL as @ptr if we want to read only n_rf24l01 status register
	if( num && !ptr ) return -1;

	// clear, before use it
	memset( transfers, 0, sizeof transfers );

	// transaction to send command
	transfers[0].tx_buf = &cmd;
	transfers[0].rx_buf = status_reg;
	transfers[0].len = 1;

	// set of transactions to write/read command's data
	for( i = 0; i < num && i < COMMAND_DATA_SIZE; i++ )
	{
		if( type ) // if client want to write some data
		{
			transfers[i + 1].tx_buf = ((__u8*)ptr) + i;
			transfers[i + 1].rx_buf = &nop;
		}
		else	// if client want to read some data
		{
			transfers[i + 1].tx_buf = &nop;
			transfers[i + 1].rx_buf = ((__u8*)ptr) + i;
		}

		transfers[i + 1].len = 1;
	}

	// ask to do actually spi fullduplex transactions
	// returns number of transmitted bytes in each spi transactions, or -1 if error was occurred
	switch( num )
	{
		case 0:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(1), transfers );
		break;
		case 1:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(2), transfers );
		break;
		case 2:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(3), transfers );
		break;
		case 3:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(4), transfers );
		break;
		case 4:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(5), transfers );
		break;
		case 5:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(6), transfers );
		break;
		case 6:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(7), transfers );
		break;
		case 7:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(8), transfers );
		break;
		case 8:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(9), transfers );
		break;
		case 9:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(10), transfers );
		break;
		case 10:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(11), transfers );
		break;
		case 11:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(12), transfers );
		break;
		case 12:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(13), transfers );
		break;
		case 13:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(14), transfers );
		break;
		case 14:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(15), transfers );
		break;
		case 15:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(16), transfers );
		break;
		case 16:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(17), transfers );
		break;
		case 17:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(18), transfers );
		break;
		case 18:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(19), transfers );
		break;
		case 19:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(20), transfers );
		break;
		case 20:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(21), transfers );
		break;
		case 21:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(22), transfers );
		break;
		case 22:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(23), transfers );
		break;
		case 23:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(24), transfers );
		break;
		case 24:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(25), transfers );
		break;
		case 25:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(26), transfers );
		break;
		case 26:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(27), transfers );
		break;
		case 27:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(28), transfers );
		break;
		case 28:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(29), transfers );
		break;
		case 29:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(30), transfers );
		break;
		case 30:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(31), transfers );
		break;
		case 31:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(32), transfers );
		break;
		case 32:
			ret = ioctl( spi_fd, SPI_IOC_MESSAGE(33), transfers );
		break;
		default:
			ret = -1;
		break;
	}

	//  Note: we can't pass variable into SPI_IOC_MESSAGE(), it is just compile time macros

	if( ret < 0 )
	{
		perror( "error while SPI_IOC_MESSAGE ioctl" );
		return -1;
	}

	return 0;
}

// transmit one byte, n_rf24l01 must be in transmit mode
//======================================================================================================
int transmit_byte( __u8 byte )
{
	__u64 data = 0;
	int ret;
	__u8 reg = 0;

	data = byte;
	ret = send_command( W_TX_PAYLOAD, NULL, &data, 1, 1 );
	if( ret < 0 ) return -1;

	read_register( STATUS_RG, &reg );
	printf( "before CE set high: STATUS_RG: 0x%02hhx.\n", reg );

	read_register( CONFIG_RG, &reg );
	printf( "before CE set high: CONFIG_RG: 0x%02hhx.\n", reg );

	// CE up... sleep 10 us... CE down - to actual data transmit (in space)
	set_up_ce_pin( 1 );
	usleep( 1 );
	set_up_ce_pin( 0 );

	// TODO: we must read status register after n_rf24l01 interrupt is occurred to determine
	//       that data were successfully transmitted, but currently we just sleep 1 second.
	sleep( 1 );

	ret = clear_pending_interrupts();
	if( ret < 0 ) return -1;

	return 0;
}

// turn n_rf24l01 into transmit mode
//======================================================================================================
int prepare_to_transmit( void )
{
	int ret;

	ret = set_up_ce_pin( 0 );
	if( ret < 0 ) return -1;

	ret = clear_bits( CONFIG_RG, PRIM_RX );
	if( ret < 0 ) return -1;
	usleep( 140 );

	return 0;
}

// turn n_rf24l01 into receive mode
//======================================================================================================
int prepare_to_receive( void )
{
	int ret;

	ret = set_bits( CONFIG_RG, PRIM_RX );
	if( ret < 0 ) return -1;

	ret = set_up_ce_pin( 1 );
	if( ret < 0 ) return -1;
	usleep( 140 );

	return 0;
}

// write register with @reg_addr from @ptr
// reg_addr - address of register to be written to
// byte - variable register's content will be read from
// only for 1-byte registers
//======================================================================================================
int write_register( __u8 reg_addr, __u8 byte )
{
	int ret;
	__u64 data = 0;

	// clear first command-specified bits (for R_REGISTER and W_REGISTER)
	reg_addr &= REG_ADDR_BITS;

	data = byte;

	ret = send_command( W_REGISTER | reg_addr, NULL, &data, 1, 1 );
	if( ret < 0 ) return -1;

	return 0;
}

// read register with @reg_addr in @ptr
// reg_addr - address of register to be read from
// byte - pointer to memory register's content will be written into
// only for 1-byte registers
//======================================================================================================
int read_register( __u8 reg_addr, __u8* byte )
{
	int ret;
	__u64 data = 0;

	if( !byte ) return -1;

	// clear first command-specified bits (for R_REGISTER and W_REGISTER)
	reg_addr &= REG_ADDR_BITS;

	ret = send_command( R_REGISTER | reg_addr, NULL, &data, 1, 0 );
	if( ret < 0 ) return -1;

	*byte = (__u8)data;

	return 0;
}

// clear specified bits in register
// reg_addr - address of register to be modified
// bits - bits to be cleared
// return -1 if failed, 0 otherwise
// only for 1-byte registers
//======================================================================================================
int clear_bits( __u8 reg_addr, __u8 bits )
{
	__u64 data = 0;
	int ret;

	// clear first command-specified bits (for R_REGISTER and W_REGISTER)
	reg_addr &= REG_ADDR_BITS;

	// firstly we must read register...
	ret = send_command( R_REGISTER | reg_addr, NULL, &data, 1, 0 );
	if( ret < 0 ) return -1;

	// apply new bits...
	data &= ~bits;

	// then set new bits
	ret = send_command( W_REGISTER | reg_addr, NULL, &data, 1, 1 );
	if( ret < 0 ) return -1;

	return 0;
}

// set specified bits in register
// reg_addr - address of register to be modified
// bits - bits to be set
// return -1 if failed, 0 otherwise
// only for 1-byte registers
//======================================================================================================
int set_bits( __u8 reg_addr, __u8 bits )
{
	__u64 data = 0;
	int ret;

	// clear first command-specified bits (for R_REGISTER and W_REGISTER)
	reg_addr &= REG_ADDR_BITS;

	// firstly we must read register...
	ret = send_command( R_REGISTER | reg_addr, NULL, &data, 1, 0 );
	if( ret < 0 ) return -1;

	// apply new bits...
	data |= bits;

	// then set new bits
	ret = send_command( W_REGISTER | reg_addr, NULL, &data, 1, 1 );
	if( ret < 0 ) return -1;

	return 0;
}

// for clear interrupts pending bits
//======================================================================================================
int clear_pending_interrupts( void )
{
	int ret;
	__u8 status_reg = 0;

	// NOP command: for read status register
	ret = send_command( NOP, &status_reg, NULL, 0, 0 );
	if( ret < 0 ) return -1;

	printf( "clear_pending_interrupts: STATUS_RG: 0x%02hhx.\n", status_reg );

	ret = write_register( STATUS_RG, status_reg );
	if( ret < 0 ) return -1;

	ret = send_command( NOP, &status_reg, NULL, 0, 0);
	if( ret < 0 ) return -1;

	printf( "clear_pending_interrupts: STATUS_RG: 0x%02hhx.\n\n", status_reg );

	return 0;
}

// wait for pakage transmitting (TX_DS bit in STATUS register)
//======================================================================================================
void wait_pkg_transmitted( void )
{
  __u8 reg;

  while( 1 )
  {
    usleep( 1 );

    read_register( STATUS_RG, &reg );

    // clear TX FIFO pending interrupt flag
    if( reg & TX_DS )
    {
      set_bits( STATUS_RG, TX_DS );
      break;
    }
  }
}

// setup odroid's pins needed to control n_rf24l01
// to explain watch to http://forum.odroid.com/viewtopic.php?f=80&t=5702
// return -1 if failed
//======================================================================================================
static int init_pins( void )
{
	struct stat st;
	int fd;
	int ret;

	// check does file CE_PIN_FILE exist, if not, we must create it...
	ret = stat( CE_PIN_FILE, &st );
	if( ret == -1 )
	{
		// create new directory /sys/class/gpio/gpio199,
		// now I don't know how to explain this otherwise
		fd = open( "/sys/class/gpio/export", O_WRONLY );
		if( fd < 0 ) return -1;

		ret = write( fd, CE_EXPORT_NAME, sizeof(CE_EXPORT_NAME) - 1 );
		if( ret < 0 || ret != sizeof(CE_EXPORT_NAME) - 1 )
		{
			printf( "error while write to /sys/class/gpio/export.\n" );
			close( fd );
			return -1;
		}
		close( fd );
	}

	// set ce pin direction
	fd = open( CE_PIN_FILE"direction", O_RDWR );
	if( fd < 0 ) return -1;

	// -1 due to \0 symbol at end of string
	ret = write( fd, CE_PIN_DIRECTION, sizeof(CE_PIN_DIRECTION) - 1 );
	if( ret < 0 || ret != sizeof(CE_PIN_DIRECTION) - 1 )
	{
		printf( "error while set ce pin direction.\n" );
		close( fd );
		return -1;
	}

	close( fd );

	// open file that control ce pin state
	ce_pin_fd = open( CE_PIN_FILE"value", O_RDWR );
	if( ce_pin_fd < 0 ) return -1;

	return 0;
}

// set up spi master to correct settings
// spi on n_rf24l01 works with next settings:
//  CPOL = 0, CPHA = 0
//  msbit first
//  8 bits per word
//  spi speed up to 8MHz (but now we use only 50kHz)
//======================================================================================================
static int setup_master_spi( void )
{
	int ret;
	__u8 mode;
	__u8 bits_order;
	__u8 bits_per_word;
	__u32 speed_hz;

	mode = SPI_MODE_0; // CPOL = 0, CPHA = 0
	ret = ioctl( spi_fd, SPI_IOC_WR_MODE, &mode );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_WR_MODE ioctl call" );
		return -1;
	}

	ret = ioctl( spi_fd, SPI_IOC_RD_MODE, &mode );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_RD_MODE ioctl call" );
		return -1;
	}

	bits_order = 0; // msbit first
	ret = ioctl( spi_fd, SPI_IOC_WR_LSB_FIRST, &bits_order );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_WR_LSB_FIRST ioctl call" );
		return -1;
	}

	ret = ioctl( spi_fd, SPI_IOC_RD_LSB_FIRST, &bits_order );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_RD_LSB_FIRST ioctl call" );
		return -1;
	}

	bits_per_word = 0; // 8 bit per word
	ret = ioctl( spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits_per_word );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_WR_BITS_PER_WORD ioctl call" );
		return -1;
	}

	ret = ioctl( spi_fd, SPI_IOC_RD_BITS_PER_WORD, &bits_per_word );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_RD_BITS_PER_WORD ioctl call" );
		return -1;
	}

	speed_hz = 500000; // 500 kHz
	ret = ioctl( spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_WR_MAX_SPEED_HZ ioctl call" );
		return -1;
	}

	ret = ioctl( spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed_hz );
	if( ret < 0 )
	{
		perror( "error while SPI_IOC_RD_MAX_SPEED_HZ ioctl call" );
		return -1;
	}

	printf( "spi mode: SPI_MODE_%hhu.\n", mode );
	printf( "spi bits order: %s.\n", bits_order ? "lsbit first" : "msbit first" );
	if (!bits_per_word)
		printf( "spi bits per word: 8.\n" );
	else
		printf( "spi bits per word: %hhu.\n", bits_per_word );
	printf( "spi speed: %uHz.\n\n", speed_hz );

	return 0;
}

//  make all initialization step to prepare n_rf24l01 to work
//======================================================================================================
int init_n_rf24l01( void )
{
	int ret;
	//__u8 reg;

	spi_fd = open( SPI_DEVICE_FILE, O_RDWR );
	if( spi_fd < 0 )
	{
		char temp[128];

		snprintf( temp, sizeof temp, "error while open spidev device file: %s", SPI_DEVICE_FILE );
		perror( temp );

		return -1;
	}

	ret = setup_master_spi();
	if( ret < 0 )
	{
		close( spi_fd );
		return -1;
	}

	printf( "spi master was successfully prepared to use.\n" );

	ret = init_pins();
	if( ret < 0 )
	{
		close( spi_fd );
		return -1;
	}

	printf( "pins were successfully prepared to use.\n" );

	set_up_ce_pin( 1 );

    write_register( EN_AA_RG, 0x00 );

	// turn on n_rf24l01 transceiver
	ret = set_bits( CONFIG_RG, PWR_UP);
	if( ret < 0 ) return -1;
	usleep( 1500 );

	// set data field size to 32 bytes (we will transmit 32 bytes for time)
	write_register( RX_PW_P0_RG, 0x01 );

	// set the lowermost transmit power
	clear_bits( RF_SETUP_RG, 0x06 );

	printf( "n_rf24l01 was successfully prepared to use.\n\n" );

	return 0;
}
