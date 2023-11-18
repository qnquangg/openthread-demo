#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <openthread/thread.h>
#include <zephyr/net/openthread.h>
#include <zephyr/net/socket.h>
#include <zephyr/logging/log.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

LOG_MODULE_REGISTER(blinky);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static void on_thread_state_changed(otChangedFlags flags, struct openthread_context *ot_context,
									void *user_data)
{
	if (flags & OT_CHANGED_THREAD_ROLE)
	{
		switch (otThreadGetDeviceRole(ot_context->instance))
		{
		case OT_DEVICE_ROLE_CHILD:
			LOG_INF("OT_DEVICE_ROLE_CHILD");
			break;
		case OT_DEVICE_ROLE_ROUTER:
			LOG_INF("OT_DEVICE_ROLE_ROUTER");
			break;
		case OT_DEVICE_ROLE_LEADER:
			LOG_INF("OT_DEVICE_ROLE_LEADER");
			break;
		case OT_DEVICE_ROLE_DISABLED:
			LOG_INF("OT_DEVICE_ROLE_DISABLED");
			break;
		case OT_DEVICE_ROLE_DETACHED:
			LOG_INF("OT_DEVICE_ROLE_DETACHED");
			break;
		default:
			LOG_INF("OT DEFAULT STATE");
			break;
		}
	}
}
static struct openthread_state_changed_cb ot_state_chaged_cb = {
	.state_changed_cb = on_thread_state_changed};

void config_openthread()
{
	openthread_state_changed_cb_register(openthread_get_default_context(), &ot_state_chaged_cb);
	openthread_start(openthread_get_default_context());
}

int main(void)
{
	int ret;

	if (!gpio_is_ready_dt(&led))
	{
		return 0;
	}

	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	if (ret < 0)
	{
		return 0;
	}

	config_openthread();

	while (1)
	{
		ret = gpio_pin_toggle_dt(&led);
		if (ret < 0)
		{
			return 0;
		}
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}
