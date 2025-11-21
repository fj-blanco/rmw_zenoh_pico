#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zenoh-pico.h>

#include <rmw_zenoh_pico/zephyr_simple_pub.h>

LOG_MODULE_REGISTER(rzp_pub, LOG_LEVEL_INF);

int rzp_zephyr_run_publisher(const char *keyexpr,
                             const char *initial_value,
                             bool client_mode,
                             const char *connect_endpoint)
{
    if (keyexpr == NULL || keyexpr[0] == '\0') {
        LOG_ERR("Invalid key expression");
        return -1;
    }

    z_owned_config_t config;
    z_config_default(&config);

    if (client_mode) {
        zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, "client");
        if (connect_endpoint && connect_endpoint[0] != '\0') {
            zp_config_insert(z_loan_mut(config), Z_CONFIG_CONNECT_KEY, connect_endpoint);
            LOG_INF("Client mode: %s", connect_endpoint);
        } else {
            LOG_WRN("Client mode requested but no endpoint provided; using defaults");
        }
    } else {
        zp_config_insert(z_loan_mut(config), Z_CONFIG_MODE_KEY, "peer");
        LOG_INF("Peer mode (multicast discovery)");
    }

    /* Open zenoh session */
    z_owned_session_t session;
    int8_t r = z_open(&session, z_move(config), NULL);
    if (r < 0) {
        LOG_ERR("z_open failed (err=%d)", r);
        return -2;
    }
    LOG_INF("Zenoh session opened (result=%d)", r);

    z_view_keyexpr_t ke;
    z_view_keyexpr_from_str(&ke, keyexpr);
    z_owned_publisher_t pub;
    if (z_declare_publisher(z_loan(session), &pub, z_loan(ke), NULL) < 0) {
        LOG_ERR("declare publisher failed");
        z_drop(z_move(session));
        return -3;
    }

    int i = 0;
    char buf[96];
    for (;;) {
        k_sleep(K_SECONDS(1));
        if (initial_value) {
            (void)snprintf(buf, sizeof(buf), "%s [%d]", initial_value, i++);
        } else {
            (void)snprintf(buf, sizeof(buf), "Hello from rmw_zenoh_pico [%d]", i++);
        }

        z_publisher_put_options_t options;
        z_publisher_put_options_default(&options);
        z_owned_bytes_t payload;
        z_bytes_from_static_str(&payload, buf);
        if (z_publisher_put(z_loan(pub), z_move(payload), &options) < 0) {
            LOG_WRN("publish failed");
        } else {
            LOG_INF("Published: %s", buf);
        }
    }

    // Unreachable
    // z_drop(z_move(pub));
    // z_drop(z_move(session));
    // return 0;
}
