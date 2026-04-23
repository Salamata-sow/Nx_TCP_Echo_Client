/* USER CODE BEGIN Header */

/**

  ******************************************************************************

  * @file    app_netxduo.c

  * @brief   NetXDuo applicative file

  ******************************************************************************

  */

/* USER CODE END Header */



/* Includes ------------------------------------------------------------------*/

#include "app_netxduo.h"

#include "nx_api.h"

#include "tx_api.h"

#include <stdio.h>



/* Private includes ----------------------------------------------------------*/

#include "nxd_dhcp_client.h"

#include "nxd_dns.h"



/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */



/* Private define ------------------------------------------------------------*/

/* USER CODE BEGIN PD */

/* USER CODE END PD */



/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */



/* Private variables ---------------------------------------------------------*/

TX_THREAD      NxAppThread;

NX_PACKET_POOL NxAppPool;

NX_IP          NetXDuoEthIpInstance;

TX_SEMAPHORE   DHCPSemaphore;

NX_DHCP        DHCPClient;



/* USER CODE BEGIN PV */

TX_THREAD AppTCPThread;

TX_THREAD AppLinkThread;



NX_TCP_SOCKET TCPSocket;

NX_DNS        DnsClient;



ULONG IpAddress;

ULONG NetMask;

/* USER CODE END PV */



/* Private function prototypes -----------------------------------------------*/

static VOID nx_app_thread_entry(ULONG thread_input);

static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);



/* USER CODE BEGIN PFP */

static VOID App_TCP_Thread_Entry(ULONG thread_input);

static VOID App_Link_Thread_Entry(ULONG thread_input);

static VOID App_Ping_8_8_8_8(VOID);

static UINT App_DNS_Client_Init(VOID);

static VOID App_DNS_Resolve_And_Ping_Google(VOID);

/* USER CODE END PFP */



UINT MX_NetXDuo_Init(VOID *memory_ptr)

{

  UINT ret = NX_SUCCESS;

  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *)memory_ptr;

  CHAR *pointer;



  printf("Nx_TCP_Echo_Client application started..\r\n");



  nx_system_initialize();



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE,

                              pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)

  {

    return NX_POOL_ERROR;

  }



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance",

                     NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK,

                     &NxAppPool, nx_stm32_eth_driver,

                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)

  {

    return NX_NOT_SUCCESSFUL;

  }



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)

  {

    return NX_NOT_SUCCESSFUL;

  }



  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)

  {

    return NX_NOT_SUCCESSFUL;

  }



  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)

  {

    return NX_NOT_SUCCESSFUL;

  }



  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)

  {

    return NX_NOT_SUCCESSFUL;

  }



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = tx_thread_create(&AppTCPThread, "App TCP Thread", App_TCP_Thread_Entry, 0,

                         pointer, NX_APP_THREAD_STACK_SIZE,

                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY,

                         TX_NO_TIME_SLICE, TX_DONT_START);

  if (ret != TX_SUCCESS)

  {

    return TX_THREAD_ERROR;

  }



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry, 0,

                         pointer, NX_APP_THREAD_STACK_SIZE,

                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY,

                         TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)

  {

    return TX_THREAD_ERROR;

  }



  ret = nx_dhcp_create(&DHCPClient, &NetXDuoEthIpInstance, "DHCP Client");

  if (ret != NX_SUCCESS)

  {

    return NX_DHCP_ERROR;

  }



  ret = tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);

  if (ret != TX_SUCCESS)

  {

    return NX_DHCP_ERROR;

  }



  if (tx_byte_allocate(byte_pool, (VOID **)&pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)

  {

    return TX_POOL_ERROR;

  }



  ret = tx_thread_create(&AppLinkThread, "App Link Thread", App_Link_Thread_Entry, 0,

                         pointer, NX_APP_THREAD_STACK_SIZE,

                         LINK_PRIORITY, LINK_PRIORITY,

                         TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)

  {

    return TX_THREAD_ERROR;

  }



  return ret;

}



static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)

{

  NX_PARAMETER_NOT_USED(ip_instance);

  NX_PARAMETER_NOT_USED(ptr);



  if (nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask) != NX_SUCCESS)

  {

    Error_Handler();

  }



  if (IpAddress != NULL_ADDRESS)

  {

    tx_semaphore_put(&DHCPSemaphore);

  }

}



static VOID App_Ping_8_8_8_8(VOID)

{

  UINT status;

  ULONG target_ip = IP_ADDRESS(8, 8, 8, 8);

  NX_PACKET *reply_packet = NX_NULL;

  CHAR ping_data[] = "PING STM32";



  printf("Starting ICMP ping to 8.8.8.8 ...\r\n");



  status = nx_icmp_ping(&NetXDuoEthIpInstance,

                        target_ip,

                        ping_data,

                        sizeof(ping_data) - 1,

                        &reply_packet,

                        5 * NX_IP_PERIODIC_RATE);



  if (status == NX_SUCCESS)

  {

    printf("Ping to 8.8.8.8: SUCCESS\r\n");

    if (reply_packet != NX_NULL)

    {

      nx_packet_release(reply_packet);

    }

  }

  else

  {

    printf("Ping to 8.8.8.8: FAILED, status = 0x%X\r\n", status);

  }

}



static UINT App_DNS_Client_Init(VOID)

