/* Minimal Wi-Fi station API for the Zephyr examples. */

#pragma once

#include <zephyr/kernel.h>

int wifi_init(void *unused);
int connect_to_wifi(void);
int wait_for_wifi_connection(void);
int wifi_disconnect(void);
