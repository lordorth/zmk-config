/*
 * Auto-manage underglow: off on battery, rainbow when USB power is present.
 */
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(rgb_power_underglow, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/rgb_underglow.h>
#include <zmk/usb.h>

#define EFFECT_SPECTRUM 2 /* matches UNDERGLOW_EFFECT_SPECTRUM in rgb_underglow.c */

static void apply_power_policy(void) {
    const bool usb = zmk_usb_is_powered();

    if (usb) {
        (void)zmk_rgb_underglow_select_effect(EFFECT_SPECTRUM);
        (void)zmk_rgb_underglow_on();
    } else {
        (void)zmk_rgb_underglow_off();
    }
}

static int usb_power_listener(const zmk_event_t *eh) {
    if (as_zmk_usb_conn_state_changed(eh)) {
        apply_power_policy();
        return ZMK_EV_EVENT_BUBBLE;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(rgb_power_underglow, usb_power_listener);
ZMK_SUBSCRIPTION(rgb_power_underglow, zmk_usb_conn_state_changed);

static int rgb_power_init(const struct device *dev) {
    ARG_UNUSED(dev);
    apply_power_policy();
    return 0;
}

SYS_INIT(rgb_power_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY + 2);
