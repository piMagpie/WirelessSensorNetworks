 /*
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "cpu.h" //zoul
#include "lib/random.h"
#include "sys/ctimer.h"
#include "sys/etimer.h" //zoul
#include "sys/rtimer.h" //zoul
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-udp-packet.h"
//#include "dev/sht11/sht11-sensor.h" /* lab3a */
//#include "dev/light-sensor.h" /* lab3a */
#include "dev/leds.h" //zoul
#include "dev/uart.h" //zoul
#include "dev/button-sensor.h" //zoul
#include "dev/adc-zoul.h" //zoul
#include "dev/zoul-sensors.h" //zoul
#include "dev/watchdog.h" //zoul
#include "dev/serial-line.h" //zoul
#include "dev/sys-ctrl.h" //zoul
#include "net/netstack.h" //zoul

#ifdef WITH_COMPOWER
#include "powertrace.h"
#endif
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define UDP_EXAMPLE_ID  190

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#ifndef PERIOD
#define PERIOD 10
#endif

#define START_INTERVAL		(10 * CLOCK_SECOND)
#define SEND_INTERVAL		(PERIOD * CLOCK_SECOND)
#define SEND_TIME		(random_rand() % (SEND_INTERVAL))
#define MAX_PAYLOAD_LEN		40

//zoul
#define LOOP_PERIOD         8
#define LOOP_INTERVAL       (CLOCK_SECOND * LOOP_PERIOD)
#define LEDS_OFF_HYSTERISIS ((RTIMER_SECOND * LOOP_PERIOD) >> 1)
#define LEDS_PERIODIC       LEDS_BLUE
#define LEDS_BUTTON         LEDS_RED
#define LEDS_SERIAL_IN      LEDS_GREEN
#define LEDS_REBOOT         LEDS_ALL
#define LEDS_RF_RX          (LEDS_YELLOW | LEDS_RED)
#define BUTTON_PRESS_EVENT_INTERVAL (CLOCK_SECOND)
//zoul

