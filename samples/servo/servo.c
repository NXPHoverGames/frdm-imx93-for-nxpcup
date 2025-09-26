/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include "servo.h"

LOG_MODULE_REGISTER(servo);

int servo_set_angle(struct nxp_servo *servo, uint32_t angle)
{
	uint32_t pulse_duration;
	float nsec_per_degree;
	int ret;

	/* sanity checks */
	if (!servo || !servo->pwm_dev) {
		return -EINVAL;
	}

	/* angle needs to be in [0, 180] interval */
	if (angle > SERVO_MAX_ANGLE) {
		LOG_ERR("invalid angle: %u", angle);
		return -EINVAL;
	}

	/*
	 * based on the most right and left position of the servo,
	 * find out how many nanoseconds of the pulse correspond to
	 * 1 degree.
	 */
	nsec_per_degree = (SERVO_MSEC_RIGHT - SERVO_MSEC_LEFT) / SERVO_MAX_ANGLE;

	/* the most left position corresponds to 0 degrees */
	pulse_duration = SERVO_MSEC_LEFT + (float)angle * nsec_per_degree;

	/*
	 * configure the PWM signal with the following properties:
	 *
	 * - PERIOD = SERVO_PWM_PERIOD_NS
	 * - DUTY = pulse_duration
	 * - POLARITY = NORMAL
	 * - CHANNEL = SERVO_PWM_CHANNEL
	 */
	ret = pwm_set(servo->pwm_dev, servo->channel, servo->period,
		      pulse_duration, PWM_POLARITY_NORMAL);
	if (ret < 0) {
		LOG_ERR("failed to configure PWM: %d", ret);
		return ret;
	}

	return 0;
}
