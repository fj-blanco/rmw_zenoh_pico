/*
 * Copyright(C) 2024 eSOL Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#ifdef CONFIG_WIFI
#include <zephyr/net/net_mgmt.h>
#endif
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/string.h>

#ifdef CONFIG_WIFI
#include "wifi.h"
#endif

LOG_MODULE_REGISTER(z_talker, LOG_LEVEL_INF);

#define ARRAY_LEN 200

#define RCCHECK(fn) {                                           \
    rcl_ret_t temp_rc = fn;                                     \
    if((temp_rc != RCL_RET_OK)) {                               \
      LOG_ERR("Failed status on line %d: %d. Aborting.",        \
             __LINE__,(int)temp_rc);                            \
      return 1;                                                 \
    }                                                           \
  }

#define RCSOFTCHECK(fn) {                                       \
    rcl_ret_t temp_rc = fn;                                     \
    if((temp_rc != RCL_RET_OK)) {                               \
      LOG_WRN("Failed status on line %d: %d. Continuing.",      \
             __LINE__,(int)temp_rc);                            \
    }                                                           \
  }

// ROS2 rcl/rclc common data
rclc_executor_t executor;
rclc_support_t support;
rcl_allocator_t allocator;
rcl_node_t node;
rcl_timer_t timer;
rcl_publisher_t publisher;

// ROS message type
std_msgs__msg__String msg;

int counter = 0;

void timer_callback(rcl_timer_t * timer, int64_t last_call_time)
{
  (void) last_call_time;
  if (timer != NULL) {
    snprintf(msg.data.data, ARRAY_LEN, "Hello World from Zephyr: %d", counter++);
    msg.data.size = strlen(msg.data.data);
    RCSOFTCHECK(rcl_publish(&publisher, &msg, NULL));
    LOG_INF("Published: \"%s\"", msg.data.data);
  }
}

#ifdef CONFIG_WIFI
static void print_dhcpv4_address(struct net_if *iface)
{
	char ipbuf[NET_IPV4_ADDR_LEN];
	struct net_if_config *cfg = net_if_get_config(iface);

	if (!cfg || !cfg->dhcpv4.state ||
	    cfg->ip.ipv4->unicast[0].addr_state != NET_ADDR_PREFERRED) {
		return;
	}

	LOG_INF("WiFi IPv4 address: %s",
		net_addr_ntop(AF_INET,
			      &cfg->ip.ipv4->unicast[0].address.in_addr,
			      ipbuf, sizeof(ipbuf)));
}
#endif

int main(void)
{
  LOG_INF("RMW Zenoh-pico Zephyr talker starting");
  LOG_INF("Build: %s", CONFIG_BOARD);

#ifdef CONFIG_WIFI
  /* Initialize and connect WiFi before starting ROS2 */
  (void) wifi_init(NULL);
  if (connect_to_wifi() < 0) {
    LOG_ERR("WiFi connect request failed");
    return -1;
  }
  if (wait_for_wifi_connection() < 0) {
    LOG_ERR("WiFi did not connect in time");
    return -1;
  }
  LOG_INF("WiFi connected");

  /* Print the assigned IPv4 address */
  struct net_if *iface = net_if_get_first_wifi();
  if (iface) {
    char ipbuf[NET_IPV4_ADDR_LEN];
    struct net_if_config *cfg = net_if_get_config(iface);

    if (cfg && cfg->dhcpv4.state &&
        cfg->ip.ipv4->unicast[0].addr_state == NET_ADDR_PREFERRED) {
      LOG_INF("WiFi IPv4 address: %s",
              net_addr_ntop(AF_INET,
                            &cfg->ip.ipv4->unicast[0].address.in_addr,
                            ipbuf, sizeof(ipbuf)));
    }
  }
#endif

  /* Give the network stack a moment to settle */
  k_sleep(K_MSEC(500));

  /* Initialize RCL */
  allocator = rcl_get_default_allocator();

  /* Create init_options and support */
  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  /* Create node */
  RCCHECK(rclc_node_init_default(&node,
                                 "zephyr_talker_node",
                                 "",
                                 &support));

  LOG_INF("RCL node created: zephyr_talker_node");

  /* Create publisher */
  RCCHECK(rclc_publisher_init_default(
            &publisher,
            &node,
            ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
            "chatter"));

  LOG_INF("Publisher created on topic: chatter");

  /* Create timer (1 second period) */
  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
            &timer,
            &support,
            RCL_MS_TO_NS(timer_timeout),
            timer_callback));

  LOG_INF("Timer created with %u ms period", timer_timeout);

  /* Allocate message buffer */
  msg.data.data = (char *) malloc(ARRAY_LEN * sizeof(char));
  msg.data.size = 0;
  msg.data.capacity = ARRAY_LEN;

  /* Create executor */
  executor = rclc_executor_get_zero_initialized_executor();
  RCCHECK(rclc_executor_init(
            &executor,
            &support.context,
            1,
            &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  LOG_INF("Executor initialized, starting spin loop...");

  /* Spin forever */
  rclc_executor_spin(&executor);

  /* Cleanup (unreachable) */
  RCCHECK(rcl_publisher_fini(&publisher, &node));
  RCCHECK(rcl_node_fini(&node));

#ifdef CONFIG_WIFI
  wifi_disconnect();
#endif

  return 0;
}
