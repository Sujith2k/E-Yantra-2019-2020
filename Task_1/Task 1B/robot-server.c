/*
*****************************************************************************************
*
*        		===============================================
*           		Rapid Rescuer (RR) Theme (eYRC 2019-20)
*        		===============================================
*
*  This script is to implement Task 1B of Rapid Rescuer (RR) Theme (eYRC 2019-20).
*  
*  This software is made available on an "AS IS WHERE IS BASIS".
*  Licensee/end user indemnifies and will keep e-Yantra indemnified from
*  any and all claim(s) that emanate from the use of the Software or 
*  breach of the terms of this agreement.
*  
*  e-Yantra - An MHRD project under National Mission on Education using ICT (NMEICT)
*
*****************************************************************************************
*/

/*
* Team ID:			1662
* Author List:		Thiruvikkraman,Sujith Krishna,Kaushek Kumar,Arnab Das
* Filename:			robot-server.c
* Functions:		socket_create, receive_from_send_to_client
* 					[ Comma separated list of functions in this file ]
* Global variables:	SERVER_PORT, RX_BUFFER_SIZE, TX_BUFFER_SIZE, MAXCHAR,
* 					dest_addr, source_addr, rx_buffer, tx_buffer,
* 					ipv4_addr_str, ipv4_addr_str_client, listen_sock, line_data, input_fp, output_fp
* 					[ List of global variables defined in this file ]
*					repeat
*/


// Include necessary header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>


// Constants defined
#define SERVER_PORT 3333
#define RX_BUFFER_SIZE 1024
#define TX_BUFFER_SIZE 1024

#define MAXCHAR 1000				// max characters to read from txt file

// Global variables
struct sockaddr_in dest_addr;
struct sockaddr_in source_addr;

char rx_buffer[RX_BUFFER_SIZE];		// buffer to store data from client
char tx_buffer[RX_BUFFER_SIZE];		// buffer to store data to be sent to client

char ipv4_addr_str[128];			// buffer to store IPv4 addresses as string
char ipv4_addr_str_client[128];		// buffer to store IPv4 addresses as string

int listen_sock;

char line_data[MAXCHAR];

FILE *input_fp, *output_fp;
int repeat =0;

/*
* Function Name:	socket_create
* Inputs:			dest_addr [ structure type for destination address ]
* 					source_addr [ structure type for source address ]
* Outputs: 			my_sock [ socket value, if connection is properly created ]
* Purpose: 			the function creates the socket connection with the server
* Example call: 	int sock = socket_create(dest_addr, source_addr);
*/
int socket_create(struct sockaddr_in dest_addr, struct sockaddr_in source_addr){

	int addr_family;
	int ip_protocol;

	dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(SERVER_PORT);
	addr_family = AF_INET;
	ip_protocol = IPPROTO_IP;

	int my_sock;

	int ser_sock;
	ser_sock = socket(AF_INET , SOCK_STREAM,0);
	bind(ser_sock ,(struct sockaddr*)&dest_addr,sizeof(dest_addr ));
	listen(ser_sock,5);

	socklen_t peer_addr_size;
	peer_addr_size = sizeof(struct sockaddr*);
    my_sock = accept(ser_sock,(struct sockaddr*)&source_addr,&peer_addr_size);


	return my_sock;
}


/*
* Function Name:	receive_from_send_to_client
* Inputs:			sock [ socket value, if connection is properly created ]
* Outputs: 			None
* Purpose: 			the function rec					    //tx_buffer[p]='\0';eives the data from server and updates the 'rx_buffer'
*					variable with it, sends the obstacle position based on obstacle_pos.txt
*					file and sends this information to the client in the provided format.
* Example call: 	receive_from_send_to_client(sock);
*/
int receive_from_send_to_client(int sock){

	//reciving data	
	recv(sock,&rx_buffer,sizeof(rx_buffer),0);

	char file_num = rx_buffer[0];
	
	int flag2=0;
	if(file_num!=line_data[0])
	{
		flag2=1;
	}

	int i=0;	
	int sum=0;
	for(i=0;line_data[i];i++)
	{
	sum+=1;					    
	} 

	if(flag2==0)
	{
		if((sum>=3))
		{
			int j=0;
			while(line_data[j])
			{
				int size=0;
				if(line_data[j]=='(')
				{	int flag=0,p;
					for(p=j;line_data[p];p++)
					{
						
						if(line_data[p]==')')
						{
							flag=1;
							size+=2;
							tx_buffer[p-j+1]=')';
							tx_buffer[p-j+2]='@';
							break;
						}	
						if((p-j)==0)
						{
							size+=2;
							tx_buffer[p-j]='@';
							tx_buffer[p-j+1]='(';
						}
						else
						{
							tx_buffer[p-j+1]=line_data[p];
							size++;
						}
					}
					if(flag!=0)
					{
						send(sock,&tx_buffer,size,0);
						recv(sock,&rx_buffer,sizeof(rx_buffer),0);

					}
					flag=0;
				}
				
				j++;
			}

		}

	}

	char temp[] = "@$@";
	send(sock,&temp,3,0);
	if((repeat !=0) && (flag2 !=1))
	{

		fgets(line_data ,MAXCHAR, input_fp);
		fgets(line_data ,MAXCHAR, input_fp);
		
	}
	repeat =1;
	return 0;

}


/*
* Function Name:	main()
* Inputs:			None
* Outputs: 			None
* Purpose: 			the function solves Task 1B problem statement by making call to
* 					functions socket_create() and receive_from_send_to_client()
*/
int main() {
	
    char *input_file_name = "obstacle_pos.txt";
	char *output_file_name = "data_from_client.txt";

	// Create socket and accept connection from client
	int sock = socket_create(dest_addr, source_addr);

	input_fp = fopen(input_file_name, "r");

	if (input_fp == NULL){
		printf("Could not open file %s\n",input_file_name);
		return 1;
	}

	fgets(line_data, MAXCHAR, input_fp);

	output_fp = fopen(output_file_name, "w");

	if (output_fp == NULL){
		printf("Could not open file %s\n",output_file_name);
		return 1;
	}

	while (1) {

		// Receive and send data from client and get the new shortest path
		receive_from_send_to_client(sock);

		// NOTE: YOU ARE NOT ALLOWED TO MAKE ANY CHANGE HERE
		fputs(rx_buffer, output_fp);
		fputs("\n", output_fp);

	}

	return 0;
}

