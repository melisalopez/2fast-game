/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
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

#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/sys.h"

#include "config_apps.h"
#ifdef __arm__
#include "xil_printf.h"
#endif

void print_utxperf_app_header();
void print_urxperf_app_header();

void print_headers()
{
//
//    if (INCLUDE_UTXPERF_CLIENT)
//    	print_utxperf_app_header();

    if (INCLUDE_URXPERF_SERVER)
    	print_urxperf_app_header();

    xil_printf("\r\n");
}

void utxperf_application_thread(void *);
void urxperf_application_thread(void *);

void launch_app_threads()
{
    if (INCLUDE_UTXPERF_CLIENT)
            sys_thread_new("utxperfd", utxperf_application_thread, 0,
                THREAD_STACKSIZE,
                DEFAULT_THREAD_PRIO);

//    if (INCLUDE_URXPERF_SERVER)
//                sys_thread_new("urxperfd", urxperf_application_thread, 0,
//                    THREAD_STACKSIZE,
//                    DEFAULT_THREAD_PRIO);
}