static struct uip_udp_conn *client_conn;
static uip_ipaddr_t server_ipaddr;
static int temperature; /* lab3a */
static int lights; /* lab3a */
static uint16_t counter; //zoul
/*---------------------------------------------------------------------------*/
PROCESS(udp_client_process, "UDP client process");
AUTOSTART_PROCESSES(&udp_client_process);
/*---------------------------------------------------------------------------*/
static int
get_light(void)
{
  //return 10 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) / 7;
  return vdd3_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED);
}
/*---------------------------------------------------------------------------*/
static int
get_temp(void)
{
  //return ((sht11_sensor.value(SHT11_SENSOR_TEMP) / 10) - 396) / 10;
  return cc2538_temp_sensor.value(CC2538_SENSORS_VALUE_TYPE_CONVERTED); 
}
/*---------------------------------------------------------------------------*/
static void
tcpip_handler(void)
{
  char *str;

  if(uip_newdata()) {
    str = uip_appdata;
    str[uip_datalen()] = '\0';
    printf("DATA recv '%s'\n", str);   
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet(void *ptr)
{
  static int seq_id;
  char buf[MAX_PAYLOAD_LEN];

  seq_id++;
  PRINTF("DATA send to %d 'Hello %d'\n",
         server_ipaddr.u8[sizeof(server_ipaddr.u8) - 1], seq_id);
  /*sprintf(buf, "Hello %d from the client", seq_id);*/
  sprintf(buf,"Lights = %d mV, Temperature = %d mC\n", lights, temperature);
  printf("Lights = %d mV, Temperature = %d mC\n", lights, temperature);
  uip_udp_packet_sendto(client_conn, buf, strlen(buf),
                        &server_ipaddr, UIP_HTONS(UDP_SERVER_PORT));
}
/*---------------------------------------------------------------------------*/
static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTF("Client IPv6 addresses: ");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINT6ADDR(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTF("\n");
      /* hack to make address "final" */
      if (state == ADDR_TENTATIVE) {
	uip_ds6_if.addr_list[i].state = ADDR_PREFERRED;
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
set_global_address(void)
{
  uip_ipaddr_t ipaddr;

  /*uip_ip6addr(&ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 0);*/
  uip_ip6addr(&ipaddr, 0x2001, 0x5c0, 0x1101, 0x5001, 0, 0, 0, 1);
  uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
  uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);

/* The choice of server address determines its 6LoPAN header compression.
 * (Our address will be compressed Mode 3 since it is derived from our link-local address)
 * Obviously the choice made here must also be selected in udp-server.c.
 *
 * For correct Wireshark decoding using a sniffer, add the /64 prefix to the 6LowPAN protocol preferences,
 * e.g. set Context 0 to aaaa::.  At present Wireshark copies Context/128 and then overwrites it.
 * (Setting Context 0 to aaaa::1111:2222:3333:4444 will report a 16 bit compressed address of aaaa::1111:22ff:fe33:xxxx)
 *
 * Note the IPCMV6 checksum verification depends on the correct uncompressed addresses.
 */
 
#if 0
/* Mode 1 - 64 bits inline */
   uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0, 0, 0, 1);
#elif 1
/* Mode 2 - 16 bits inline */
  uip_ip6addr(&server_ipaddr, 0x2001, 0x5c0, 0x1101, 0x5001, 0, 0, 0, 1);
#else
/* Mode 3 - derived from server link-local (MAC) address */
  uip_ip6addr(&server_ipaddr, 0xaaaa, 0, 0, 0, 0x0250, 0xc2ff, 0xfea8, 0xcd1a); //redbee-econotag
#endif
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_client_process, ev, data)
{
  static struct etimer periodic;
  static struct ctimer backoff_timer;
#if WITH_COMPOWER
  static int print = 0;
#endif

  PROCESS_BEGIN();

  PROCESS_PAUSE();

  counter = 0;

  /* Disable the radio duty cycle and keep the radio on */
  NETSTACK_MAC.off(1); //zoul

  /* Configure the user button */
  button_sensor.configure(BUTTON_SENSOR_CONFIG_TYPE_INTERVAL,
                          BUTTON_PRESS_EVENT_INTERVAL); //zoul

  /* Configure the ADC ports */
  adc_zoul.configure(SENSORS_HW_INIT, ZOUL_SENSORS_ADC_ALL); //zoul

  
  set_global_address();
  
  PRINTF("UDP client process started\n");

  print_local_addresses();

  /* new connection with remote host */
  client_conn = udp_new(NULL, UIP_HTONS(UDP_SERVER_PORT), NULL); 
  if(client_conn == NULL) {
    PRINTF("No UDP connection available, exiting the process!\n");
    PROCESS_EXIT();
  }
  udp_bind(client_conn, UIP_HTONS(UDP_CLIENT_PORT)); 

  PRINTF("Created a connection with the server ");
  PRINT6ADDR(&client_conn->ripaddr);
  PRINTF(" local/remote port %u/%u\n",
	UIP_HTONS(client_conn->lport), UIP_HTONS(client_conn->rport));

#if WITH_COMPOWER
  powertrace_sniff(POWERTRACE_ON);
#endif

  //SENSORS_ACTIVATE(light_sensor); /* lab3a */
  //SENSORS_ACTIVATE(sht11_sensor); /* lab3a */
  etimer_set(&periodic, SEND_INTERVAL);
  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
    
    if(etimer_expired(&periodic)) {
      lights = get_light(); // lab3a plus zoul
      temperature = get_temp(); // lab3a plus zoul
      etimer_reset(&periodic);
      ctimer_set(&backoff_timer, SEND_TIME, send_packet, NULL);
      counter++;
           
/*#if WITH_COMPOWER
      if (print == 0) {
	powertrace_print("#P");
      }
      if (++print == 3) {
	print = 0;
      }
#endif */

    } else if(ev == sensors_event) {
      if(data == &button_sensor) {
        if(button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_LEVEL) ==
           BUTTON_SENSOR_PRESSED_LEVEL) {
          printf("Button pressed\n");
          packetbuf_copyfrom(&counter, sizeof(counter));
          //broadcast_send(&bc);
        } else {
          printf("...and released!\n");
        }
      }

    } else if(ev == serial_line_event_message) {
      leds_toggle(LEDS_SERIAL_IN);
    } else if(ev == button_press_duration_exceeded) {
      printf("Button pressed for %d ticks [%u events]\n",
             (*((uint8_t *)data) * BUTTON_PRESS_EVENT_INTERVAL),
             button_sensor.value(BUTTON_SENSOR_VALUE_TYPE_PRESS_DURATION));
    }   
  
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
