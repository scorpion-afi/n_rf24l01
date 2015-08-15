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

	byte = str;

	ret = init_n_rf24l01();
	if( ret < 0 ) return 1;

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
