/*
 * Keep the nice!nano blue LED steady when split peripherals are connected.
 */
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sofle_dongle_led, CONFIG_ZMK_LOG_LEVEL);

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
