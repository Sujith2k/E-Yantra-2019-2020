
/*
 * TITLE:
 * DATE:
 * AUTHOR:
 */ 


#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "VL53L0X.h"
#include "mcp23017.h"
#include "uart.h"
#include "i2c.h"


#define user_sw PE7
#define motorL1 PA0
#define motorL2 PA2
#define motorR1 PA1
#define motorR2 PA3
#define RED_LED PH3
#define GREEN_LED PH5

#define xshutL	PH2
#define xshutR	PC4
int count1=0;
void all_off()
{
	DDRH |= ((1<<xshutL));
	DDRC |= (1<<xshutR);
	PORTH &= ~(1<<xshutL);
	PORTC &= ~(1<<xshutR);

}


void sensorL()
{
	
	DDRH &= ~(1<<xshutL);
	//uart0_puts("0");
}

void sensorR()
{
	DDRC &= ~(1<<xshutR);
	//uart0_puts("1");

}


void all_low()
{
	PORTA &= ~((1<<motorL1) | (1<<motorL2) | (1<<motorR1) | (1<<motorR2));
}

void align()
{
	uint8_t reading;
	PORTA |= (1<<motorL1) | (1<<motorR1);
	_delay_ms(75);
	all_low();
	_delay_ms(300);
	reading = mcp23017_readpinsA(0);
	while(reading == 0b00000000)
	{
		PORTA |= (1<<motorL2) | (1<<motorR2);
		_delay_ms(2);
		all_low();
		_delay_ms(5);
		reading = mcp23017_readpinsA(0);
	}
	if(reading >= 0b00001000)
	{
		while (1)
		{
			PORTA |= (1<<motorR2);
			_delay_ms(2);
			all_low();
			_delay_ms(5);
			reading = mcp23017_readpinsA(0);
			if (reading == 0b11111111)
				break;
		}
	}
	else
	{
		while (1)
		{
			PORTA |= (1<<motorL2);
			_delay_ms(2);
			all_low();
			_delay_ms(5);
			reading = mcp23017_readpinsA(0);
			if (reading == 0b11111111)
				break;
		}
	}
		
}

bool forward()
{
	count1++;
	uint8_t reading ,count = 0;
	update_i2cAddr(6); 				// 6 - Address of Left TOF sensor

	while(1)
	{
		count++;
		PORTA |= (1<<motorL1)|(1<<motorR1);
		_delay_ms(16);
		PORTA &= ~(1<<motorR1);
		_delay_ms(2);
		all_low();

		if( mcp23017_readpinsA(0) > 0b00000000 && count>=20)
		{
			PORTA |= (1<<motorL1) | (1<<motorR1);
			_delay_ms(30);
			all_low();
			reading = mcp23017_readpinsA(0);
			if(reading == 0b00110000 || reading == 0b00011000 || reading == 0b00001100 || reading == 0b01100000 || reading == 0b11000000 || reading == 0b00111000 || reading == 0b00011100 || reading == 0b00000010 || reading == 0b00100000 || reading == 0b00010000 || reading == 0b00001000 || reading == 0b00000100)
			{
				return false ;
			}
			break;
		}
		if (readRangeSingleMillimeters(0) <= 50)
			return true;
		
	}
	_delay_ms(1000);
	reading = mcp23017_readpinsA(0);
	if(count1<14)
	{
	while(reading == 0b00000000)
	{
		PORTA |= (1<<motorL2) | (1<<motorR2);
		_delay_ms(2);
		all_low();
		_delay_ms(5);
		reading = mcp23017_readpinsA(0);
	}
	reading = mcp23017_readpinsA(0);
	if(reading >= 0b00001000)
	{
		while (1)
		{
			PORTA |= (1<<motorR2);
			_delay_ms(2);
			all_low();
			_delay_ms(5);
			reading = mcp23017_readpinsA(0);
			if (reading == 0b11111111)
				break;
		}
	}
	else
	{
		while (1)
		{
			PORTA |= (1<<motorL2);
			_delay_ms(2);
			all_low();
			_delay_ms(5);
			reading = mcp23017_readpinsA(0);
			if (reading == 0b11111111)
				break;
		}
	}
	}
}


void right_turn( bool wall)
{
	int count =0;
	if (wall == false)
		for (int i =0 ; i< 8 ; i++)
		{
			PORTA |= (1<<motorL2) | (1<<motorR2);
			_delay_ms(5);
			all_low();
			_delay_ms(10);
		}
	_delay_ms(1000);

	while(1)
	{
		count++;
		PORTA |= (1<<motorL1)|(1<<motorR2);
		_delay_ms(10);
		all_low();
		_delay_ms(20);
		if( count >=  20)
			break;
	}	
	_delay_ms(1000);
	align();
	
}


void left_turn(bool wall)
{
	int count =0;
	if (wall == false)
		for (int i =0 ; i< 8 ; i++)
		{
			PORTA |= (1<<motorL2) | (1<<motorR2);
			_delay_ms(5);
			all_low();
			_delay_ms(10);
		}
	_delay_ms(1000);

	while(1)
	{
		count++;
		PORTA |= (1<<motorR1)|(1<<motorL2);
		_delay_ms(10);
		all_low();
		_delay_ms(20);
		if( count >=  20)
			break;
		
	}	
	_delay_ms(1000);
	align();
	
}


