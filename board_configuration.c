/*
 * board_configuration.c
 *
 *  Created on: 17/11/2016
 *      Author: david.martinez.cid
 */


#include "lwip/inet.h"
#ifndef __PPC__
#include "xil_printf.h"
#endif
#include "board_configuration.h"

void set_state(int state)
{
	if(state == CONNECTION){
		myState = CONNECTION;
	}else if(state == WAITOPLAY){
		myState = WAITOPLAY;
	}else if(state == PLAYING){
		myState = PLAYING;
	}else if(state == TIMESHARING){
		myState = TIMESHARING;
	}else if(state == PLAYAGAIN){
		myState = PLAYAGAIN;
	}
}

int get_state()
{
	return myState;
}

void assign_ip(int board_num, struct ip_addr *ipaddr,
		struct ip_addr *netmask, struct ip_addr *gw)
{
	IP4_ADDR(ipaddr, 147, 83, 81, (220 + board_num));
	IP4_ADDR(netmask, 255, 255, 255, 0);
	IP4_ADDR(gw, 147, 83, 81, 1);
}

void assign_mac_addr(int board_num, mac_addr *macaddr)
{
	unsigned char pos4, pos5, pos6;

	switch (board_num) {
	case 1:
		pos4 = 0x28;
		pos5 = 0xb8;
		pos6 = 0xd5;
		break;
	case 2:
		pos4 = 0x7f;
		pos5 = 0x9f;
		pos6 = 0x39;
		break;
	case 3:
		pos4 = 0x7d;
		pos5 = 0x7c;
		pos6 = 0xde;
		break;
	case 4:
		pos4 = 0xd7;
		pos5 = 0x9f;
		pos6 = 0x67;
		break;
	case 5:
		pos4 = 0xa4;
		pos5 = 0xd7;
		pos6 = 0x4b;
		break;
	case 6:
		pos4 = 0x1c;
		pos5 = 0xba;
		pos6 = 0x7f;
		break;
	case 7:
		pos4 = 0xa4;
		pos5 = 0xd2;
		pos6 = 0x13;
		break;
	case 8:
		pos4 = 0x7d;
		pos5 = 0x25;
		pos6 = 0x8d;
		break;
	case 9:
		pos4 = 0x8a;
		pos5 = 0x79;
		pos6 = 0x38;
		break;
	case 10:
		pos4 = 0x0f;
		pos5 = 0xea;
		pos6 = 0x85;
		break;
	case 11:
		pos4 = 0x54;
		pos5 = 0x66;
		pos6 = 0xcb;
		break;
	case 12:
		pos4 = 0xfa;
		pos5 = 0xf6;
		pos6 = 0xab;
		break;
	default:
		pos4 = 0x00;
		pos5 = 0x00;
		pos6 = 0x00;
		break;
	}

	(*macaddr)[0] = 0x00;
	(*macaddr)[1] = 0x16;
	(*macaddr)[2] = 0x3e;

	(*macaddr)[3] = pos4;
	(*macaddr)[4] = pos5;
	(*macaddr)[5] = pos6;
}

static void print_ip(char *msg, struct ip_addr *ip)
{
	print(msg);
	xil_printf("%d.%d.%d.%d\r\n",
			ip4_addr1(ip),
			ip4_addr2(ip),
			ip4_addr3(ip),
			ip4_addr4(ip));
}

void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw)
{
	xil_printf("Assigned IP address:\n");
	print_ip("Board IP:       ", ip);
	print_ip("Netmask :       ", mask);
	print_ip("Gateway :       ", gw);
}

void print_mac_addr(mac_addr *macaddr)
{
	xil_printf("Assigned MAC address: %02X : %02X : %02X : %02X : %02X : %02X \r\n",
			(*macaddr)[0],
			(*macaddr)[1],
			(*macaddr)[2],
			(*macaddr)[3],
			(*macaddr)[4],
			(*macaddr)[5]);
}

int sequence_to_led_mapping(int recv_seq){
	int led_value = 0;

	if(recv_seq == 0){
		led_value = 1;
	}else if(recv_seq == 1){
		led_value = 2;
	}else if(recv_seq == 2){
		led_value = 4;
	}else if(recv_seq == 3){
		led_value = 8;
	}else if(recv_seq == 4){
		led_value = 0x10;
	}else if(recv_seq == 5){
		led_value = 0x20;
	}else if(recv_seq == 6){
		led_value = 0x40;
	}else{
		led_value = 0x80;
	}

	return led_value;
}
