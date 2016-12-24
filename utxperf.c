/*
 * Copyright (c) 2008 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "board_configuration.h"
#include "display.h"
#include "bls.h"


#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif


#define SEND_BUFSIZE 20


static char seq1[20] = {1,4,6,3,5,3,6,7,0,1,2,6,1,4,7,3,1,0,4,5};
static char send_bors[1];
static char recv_bors[1];
static unsigned rxperf_port = 2222;


void utxperf_application_thread()
{
	struct ip_addr ipaddr, netmask, gw;
	struct ip_addr ipaddr2;
	int  n, sock, sock2, j, i=0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in local_addr;
	char send_buf[SEND_BUFSIZE];
	int seq_play[SEND_BUFSIZE];

//	int myTime;
//	char hisTime=NULL;


	while(1){

		if(get_state() == CONNECTION){

			xil_printf("Server address:\r\n");
			IP4_ADDR(&ipaddr,  147, 83,   81, 220 + BOARD_S); /* iperf server address */
			IP4_ADDR(&ipaddr2,  147, 83,   81, 220 + BOARD_C); /* iperf client address */
			IP4_ADDR(&netmask, 255, 255, 255,  0);
			IP4_ADDR(&gw,      147, 83,   81,  1);

			/*Create socket*/
			if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
				xil_printf("error creating socket\r\n");
				return;
			}
            

			memset((void*)&serv_addr, 0, sizeof serv_addr);
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(5001);
			serv_addr.sin_addr.s_addr = ipaddr.addr;

			if ((sock2 = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					xil_printf("Error creating socket 2\r\n");
					return;
				}

				memset((void*)&local_addr, 0, sizeof local_addr);
				local_addr.sin_family = AF_INET;
				local_addr.sin_port = htons(rxperf_port);
				local_addr.sin_addr.s_addr = ipaddr2.addr;

			print("Connecting to iperf UDP server 1...\r\n");
			if (lwip_connect(sock, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
				xil_printf("ERROR: Cannot establish connection\r\n");
				vTaskDelete(NULL);
				connection_failure();
				return;
			}

			if (lwip_bind(sock2, (struct sockaddr *)&local_addr, sizeof (local_addr)) < 0) {
							xil_printf("Error binding socket 2\r\n");
							vTaskDelete(NULL);
							connection_failure();
						return;
			}
			connection_ok();
			print("Connection established\r\n");

			/*Random sequence generation*/
			for (n = 0; n<SEND_BUFSIZE ; n++){
					send_buf[n] = seq1[n] + '0';
			}

			/* keep sending data */
			xil_printf("\r\n\r\nWriting sequence to socket:\r\n");

			if ((lwip_write(sock, (const void*)send_buf, 20)) < 0){
				xil_printf("\r\nError sending sequence\r\n");
			}
			xil_printf("Sequence sent: ");
			for(i = 0; i<SEND_BUFSIZE; i++){
				xil_printf("%c ",send_buf[i]);
				seq_play[i] = send_buf[i] - '0';
			}
			xil_printf("\r\n\r\n");
			set_state(WAITOPLAY);
		}

		if(get_state() == WAITOPLAY){

			ready2play();
			xil_printf("Do you want to play?\r\n");
			xil_printf("Yes - Press any button\r\n");
			xil_printf("No - Press any switch\r\n");

			STARTAGAIN:
			while(1){
				int button_value, switches_value;
				button_value = read_button();
				switches_value = read_switches();
				if(button_value == RIGHT_BUTTON || button_value == DOWN_BUTTON ||
						button_value == LEFT_BUTTON || button_value == UP_BUTTON ||
						button_value == CENTER_BUTTON){
					send_bors[0] = BUTTON + '0';
					waiting_answer();
					break;
				}
				else if(switches_value == SWITCH_0 || switches_value == SWITCH_1 ||
						switches_value == SWITCH_2 || switches_value == SWITCH_3 ||
						switches_value == SWITCH_4 || switches_value == SWITCH_5 ||
						switches_value == SWITCH_6 || switches_value == SWITCH_7){
					send_bors[0] = SWITCH + '0';
					break;
				}
			}

			if ((lwip_write(sock,(const void*)send_bors, 1)) < 0){
					xil_printf("\r\nError answering opponent\n");
			}

			xil_printf("\r\nYou have pressed: %c\n", send_bors[0]);

			if((lwip_read(sock2, recv_bors, 1)) < 0){
					xil_printf("\r\nError reading answer from opponent\n");
				}

			xil_printf("\r\nThe opponent has pressed: %c \r\n", recv_bors[0]);

			if(recv_bors[0] == BUTTON + '0' && send_bors[0] == BUTTON + '0'){
				set_state(PLAYING);
			}else if(send_bors[0] == SWITCH + '0'){
				you_dont_want_to_play();
				/* Closing sockets */
				lwip_close(sock);
				lwip_close(sock2);
				break;
			}else if(recv_bors[0] == SWITCH + '0'){
				opponent_dont_want_to_play();
				/* Closing sockets */
				lwip_close(sock);
				lwip_close(sock2);
				break;
			}
		}

		if (get_state() == PLAYING){

			countdown2play();
//			myTime = play_game(seq_play);
			play_game(seq_play);
			set_state(PLAYAGAIN);
		}

//		if (get_state() == TIMESHARING){
//
//			if ((lwip_write(sock,(const void*)myTime, 1)) < 0){
//				xil_printf("\r\nError sending myTime\n");
//			}
//
//			xil_printf("\r\nYour time is: %d\r\n", myTime - '0');
//
//			if((lwip_read(sock2, hisTime,6)) < 0){
//				xil_printf("\r\nError reading hisTime\n");
//			}
//			xil_printf("\r\nThe opponent's time is: %d\r\n", hisTime - '0');
//
//			xil_printf("\r\n\r\n");
//			if(myTime - '0' <= hisTime - '0'){
//				xil_printf("You are the winner !!");
//				disp_winner();
//			}else{
//				xil_printf("You are the loser !!");
//				disp_loser();
//			}
//			set_state(PLAYAGAIN);
//		}

		if(get_state() == PLAYAGAIN){

			play_again();
			xil_printf("\r\n\r\n");
			xil_printf("Do you want to play again?\r\n");
			xil_printf("Yes - Press any button\r\n");
			xil_printf("No - Press any switch\r\n");
			goto STARTAGAIN;

		}
	}
	xil_printf("Closing game...\r\n");
	sleep(3);
	xil_printf("Game closed!\r\n");
}



