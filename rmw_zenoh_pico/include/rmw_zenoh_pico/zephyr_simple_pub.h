/*
 * Simple Zephyr-facing publisher wrapper for rmw_zenoh_pico
 * This avoids pulling ROS 2 headers into Zephyr apps and uses zenoh-pico internally.
 */
#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Contract:
 * Inputs:
 *  - keyexpr: Zenoh key expression to publish on (e.g., "demo/example/zenoh-pico-pub")
 *  - initial_value: Optional initial payload prefix (can be NULL)
 *  - client_mode: If true, connect to router specified in connect_endpoint; otherwise peer mode
 *  - connect_endpoint: e.g., "tcp/<router-ip>:7447" (ignored for peer mode)
 * Behavior:
 *  - Initializes zenoh-pico, opens a session, declares a publisher
 *  - Publishes a message every second with an incrementing counter
 * Returns:
 *  - 0 on success (this function runs an infinite loop and never returns)
 *  - negative value on initialization failure
 */
int rzp_zephyr_run_publisher(const char *keyexpr,
                             const char *initial_value,
                             bool client_mode,
                             const char *connect_endpoint);

#ifdef __cplusplus
}
#endif
