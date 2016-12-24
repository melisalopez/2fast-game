/*
 * urxperf.c
 *
 *  Created on: Aug 29, 2012
 *      Author: anirudh
 */

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
#include <xgpio.h>
#include <xparameters.h>

#include "bls.h"
#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "board_configuration.h"
#include "display.h"

#ifdef __arm__
#include "FreeRTOS.h"
#include "task.h"
#include "xil_printf.h"
#endif

#define RECV_SEQUENCE_SIZE (20)

static unsigned rxperf_port = 5001;
static unsigned rxperf_server_running = 0;
static char send_bors[1] = {NULL};
static char recv_bors[1] = {NULL};
static char send_bors2[1] = {NULL};
static char recv_bors2[1] = {NULL};


void print_urxperf_app_header()
{
	printf("%20s %6d %10s %s\r\n", "rxperf server",
                        rxperf_port,
                        rxperf_server_running ? "RUNNING" : "INACTIVE",
                        "$ iperf -c <board ip> -i 5 -t 100 -u -b <bandwidth>");
}

void urxperf_application_thread()
{
	int sock, sock2, i = 0, j = 0;
	struct ip_addr ipaddr;
	struct ip_addr ipaddr2;
	struct sockaddr_in local_addr;
	struct sockaddr_in serv_addr;
	char recv_seq[RECV_SEQUENCE_SIZE];
	int seq_play[RECV_SEQUENCE_SIZE];

//	int myTime, hisTime;

	IP4_ADDR(&ipaddr,  147, 83,   81, 220 + BOARD_S);
	IP4_ADDR(&ipaddr2,  147, 83,   81, 220 + BOARD_C);

	while(1){

		if(get_state() == CONNECTION){

			if ((sock = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
					xil_printf("Error creating socket\r\n");
					return;
			}

			memset((void*)&local_addr, 0, sizeof local_addr);
			local_addr.sin_family = AF_INET;
			local_addr.sin_port = htons(rxperf_port);
			local_addr.sin_addr.s_addr = ipaddr.addr;

			if ((sock2 = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
							xil_printf("Error creating socket 2\r\n");
							connection_failure();
							return;
			}

			memset((void*)&serv_addr, 0, sizeof serv_addr);
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(2222);
			serv_addr.sin_addr.s_addr = ipaddr2.addr;

			if (lwip_bind(sock, (struct sockaddr *)&local_addr, sizeof (local_addr)) < 0) {
				xil_printf("Socket 2 binding error\r\n");
				vTaskDelete(NULL);
				connection_failure();
				return;
			}

			if (lwip_connect(sock2, (struct sockaddr *)&serv_addr, sizeof (serv_addr)) < 0) {
				xil_printf("ERROR: Cannot establish connection in 2\r\n");
				vTaskDelete(NULL);
				connection_failure();
				return;
			}

			connection_ok();
			xil_printf("Both Connections established\r\n\r\n");

			/* keep reading data */
			xil_printf("Reading data from client:\r\n");
			if (lwip_read(sock, recv_seq, RECV_SEQUENCE_SIZE) <= 0){
				sequence_received_failure();
				xil_printf("Error reading from socket");
			}

			xil_printf("Sequence received: ");

			for(i=0; i<RECV_SEQUENCE_SIZE; i++){
				xil_printf("%c ",recv_seq[i]);
				seq_play[i] = recv_seq[i] - '0';
			}

			sequence_received_ok();
			xil_printf("\r\n\r\n");
			set_state(WAITOPLAY);
		}

		if(get_state() == WAITOPLAY){
			ready2play();
			xil_printf("Do you want to play?\r\n");
			xil_printf("Yes - Press any button\r\n");
			xil_printf("No - Press any switch\r\n\r\n");

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

			/* keep reading data */
			if(lwip_read(sock, recv_bors, 1) <= 0){
				xil_printf("Error reading what has pressed the opponent\r\n");
			}
			xil_printf("The opponent has pressed: %c\r\n", recv_bors[0]);

			/* keep sending data */
			if((lwip_write(sock2, (const void*)send_bors, 1)) < 0)
				xil_printf("Error sending what pressed\r\n");

			xil_printf("You have pressed: %c\r\n", send_bors[0]);

			if((recv_bors[0] == BUTTON + '0') && (send_bors[0] == BUTTON + '0')){
				set_state(PLAYING);
			}else if(send_bors[0] == SWITCH + '0'){
				you_dont_want_to_play();
				/* Stop the connection - Socket closure */
				lwip_close(sock);
				lwip_close(sock2);

				break;
			}else if(recv_bors[0] == SWITCH + '0'){
				opponent_dont_want_to_play();
				/* Stop the connection - Socket closure */
				lwip_close(sock);
				lwip_close(sock2);
				break;
			}

		}

		if(get_state() == PLAYING){
			countdown2play();
			/* The game starts here */
//			myTime = play_game(seq_play)+'0';
			play_game(seq_play);
			set_state(PLAYAGAIN);
		}
//		if(get_state() == TIMESHARING){
//
//
//			/* sending time played */
//		if((lwip_write(sock2, (const void*)myTime, 1)) < 0)
//			xil_printf("Error sending my time\r\n");
//		myTime = myTime -'0';
//		xil_printf("You have sent my Time: %d\r\n", myTime);
//
//		/* reading hisTime */
//		if(lwip_read(sock, hisTime, 1) <= 0){
//			xil_printf("Error reading his time\r\n");
//		}
//		hisTime = hisTime -'0';
//		xil_printf("The opponent's time is: %d\r\n", hisTime);
//		if (myTime<hisTime){
//			xil_printf("You WIN!\r\n");
//			disp_winner();
//		}else{
//			xil_printf("You lose :-( \r\n");
//			disp_loser();
//		}
//
//		set_state(PLAYAGAIN);
//		}
		if(get_state() == PLAYAGAIN){

			/* Asking to play again */
			play_again();
			xil_printf("Do you want to play again?\r\n");
			xil_printf("Yes - Press any button\r\n");
			xil_printf("No - Press any switch\r\n\r\n");
			goto STARTAGAIN;
		}
	}

	xil_printf("Closing game...\r\n");
	sleep(3);
	xil_printf("Game closed!\r\n");
}
