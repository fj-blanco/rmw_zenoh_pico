/*
 * Copyright(C) 2024 eSOL Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <rcl/error_handling.h>
#include <rcl/rcl.h>
#include <rclc/executor.h>
#include <rclc/rclc.h>
#include <rmw_zenoh_pico/rmw_zenoh_pico_options.h>
#include <std_msgs/msg/string.h>

#ifdef CONFIG_WIFI
#include "wifi.h"
#endif

LOG_MODULE_REGISTER(rmw_zenoh_pico_talker, LOG_LEVEL_INF);

#define MESSAGE_CAPACITY 96

static rcl_publisher_t publisher;
static rclc_support_t support;
static rcl_node_t node;
static rcl_timer_t timer;
static rclc_executor_t executor;
static std_msgs__msg__String message;
static char message_buffer[MESSAGE_CAPACITY];
static unsigned int sequence;

static bool check_rcl(rcl_ret_t result, const char * operation)
{
  if (result == RCL_RET_OK) {
    return true;
  }

  LOG_ERR("%s failed: %d (%s)", operation, (int)result, rcl_get_error_string().str);
  rcl_reset_error();
  return false;
}

static void timer_callback(rcl_timer_t * timer_handle, int64_t last_call_time)
{
  ARG_UNUSED(last_call_time);

  if (timer_handle == NULL) {
    return;
  }

  int length = snprintf(message_buffer, sizeof(message_buffer),
    "Hello from Zephyr rmw_zenoh_pico: %u", sequence++);
  if (length < 0) {
    LOG_ERR("Message formatting failed");
    return;
  }

  message.data.size = (size_t)length;
  if (rcl_publish(&publisher, &message, NULL) == RCL_RET_OK) {
    LOG_INF("Published: %s", message_buffer);
  } else {
    LOG_ERR("Publish failed");
  }
}

int main(void)
{
  rcl_allocator_t allocator = rcl_get_default_allocator();

  LOG_INF("Starting rmw_zenoh_pico talker on %s", CONFIG_BOARD);

#ifdef CONFIG_WIFI
  wifi_init(NULL);
  if (connect_to_wifi() != 0 || wait_for_wifi_connection() != 0) {
    LOG_ERR("Wi-Fi connection failed");
    return -1;
  }
#endif

  rmw_zenoh_pico_set_mode("client");
  rmw_zenoh_pico_set_unicast(ZENOH_ROUTER_IP, "7447", NULL, NULL);

  executor = rclc_executor_get_zero_initialized_executor();
  message.data.data = message_buffer;
  message.data.size = 0;
  message.data.capacity = sizeof(message_buffer);

  if (!check_rcl(rclc_support_init(&support, 0, NULL, &allocator), "rclc_support_init") ||
    !check_rcl(rclc_node_init_default(&node, "zephyr_zenoh_talker", "", &support),
      "rclc_node_init_default") ||
    !check_rcl(rclc_publisher_init_default(
      &publisher, &node, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String), "chatter"),
      "rclc_publisher_init_default") ||
    !check_rcl(rclc_timer_init_default2(
      &timer, &support, RCL_MS_TO_NS(1000), timer_callback, true),
      "rclc_timer_init_default2") ||
    !check_rcl(rclc_executor_init(&executor, &support.context, 1, &allocator),
      "rclc_executor_init") ||
    !check_rcl(rclc_executor_add_timer(&executor, &timer), "rclc_executor_add_timer"))
  {
    return -1;
  }

  LOG_INF("RMW_ZENOH_PICO_READY router=tcp/%s:7447 topic=/chatter", ZENOH_ROUTER_IP);
  rclc_executor_spin(&executor);
  return 0;
}
