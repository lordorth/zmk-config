/*
 * Keep the nice!nano blue LED steady when split peripherals are connected.
 */
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sofle_dongle_led, CONFIG_ZMK_LOG_LEVEL);

#include <zephyr/linker/sections.h>

#include <zmk/split/transport/central.h>
#include <zmk/split/transport/types.h>

#if DT_NODE_EXISTS(DT_ALIAS(led0))
static const struct device *const status_led = DEVICE_DT_GET(DT_ALIAS(led0));
#endif

static void set_led(bool connected) {
#if DT_NODE_EXISTS(DT_ALIAS(led0))
    if (!device_is_ready(status_led)) {
        LOG_WRN("status LED not ready");
        return;
    }

    if (connected) {
        led_on(status_led, 0);
    } else {
        led_off(status_led, 0);
    }
#else
    ARG_UNUSED(connected);
#endif
}

static int led_init(const struct device *dev) {
    ARG_UNUSED(dev);
    set_led(false);
    return 0;
}

SYS_INIT(led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

static const struct zmk_split_transport_central *central_transport;

static int led_status_cb(const struct zmk_split_transport_central *transport,
                         struct zmk_split_transport_status status) {
    ARG_UNUSED(transport);
    bool all_connected =
        status.connections == ZMK_SPLIT_TRANSPORT_CONNECTIONS_STATUS_ALL_CONNECTED;
    set_led(all_connected);
    return 0;
}

static int led_register_status_cb(const struct device *dev) {
    ARG_UNUSED(dev);

    // Grab the first registered central transport (BLE central for split)
    STRUCT_SECTION_FOREACH(zmk_split_transport_central, transport) {
        central_transport = transport;
        break;
    }

    if (!central_transport || !central_transport->api ||
        !central_transport->api->set_status_callback || !central_transport->api->get_status) {
        return 0;
    }

    central_transport->api->set_status_callback(led_status_cb);

    // Seed current state so LED reflects status immediately
    struct zmk_split_transport_status status = central_transport->api->get_status();
    led_status_cb(central_transport, status);

    return 0;
}

SYS_INIT(led_register_status_cb, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY + 1);