{

  UINT status;

  NXD_ADDRESS dns_server_address;



  dns_server_address.nxd_ip_version = NX_IP_VERSION_V4;

  dns_server_address.nxd_ip_address.v4 = IP_ADDRESS(8, 8, 8, 8);



  status = nx_dns_create(&DnsClient, &NetXDuoEthIpInstance, (UCHAR *)"DNS Client");

  if (status != NX_SUCCESS)

  {

    printf("DNS create failed: 0x%X\r\n", status);

    return status;

  }



  status = nxd_dns_server_add(&DnsClient, &dns_server_address);

  if (status != NX_SUCCESS)

  {

    printf("DNS server add failed: 0x%X\r\n", status);

    return status;

  }



  printf("DNS client initialized. DNS server = 8.8.8.8\r\n");

  return NX_SUCCESS;

}



static VOID App_DNS_Resolve_And_Ping_Google(VOID)

{

  UINT status;

  NX_PACKET *reply_packet = NX_NULL;

  CHAR ping_data[] = "PING GOOGLE";

  NXD_ADDRESS google_address;



  google_address.nxd_ip_version = NX_IP_VERSION_V4;

  google_address.nxd_ip_address.v4 = 0;



  printf("Resolving google.com ...\r\n");



  status = nxd_dns_host_by_name_get(&DnsClient,

                                    (UCHAR *)"google.com",

                                    &google_address,

                                    5 * NX_IP_PERIODIC_RATE,

                                    NX_IP_VERSION_V4);



  if (status != NX_SUCCESS)

  {

    printf("DNS resolve failed for google.com, status = 0x%X\r\n", status);

    return;

  }



  printf("google.com resolved to: %lu.%lu.%lu.%lu\r\n",

         (google_address.nxd_ip_address.v4 >> 24) & 0xFF,

         (google_address.nxd_ip_address.v4 >> 16) & 0xFF,

         (google_address.nxd_ip_address.v4 >> 8) & 0xFF,

         google_address.nxd_ip_address.v4 & 0xFF);



  printf("Starting ICMP ping to google.com ...\r\n");



  status = nx_icmp_ping(&NetXDuoEthIpInstance,

                        google_address.nxd_ip_address.v4,

                        ping_data,

                        sizeof(ping_data) - 1,

                        &reply_packet,

                        5 * NX_IP_PERIODIC_RATE);



  if (status == NX_SUCCESS)

  {

    printf("Ping to google.com: SUCCESS\r\n");

    if (reply_packet != NX_NULL)

    {

      nx_packet_release(reply_packet);

    }

  }

  else

  {

    printf("Ping to google.com: FAILED, status = 0x%X\r\n", status);

  }

}



static VOID nx_app_thread_entry(ULONG thread_input)

{

  UINT ret = NX_SUCCESS;



  NX_PARAMETER_NOT_USED(thread_input);



  ret = nx_ip_address_change_notify(&NetXDuoEthIpInstance,

                                    ip_address_change_notify_callback,

                                    NULL);

  if (ret != NX_SUCCESS)

  {

    Error_Handler();

  }



  ret = nx_dhcp_start(&DHCPClient);

  if (ret != NX_SUCCESS)

  {

    Error_Handler();

  }



  printf("Looking for DHCP server ..\r\n");



  if (tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)

  {

    Error_Handler();

  }



  PRINT_IP_ADDRESS(IpAddress);



  App_Ping_8_8_8_8();



  if (App_DNS_Client_Init() != NX_SUCCESS)

  {

    Error_Handler();

  }



  App_DNS_Resolve_And_Ping_Google();



  tx_thread_resume(&AppTCPThread);

  tx_thread_relinquish();

}



static VOID App_TCP_Thread_Entry(ULONG thread_input)

{

  NX_PARAMETER_NOT_USED(thread_input);



  while (1)

  {

    tx_thread_sleep(NX_IP_PERIODIC_RATE);

  }

}



static VOID App_Link_Thread_Entry(ULONG thread_input)

{

  ULONG actual_status;

  UINT linkdown = 0, status;



  NX_PARAMETER_NOT_USED(thread_input);



  while (1)

  {

    status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0,

                                          NX_IP_LINK_ENABLED,

                                          &actual_status, 10);



    if (status == NX_SUCCESS)

    {

      if (linkdown == 1)

      {

        linkdown = 0;



        printf("The network cable is connected.\r\n");



        nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_ENABLE, &actual_status);



        status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0,

                                              NX_IP_ADDRESS_RESOLVED,

                                              &actual_status, 10);

        if (status == NX_SUCCESS)

        {

          nx_dhcp_stop(&DHCPClient);

          nx_dhcp_reinitialize(&DHCPClient);

          nx_dhcp_start(&DHCPClient);



          if (tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)

          {

            Error_Handler();

          }



          PRINT_IP_ADDRESS(IpAddress);

          App_Ping_8_8_8_8();



          if (App_DNS_Client_Init() == NX_SUCCESS)

          {

            App_DNS_Resolve_And_Ping_Google();

          }

        }

        else

        {

          nx_dhcp_client_update_time_remaining(&DHCPClient, 0);

        }

      }

    }

    else

    {

      if (linkdown == 0)

      {

        linkdown = 1;

        printf("The network cable is not connected.\r\n");

        nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_DISABLE, &actual_status);

      }

    }



    tx_thread_sleep(NX_APP_CABLE_CONNECTION_CHECK_PERIOD);

  }

}
