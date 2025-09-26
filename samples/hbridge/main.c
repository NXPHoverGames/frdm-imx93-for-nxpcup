/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Demonstrate the functionality of the L298 H-BRIDGE module.
 */

#include <zephyr/logging/log.h>

#include "hbridge.h"

LOG_MODULE_REGISTER(main);

/* PWM period in nanoseconds */
#define HBRIDGE_PERIOD_NS		20000000

/* EXP_GPIO_IO02 connected to IN1 pin */
#define HBRIDGE_IN1_GPIO		2
/* EXP_GPIO_IO03 connected to IN2 pin */
#define HBRIDGE_IN2_GPIO		3
/* EXP_GPIO_IO17 connected to IN3 pin */
#define HBRIDGE_IN3_GPIO		17
/* EXP_GPIO_IO27 connected to IN4 pin */
#define HBRIDGE_IN4_GPIO		27
/* TPM3.CH1 connected to ENA pin via EXP_GPIO_IO20 */
#define HBRIDGE_ENA_PWM_CHANNEL		1
/* TPM3.CH2 connected to ENB pin via EXP_GPIO_IO12 */
#define HBRIDGE_ENB_PWM_CHANNEL		2

#define HBRIDGE_SPEED_STEP		10

static struct nxp_hbridge hbridge = {
	.gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio2)),
	.pwm_dev = DEVICE_DT_GET(DT_NODELABEL(tpm3)),
	.period = HBRIDGE_PERIOD_NS,
	.lgpios = { HBRIDGE_IN1_GPIO, HBRIDGE_IN2_GPIO },
	.rgpios = { HBRIDGE_IN3_GPIO, HBRIDGE_IN4_GPIO },
	.lchan = HBRIDGE_ENA_PWM_CHANNEL,
	.rchan = HBRIDGE_ENB_PWM_CHANNEL,
	.lflags = NXP_HBRIDGE_MOTOR_INVERT,
};

#ifdef CONFIG_NXPCUP_DIRECTION_SAMPLE
/* go into given direction for given miliseconds */
static int do_direction_for_ms(struct nxp_hbridge *hbridge, int direction, int ms)
{
	int ret;

	ret = nxp_hbridge_set_direction(hbridge, direction);
	if (ret) {
		LOG_ERR("failed to set direction to %d: %d", direction, ret);
		return ret;
	}

	k_sleep(K_MSEC(ms));

	return 0;
}
static int do_sample(struct nxp_hbridge *hbridge)
{
	int ret;

	ret = nxp_hbridge_init(hbridge);
	if (ret) {
		LOG_ERR("failed to initialize hbridge data: %d", ret);
		return ret;
	}

	/* set speed to 10% */
	ret = nxp_hbridge_set_speed(hbridge, 10);
	if (ret) {
		LOG_ERR("failed to set speed to 10: %d", ret);
		return ret;
	}

	while (true) {
		/* go forward for 5 seconds */
		ret = do_direction_for_ms(hbridge,
					  NXP_HBRIDGE_DIRECTION_FORWARD, 5000);
		if (ret) {
			LOG_ERR("failed to go forward: %d", ret);
			return ret;
		}

		/* stop for 1 second */
		ret = do_direction_for_ms(hbridge,
					  NXP_HBRIDGE_DIRECTION_OFF, 1000);
		if (ret) {
			LOG_ERR("failed to stop the car: %d", ret);
			return ret;
		}

		/* go backwards for 5 seconds */
		ret = do_direction_for_ms(hbridge,
					  NXP_HBRIDGE_DIRECTION_BACKWARDS,
					  5000);
		if (ret) {
			LOG_ERR("failed to go backwards: %d", ret);
			return ret;
		}

		/* stop for 1 second */
		ret = do_direction_for_ms(hbridge,
					  NXP_HBRIDGE_DIRECTION_OFF, 1000);
		if (ret) {
			LOG_ERR("failed to stop the car: %d", ret);
			return ret;
		}
	}

	LOG_INF("sample executed successfully!");

	return 0;
}
#else
static int do_sample(struct nxp_hbridge *hbridge)
{
	int ret;
	uint32_t speed;

	speed = 0;

	ret = nxp_hbridge_init(hbridge);
	if (ret) {
		LOG_ERR("failed to initialize hbridge data: %d", ret);
		return ret;
	}

	ret = nxp_hbridge_set_direction(hbridge,
					NXP_HBRIDGE_DIRECTION_FORWARD);
	if (ret) {
		LOG_ERR("failed to set direction to %d: %d",
			NXP_HBRIDGE_DIRECTION_FORWARD, ret);
		return ret;
	}

	while (true) {
		ret = nxp_hbridge_set_speed(hbridge, speed);
		if (ret) {
			LOG_ERR("failed to set speed to %d: %d", speed, ret);
			return ret;
		}

		if (speed == NXP_HBRIDGE_MAX_SPEED) {
			speed = 0;
		} else {
			speed += HBRIDGE_SPEED_STEP;
		}

		k_sleep(K_MSEC(1000));
	}

	LOG_INF("sample executed successfully!");

	return 0;
}
#endif /* CONFIG_NXPCUP_DIRECTION_SAMPLE */

int main(void)
{
	return do_sample(&hbridge);
}
