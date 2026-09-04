#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <rmw_zenoh_pico/zephyr_simple_pub.h>

#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#ifdef CONFIG_WIFI
#include "wifi.h"
#include <zephyr/net/net_mgmt.h>
#endif

LOG_MODULE_REGISTER(zenoh_pub, LOG_LEVEL_INF);

#define KEYEXPR "demo/example/zenoh-pico-pub"
#define VALUE "Hello from Zephyr!"

#ifdef CONFIG_WIFI
/* IPv4 address event-based logger */
static struct net_mgmt_event_callback ipv4_cb;

static void ipv4_event_handler(struct net_mgmt_event_callback *cb, uint64_t event,
                               struct net_if *iface) {
    ARG_UNUSED(cb);
    if (event != NET_EVENT_IPV4_ADDR_ADD) {
        return;
    }
    struct in_addr *addr = net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED);
    if (addr && addr->s_addr != 0) {
        char ipbuf[NET_IPV4_ADDR_LEN];
        net_addr_ntop(AF_INET, addr, ipbuf, sizeof(ipbuf));
        LOG_INF("WiFi IPv4 address: %s", ipbuf);
    }
}
#endif

void main(void)
{
    LOG_INF("Zenoh-pico publisher starting on Zephyr");
    LOG_INF("Build: %s", CONFIG_BOARD);
    
#ifdef CONFIG_WIFI
    /* Initialize and connect WiFi before starting Zenoh */
    (void) wifi_init(NULL);
    if (connect_to_wifi() < 0) {
        LOG_ERR("WiFi connect request failed");
        return;
    }
    if (wait_for_wifi_connection() < 0) {
        LOG_ERR("WiFi did not connect in time");
        return;
    }

    /* Log IPv4 once DHCP completes using a net_mgmt event callback. */
    net_mgmt_init_event_callback(&ipv4_cb, ipv4_event_handler, NET_EVENT_IPV4_ADDR_ADD);
    net_mgmt_add_event_callback(&ipv4_cb);
    /* If DHCP already completed, log immediately. */
    {
        struct in_addr *addr =
            net_if_ipv4_get_global_addr(net_if_get_default(), NET_ADDR_PREFERRED);
        if (addr && addr->s_addr != 0) {
            char ipbuf[NET_IPV4_ADDR_LEN];
            net_addr_ntop(AF_INET, addr, ipbuf, sizeof(ipbuf));
            LOG_INF("WiFi IPv4 address: %s", ipbuf);
        }
    }
#else
    /* For native_sim, just log the configured IP */
    LOG_INF("Running on native_sim - network interface ready");
    struct net_if *iface = net_if_get_default();
    if (iface) {
        struct in_addr *addr = net_if_ipv4_get_global_addr(iface, NET_ADDR_PREFERRED);
        if (addr && addr->s_addr != 0) {
            char ipbuf[NET_IPV4_ADDR_LEN];
            net_addr_ntop(AF_INET, addr, ipbuf, sizeof(ipbuf));
            LOG_INF("IPv4 address: %s", ipbuf);
        }
    }
    // Give network stack time to initialize (especially for native_sim offloaded sockets)
    k_sleep(K_MSEC(100));
#endif
    
    bool client_mode = false;
#ifdef CONFIG_ZENOH_PICO_CLIENT_MODE
    client_mode = true;
#endif
    const char *connect_endpoint = NULL;
#ifdef CONFIG_ZENOH_PICO_CLIENT_MODE
    char endpoint[64];
    (void)snprintf(endpoint, sizeof(endpoint), "tcp/%s:7447", ZENOH_ROUTER_IP);
    connect_endpoint = endpoint;
#endif

    int rc = rzp_zephyr_run_publisher(KEYEXPR, VALUE, client_mode, connect_endpoint);
    if (rc < 0) {
        LOG_ERR("rmw_zenoh_pico publisher failed: %d", rc);
        return;
    }
#ifdef CONFIG_WIFI
    wifi_disconnect();
#endif
}
