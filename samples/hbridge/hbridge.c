/*
 * Copyright 2025 NXP
 *
 * SDPX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include "hbridge.h"

LOG_MODULE_REGISTER(hbridge);

static int motor_init(struct nxp_hbridge *hbridge, int *gpios)
{
	int ret;

	ret = gpio_pin_configure(hbridge->gpio_dev, gpios[0], GPIO_OUTPUT_LOW);
	if (ret) {
		LOG_ERR("failed to configure GPIO %d: %d", gpios[0], ret);
		return ret;
	}

	ret = gpio_pin_configure(hbridge->gpio_dev, gpios[1], GPIO_OUTPUT_LOW);
	if (ret) {
		LOG_ERR("failed to configure GPIO %d: %d", gpios[1], ret);
		return ret;
	}

	return 0;
}

static int motors_init(struct nxp_hbridge *hbridge)
{
	int ret;

	ret = motor_init(hbridge, hbridge->lgpios);
	if (ret) {
		LOG_ERR("failed to initialize left motor: %d", ret);
		return ret;
	}

	ret = motor_init(hbridge, hbridge->rgpios);
	if (ret) {
		LOG_ERR("failed to initialize right motor: %d", ret);
		return ret;
	}

	return 0;
}

static int motor_set_direction(struct nxp_hbridge *hbridge, int *gpios, int direction)
{
	int ret;
	int gpio0_level, gpio1_level;

	/* sanity checks */
	if (!hbridge || !hbridge->gpio_dev) {
		return -EINVAL;
	}

	switch (direction) {
	case NXP_HBRIDGE_DIRECTION_OFF:
		gpio0_level = 0;
		gpio1_level = 0;
		break;
	case NXP_HBRIDGE_DIRECTION_FORWARD:
		gpio0_level = 1;
		gpio1_level = 0;
		break;
	case NXP_HBRIDGE_DIRECTION_BACKWARDS:
		gpio0_level = 0;
		gpio1_level = 1;
		break;
	default:
		LOG_ERR("invalid direction: %d", direction);
		return -EINVAL;
	}

	ret = gpio_pin_set(hbridge->gpio_dev, gpios[0], gpio0_level);
	if (ret) {
		LOG_ERR("failed to set GPIO 0 level to %d: %d", gpio0_level, ret);
		return ret;
	}

	ret = gpio_pin_set(hbridge->gpio_dev, gpios[1], gpio1_level);
	if (ret) {
		LOG_ERR("failed to set GPIO 1 level to %d: %d", gpio1_level, ret);
		return ret;
	}

	LOG_DBG("GPIO %d: %d, GPIO %d: %d",
		gpios[0], gpio0_level, gpios[1], gpio1_level);

	return 0;
}

static int invert_direction(int direction)
{
	switch (direction) {
	case NXP_HBRIDGE_DIRECTION_FORWARD:
		return NXP_HBRIDGE_DIRECTION_BACKWARDS;
	case NXP_HBRIDGE_DIRECTION_BACKWARDS:
		return NXP_HBRIDGE_DIRECTION_FORWARD;
	default:
		return direction;
	}
}

int nxp_hbridge_set_direction(struct nxp_hbridge *hbridge, int direction)
{
	int ret, ldirection, rdirection;

	ldirection = direction;
	rdirection = direction;

	if (hbridge->lflags & NXP_HBRIDGE_MOTOR_INVERT) {
		ldirection = invert_direction(direction);
	}

	if (hbridge->rflags & NXP_HBRIDGE_MOTOR_INVERT) {
		rdirection = invert_direction(direction);
	}

	ret = motor_set_direction(hbridge, hbridge->lgpios, ldirection);
	if (ret) {
		LOG_ERR("failed to set left motor direction to %d: %d",
			direction, ret);
		return ret;
	}

	ret = motor_set_direction(hbridge, hbridge->rgpios, rdirection);
	if (ret) {
		LOG_ERR("failed to set right motor direction to %d: %d",
			direction, ret);
		return ret;
	}

	return 0;
}

int nxp_hbridge_init(struct nxp_hbridge *hbridge)
{
	int ret;

	/* sanity checks */
	if (!hbridge || !hbridge->gpio_dev || !hbridge->pwm_dev) {
		return -EINVAL;
	}

	/* ENA and ENB will initially be fed 3.3V */
	ret = pwm_set(hbridge->pwm_dev, hbridge->lchan, hbridge->period,
		      hbridge->period, PWM_POLARITY_NORMAL);
	if (ret < 0) {
		LOG_ERR("failed to configure left PWM channel: %d", ret);
		return ret;
	}

	ret = pwm_set(hbridge->pwm_dev, hbridge->rchan, hbridge->period,
		      hbridge->period, PWM_POLARITY_NORMAL);
	if (ret < 0) {
		LOG_ERR("failed to configure right PWM channel: %d", ret);
		return ret;
	}

	/* motors will initially be stopped */
	ret = motors_init(hbridge);
	if (ret) {
		LOG_ERR("failed to initialize motors: %d", ret);
		return ret;
	}

	return 0;
}

int nxp_hbridge_set_speed(struct nxp_hbridge *hbridge, uint32_t speed)
{
	int ret;
	uint32_t duty_cycle;

	/* sanity checks */
	if (!hbridge || !hbridge->pwm_dev) {
		return -EINVAL;
	}

	if (speed > NXP_HBRIDGE_MAX_SPEED) {
		LOG_ERR("exceeded maximum speed: %d (actual) vs %d (max)",
			speed, NXP_HBRIDGE_MAX_SPEED);
		return -EINVAL;
	}

	duty_cycle = (hbridge->period * speed) / NXP_HBRIDGE_MAX_SPEED;

	ret = pwm_set(hbridge->pwm_dev, hbridge->lchan, hbridge->period,
		      duty_cycle, PWM_POLARITY_NORMAL);
	if (ret) {
		LOG_ERR("failed to configure left PWM channel: %d", ret);
		return ret;
	}

	ret = pwm_set(hbridge->pwm_dev, hbridge->rchan, hbridge->period,
		      duty_cycle, PWM_POLARITY_NORMAL);
	if (ret < 0) {
		LOG_ERR("failed to configure right PWM channel: %d", ret);
		return ret;
	}

	LOG_INF("set speed to %d", speed);

	return 0;
}
