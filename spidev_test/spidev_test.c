/*
 ============================================================================
 Name        : test_app.c
 Author      : Ila
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "n_rf24l01.h"

//
//======================================================================================================
int main ( void )
{
	int ret;
	char str[] = "hello world!!!";
	char* byte;
	__u8 reg;
	__u64 data;

	byte = str;

	ret = init_n_rf24l01();
	if( ret < 0 ) return 1;

	write_register( EN_AA_RG, 0x00 );

	prepare_to_receive();

	while(1)
	{
		read_register( STATUS_RG, &reg );
		if( reg != 0x0e )
		{
			send_command( R_RX_PAYLOAD, NULL, &data, 1, 0);
			clear_pending_interrupts();
		}
	}

	ret = prepare_to_transmit();
	if( ret < 0 ) return 1;

	read_register( CONFIG_RG, &reg );
	printf( "before while(1): CONFIG_RG: 0x%02hhx.\n", reg );

	read_register( STATUS_RG, &reg );
	printf( "before while(1): STATUS_RG: 0x%02hhx.\n", reg );

	set_up_ce_pin( 1 );
	usleep( 10 );
	//set_up_ce_pin( 0 );

	while( 1 )
	{
		send_command( W_TX_PAYLOAD, NULL, (__u64*)byte++, 1, 1 );

		printf( "byte %c has been transmitted to space.\n", *(byte - 1) );

		// round-robin buffer to transmit
		if( *byte == '\0' )
			byte = str;

		sleep(1);
	}

	// we transmit one byte one time in second
	while( 1 )
	{
		ret = transmit_byte( *byte++ );
		if( ret < 0) return 0;

		printf( "byte %c has been transmitted to space.\n", *(byte - 1) );

		// round-robin buffer to transmit
		if( *byte == '\0' )
			byte = str;
	}

	return 0;
}
