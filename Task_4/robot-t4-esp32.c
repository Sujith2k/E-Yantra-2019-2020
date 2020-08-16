#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>

#include "driver/uart.h"
#include "driver/gpio.h"

#define ECHO_TEST_TXD  (GPIO_NUM_32)            // Connected to AVR Rx-0
#define ECHO_TEST_RXD  (GPIO_NUM_33)            // Connected to AVR Tx-0
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

#define PORT 3333				// Port address for socket communication
#define RX_BUFFER_SIZE 128




static const char *TAG = "ESP32";
static EventGroupHandle_t s_wifi_event_group ;

/* Wi-Fi event handler */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }
    return ESP_OK;
}


/* Function to initialize Wi-Fi at station */
void wifi_init_softap() //my_wifi_config my_wifi
{
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid="eYFi-Wireless-Serial",
            .password="eyantra123",
            .ssid_len = 0,
            .channel = 6,
            .authmode = WIFI_AUTH_OPEN, //WIFI_AUTH_WPA_WPA2_PSK, //WIFI_AUTH_OPEN
            .ssid_hidden = 0,
            .max_connection=4,
            .beacon_interval = 100
        },
    };

    printf(">>>>>>>> SSID: %s <<<<<<<<<\n", wifi_config.ap.ssid);
    printf(">>>>>>>> PASS: %s <<<<<<<<<\n", wifi_config.ap.password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}