void folow_line()
{
	int flag = 0;

	uint8_t reading;
	while(1)
	{
		reading = mcp23017_readpinsA(0);
		if (reading == 0b00011000)
		{
			PORTA |= (1<<motorL1 | 1<<motorR1);
			_delay_ms(5);
			all_low();
			_delay_ms(8);
		}

		
		if (reading == 0b11111111)
		{
			flag =1;
			_delay_ms(1000);
			while(1)
			{
				PORTA |= (1<<motorR1);
				_delay_ms(3);
				all_low();
				_delay_ms(5);
				reading = mcp23017_readpinsA(0);
				if (reading & 0b00010000)
					break;
			}
		}

		if ((reading == 0b00011111 || reading == 0b00001111 || reading == 0b00000111 || reading == 0b00011110 || reading == 0b00111111 || reading == 0b01111111) && flag ==1)
		{
			_delay_ms(1000);
			while(reading != 0b10000000)
			{
				PORTA |= (1<<motorL1);
				_delay_ms(4);
				all_low();
				_delay_ms(5);
				reading = mcp23017_readpinsA(0);
			}
			_delay_ms(1000);
			while(1)
			{
				if (reading & 0b00011000)
					return ;
				PORTA |= (1<<motorR2);
				_delay_ms(2);
				PORTA |= (1<<motorL1);
				_delay_ms(2);
				all_low();
				_delay_ms(8);
				reading = mcp23017_readpinsA(0);
				
			}
			
		}

		if( reading >= 0b00110000)
		{
			PORTA |= (1<<motorR1);
			_delay_ms(5);
			PORTA |= (1<<motorL1);
			_delay_ms(5);
			all_low();
			_delay_ms(10);
		}
		if( reading <= 0b00100000)
		{
			PORTA |= (1<<motorL1);
			_delay_ms(5);
			PORTA |= (1<<motorR1);
			_delay_ms(5);
			all_low();
			_delay_ms(10);
		}
	}
	PORTA |= (1<<motorL1) || (1<<motorR1);
	_delay_ms(10);

}

char uart0_readByte(void){

	uint16_t rx;
	uint8_t rx_status, rx_data;

	rx = uart0_getc();
	rx_status = (uint8_t)(rx >> 8);
	rx = rx << 8;
	rx_data = (uint8_t)(rx >> 8);

	if(rx_status == 0 && rx_data != 0){

		return rx_data;
	} else {
		return -1;
	}

}




int main(void) {

	uart0_init(UART_BAUD_SELECT(115200, F_CPU)); 
	uart0_flush();

	int flag = 0 ,Turn = 0 , fire_flag =0 , end = 0;
	unsigned char status ;						
	char rx_byte , curr_direction = 'R';				// Current Direction indicates the direction of the BOT
	uint8_t reading;
	
	DDRH |= (1<<RED_LED) | (1<<GREEN_LED);										// Initialize RED and GREEN LEDs
	PORTH |= ((1<<RED_LED) | (1<<GREEN_LED));

	DDRE &= ~( 1<<user_sw );													// Initialize USER_SW Button
	DDRA |= (1<<motorL1) | (1<<motorL2) | (1<<motorR1) | (1<<motorR2);			// Initialize Motor Pins
	all_low();

	mcp23017_init();											// Initialize LFA
	mcp23017_setmodeA(0,MCP23017_MODEINPUTALL);

	uint8_t addr[3]={6,12,14};
	uint16_t dist[3];
	char dist_s[100];
	all_off();

	bool wall = false;

	sensorL(); 
	initVL53L0X(1);
	setAddress(addr[0]);

	while(1){

		status = PINE >> user_sw;
		if (status == 0 && flag != 1)
		{
			flag =1;									// If USER_SW Button Pressed Set Flag to 1
			uart0_puts("@started@");
			
		}



		if ( (flag == 1) && (end == 0) )
		{
		
			_delay_ms(30);
			uart0_puts("request_data");
			rx_byte = uart0_readByte();					// Recive Instruction from ESP
			if(!isalpha(rx_byte))
				continue;
		
			
			if ( rx_byte == 'e')						// When Fire Zone Reached
			{
				fire_flag = 1;
				continue;
			}

			if ( rx_byte == 'E') 						// When END reached
			{
				folow_line();
            	PORTH &= ~(1<<GREEN_LED); 
            	_delay_ms(1000);
            	PORTH |= (1<<GREEN_LED); 
            	uart0_puts("@HA reached, Task accomplished!@");
				main();
			}
			if ( curr_direction != rx_byte)				// When Current Direction does not match with Required Direction
			{											// Rotate BOT according to the need


				// Left Turn 
				if ( ((curr_direction == 'L') && (rx_byte == 'D')) || ((curr_direction == 'D') && (rx_byte == 'R')) || ((curr_direction == 'R') && (rx_byte == 'U')) || ((curr_direction == 'U') && (rx_byte == 'L')) )			
				{
					left_turn(wall);
					//uart0_puts("-Left");
				}
				// Right Turn
				else 
				{
					right_turn(wall);
					//uart0_puts("-Right");
				}

			}	
			curr_direction = rx_byte;
			if ( fire_flag == 0 )
			{
				wall = forward();							// Move BOT Forward After taking care of Direction
				//uart0_puts("-Forward");
			}

			
			if ( fire_flag ==1 )							// After Reaching Fire zone
			{
				//uart0_puts("---\n \n FireZone \n \n---");
				//Switch ON RED LED for 1 sec
				PORTH &= ~(1<<RED_LED);
                _delay_ms(1000);
                PORTH |=  (1<<RED_LED);
                fire_flag = 0;
                
			}
		}
	}


	return 0;	
}