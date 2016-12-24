/*
 * board_configuration.h
 *
 *  Created on: 17/11/2016
 *      Author: david.martinez.cid
 */

#ifndef BOARD_CONFIGURATION_H_
#define BOARD_CONFIGURATION_H_

#include "netif/xadapter.h"

typedef unsigned char mac_addr[6];

#define BOARD_S 7 /*ID of the server*/
#define BOARD_C 8 /*ID of the client*/

#define CONNECTION 20
#define WAITOPLAY 30
#define PLAYING 40
#define TIMESHARING 50
#define PLAYAGAIN 60

int myState;

void set_state(int state);
int get_state();
void assign_ip (int board_num, struct ip_addr *ipaddr,
							   struct ip_addr *netmask,
							   struct ip_addr *gw);
void assign_mac_addr (int board_num, mac_addr *mac_addr);
static void print_ip(char *msg, struct ip_addr *ip);
void print_ip_settings(struct ip_addr *ip, struct ip_addr *mask, struct ip_addr *gw);
void print_mac_addr(mac_addr *macaddr);
int sequence_to_led_mapping(int recv_seq);


#endif /* BOARD_CONFIGURATION_H_ */