void app_main()
{
	
	ESP_ERROR_CHECK(nvs_flash_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	wifi_init_softap();

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data_uart = (uint8_t *) malloc(BUF_SIZE);
	int len_uart = 0;
    char ack_buffer[100];

	char rx_buffer[RX_BUFFER_SIZE];     // buffer to store data from client
	char sen_data[RX_BUFFER_SIZE];
	char ipv4_addr_str[128];            // buffer to store IPv4 addresses as string
	char ipv4_addr_str_client[128];     // buffer to store IPv4 addresses as string
	int addr_family;
	int ip_protocol,len1;

	while (1)
	{

		struct sockaddr_in dest_addr;
		dest_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_port = htons(PORT);
		addr_family = AF_INET;
		ip_protocol = IPPROTO_IP;
		
		inet_ntop( AF_INET, &dest_addr.sin_addr, ipv4_addr_str, INET_ADDRSTRLEN );
		printf("[DEBUG] Self IP = %s\n", ipv4_addr_str);


		int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
		if (listen_sock < 0) {
			printf("[ERROR] Unable to create socket. ERROR %d\n", listen_sock);
			break;
		}
		printf("[DEBUG] Socket created\n");

		int flag = 1;
		setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

		int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
		if (err != 0) {
			printf("[ERROR] Socket unable to bind. ERROR %d\n", err);
			break;
		}

		printf("[DEBUG] Socket bound, port %d\n", PORT);

		err = listen(listen_sock, 1);
		if (err != 0) {
			printf("[ERROR] Error occurred during listen. ERROR %d\n", err);
			break;
		}

		printf("[DEBUG] Socket listening\n");

		struct sockaddr_in6 source_addr; // Can store both IPv4 or IPv6
		uint addr_len = sizeof(source_addr);
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0) {
			printf("[ERROR] Error occurred during listen. ERROR %d\n", sock);
			break;
		}
		printf("[DEBUG] Socket accepted\n");

		// ********************************************************************************************
		// ********************************************************************************************

		// for digit_lists and combinations
		char digit_list[RX_BUFFER_SIZE];
		char combination[RX_BUFFER_SIZE];
		recv(sock, digit_list, sizeof(digit_list) - 1, 0);      					// use sizeof or strlen
		recv(sock, combination, sizeof(combination) - 1, 0);
		printf("\n Recived from py client");
		int digit,bot_vac=4;

		// for @started@
        while(1)
        {
			

            len_uart=uart_read_bytes(UART_NUM_1,data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
            data_uart[len_uart]=0;

			if(len_uart<=1 )
				continue;
			printf("\n \n %s \n \n ",(char *)data_uart);
            
			if(!strcmp((char *)data_uart,"@started@"))
            {	
				sprintf(sen_data,"%s",(char *)data_uart);
				len1 = strlen(sen_data);
				sen_data[len1] = 0; 	

				send(sock,sen_data,len1,0); 								 // use sizeof or strlen
                break;
            }
        }
			

		// getting new path from fire zone
		while (1)
		{
			
			digit = (int)digit_list[2]; // finding no of vacencies in bot
			digit = digit -48;
			printf("\n%d",digit);
			printf("\n\n %s \n\n",digit_list);
	

			recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);		
			printf("%s",rx_buffer);
			// traevling the path


			while(strlen(rx_buffer)!=10)
			{
				len_uart = uart_read_bytes(UART_NUM_1,data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
                data_uart[len_uart]=0;
				sprintf(ack_buffer,"%s",(char *)data_uart);

				int lr = 0 , tb = 0;

				const char l1 = rx_buffer[3];
				const char l2 = rx_buffer[11];
				const char f1 = rx_buffer[6];
				const char f2 = rx_buffer[14];

				lr = (int)l1 - (int)l2 ;
				tb = (int)f1 - (int)f2 ;	

				if ( (len_uart < 2) || (ack_buffer[0] != 'r') )
					continue;

				printf("\nDATA FROM UART :%s",ack_buffer);
				if(strlen(rx_buffer)==18 && bot_vac !=0)
				{
					sprintf(ack_buffer, "e");
					uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));	
					len_uart = uart_read_bytes(UART_NUM_1,data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
                    data_uart[len_uart]=0;				

				}

				if(lr == 1)
				{
					printf("\nmoving U");
					sprintf(ack_buffer, "U");
					uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));
				}

				if(lr == -1)
				{
					printf("\nmoving D");
					sprintf(ack_buffer, "D");
					uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));
				}

				if(tb == 1)
				{
					printf("\nmoving L");
					sprintf(ack_buffer, "L");
					uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));
				}

				if(tb == -1)
				{
					printf("\nmoving R");
					sprintf(ack_buffer, "R");
					uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));
				}
                vTaskDelay(1000 / portTICK_PERIOD_MS);

				// To remove the first coordinate from the path
				int idxToDel = 2;
				int i=0;
				for(i=0;i<8;i++) 
					memmove(&rx_buffer[idxToDel], &rx_buffer[idxToDel + 1], strlen(rx_buffer) - idxToDel);
				printf("\n %s",rx_buffer);

			}
			

			bot_vac -= 2;


			if(!strcmp(rx_buffer,"#[(9, 9)]#"))
			{	
				sprintf(ack_buffer, "E");
				uart_write_bytes(UART_NUM_1, (const char *) ack_buffer, strlen(ack_buffer));
				while(1)
				{
					len_uart = uart_read_bytes(UART_NUM_1,data_uart, BUF_SIZE, 20 / portTICK_RATE_MS);
                	data_uart[len_uart]=0;
					sprintf(ack_buffer,"%s",(char *)data_uart);
					if( ((len_uart >2) && (ack_buffer[0] == '@')) )
					{
						printf("\n@HA reached, Task accomplished!@\n");
								
						strcpy(sen_data,"@HA reached, Task accomplished!@");
						send(sock,sen_data,strlen(sen_data),0);  		// use sizeof or strlen
						break;
					}
				}
				break;
			}
			else
			{	int remv;

				if(strcmp(rx_buffer,"#[(9, 9)]#"))
				{
				
					// removing '#' and '[]' from 
					remv = 5;
					memmove(&rx_buffer[remv], &rx_buffer[remv + 1], strlen(rx_buffer) - remv);
					remv = 7;
					memmove(&rx_buffer[remv], &rx_buffer[remv + 1], strlen(rx_buffer) - remv);
					remv = 7;
					memmove(&rx_buffer[remv], &rx_buffer[remv + 1], strlen(rx_buffer) - remv);
					remv = 0;
					memmove(&rx_buffer[remv], &rx_buffer[remv + 1], strlen(rx_buffer) - remv);
					remv = 0;
					memmove(&rx_buffer[remv], &rx_buffer[remv + 1], strlen(rx_buffer) - remv);	

					printf("__________________________________________________\n");
					printf("%s\n",rx_buffer);
					printf("___________________________________________________\n");

					sprintf(sen_data,"@$|%d|",2);
					strcat(sen_data,rx_buffer);
					strcat(sen_data,"@");	
					printf("\n%s",sen_data);
					printf("%d",strlen(sen_data));	

				}
				

				
				send(sock,sen_data,strlen(sen_data),0) ; 								 // use sizeof or strlen
			}	

		}
		// ********************************************************************************************
		// ********************************************************************************************


	}
}