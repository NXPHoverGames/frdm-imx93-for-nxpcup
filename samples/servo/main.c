/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Demonstrate the functionality of the MG996R servo motor.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "servo.h"

LOG_MODULE_REGISTER(main);

/* TPM3.CH0 connected to the PWM pin via EXP_GPIO_IO04 */
#define SERVO_PWM_CHANNEL	0

/* PWM signal period (in NS) */
#define SERVO_PWM_PERIOD_NS	20000000

/* how much do we increment the angle in a single step? */
#define SERVO_ANGLE_STEP	10

static struct nxp_servo servo = {
	.pwm_dev = DEVICE_DT_GET(DT_NODELABEL(tpm3)),
	.channel = SERVO_PWM_CHANNEL,
	.period = SERVO_PWM_PERIOD_NS,
};

int main(void)
{
	uint32_t angle;
	int ret;

	angle = 0;

	while (true) {
		LOG_INF("setting servo angle to %u degrees", angle);

		ret = servo_set_angle(&servo, angle);
		if (ret) {
			LOG_ERR("failed to set angle to %u: %d", angle, ret);
			return ret;
		}

		if (angle == SERVO_MAX_ANGLE) {
			angle = 0;
		} else {
			angle += SERVO_ANGLE_STEP;
		}

		/* sleep for 2 seconds otherwise the servo might change
		 * angle too quickly.
		 */
		k_sleep(K_MSEC(1000));
	}

	return 0;
}
