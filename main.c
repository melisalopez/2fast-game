
/*
 * Copyright (c) 2007-2009 Xilinx, Inc.  All rights reserved.
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
#include <xgpio.h>

#include "xparameters.h"
#include "lwipopts.h"
#include "ZedboardOLED.h"

#include "platform_config.h"
#include "platform.h"
#include "board_configuration.h"

#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/init.h"
#include "netif/xadapter.h"
#include "lwip/dhcp.h"
#include "config_apps.h"
#if __arm__
#include "task.h"
#include "portmacro.h"
#include "xil_printf.h"
int main_thread();
#endif
void print_headers();
void launch_app_threads();

int main()
{
	clear();

    Xil_Out32(XPAR_LEDS_BASEADDR,0x00);


    sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
                THREAD_STACKSIZE,
                DEFAULT_THREAD_PRIO);
    vTaskStartScheduler();
    while(1);

    return 0;
}

struct netif server_netif;

void network_thread(void *p)
{
    struct netif *netif;
    struct ip_addr ipaddr, netmask, gw;
    mac_addr mac_ethernet_address;

    /* Server MAC address assignment */
    assign_mac_addr(BOARD_C, &mac_ethernet_address);
    print_mac_addr(&mac_ethernet_address);

    netif = &server_netif;

    /* Server IP address assignment */
    assign_ip(BOARD_C, &ipaddr, &netmask, &gw);
    print_ip_settings(&ipaddr, &netmask, &gw);

    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
        xil_printf("Error adding N/W interface\r\n");
        return;
    }
    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

	set_state(CONNECTION);
    launch_app_threads();
    return;
}

int main_thread()
{

    /* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("NW_THRD", network_thread, NULL,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

#ifdef OS_IS_FREERTOS
   vTaskDelete(NULL);
#endif


    return 0;
}
#ifdef __arm__
void vApplicationMallocFailedHook( void )
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue or
   semaphore is created.  It is also called by various parts of the demo
   application.  If heap_1.c or heap_2.c are used, then the size of the heap
   available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
   xil_printf("Memory Allocation Error\r\n");
   taskDISABLE_INTERRUPTS();
   for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
   ( void ) pcTaskName;
   ( void ) pxTask;

   /* vApplicationStackOverflowHook() will only be called if
   configCHECK_FOR_STACK_OVERFLOW is set to either 1 or 2.  The handle and name
   of the offending task will be passed into the hook function via its
   parameters.  However, when a stack has overflowed, it is possible that the
   parameters will have been corrupted, in which case the pxCurrentTCB variable
   can be inspected directly. */
   xil_printf("Stack Overflow in %s\r\n", pcTaskName);
   taskDISABLE_INTERRUPTS();
   for( ;; );
}
void vApplicationSetupHardware( void )
{

}

#endif

