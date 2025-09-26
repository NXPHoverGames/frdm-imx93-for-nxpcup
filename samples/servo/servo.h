/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file servo.h
 * @brief MG996R servo motor API definition
 *
 * This file offers the API required for working with the MG996R servo motor.
 */

#ifndef _SERVO_H_
#define _SERVO_H_

#include <zephyr/drivers/pwm.h>

/** convert miliseconds to nanoseconds */
#define MSEC_TO_NSEC(x) ((x) * NSEC_PER_MSEC)

/** 1ms PWM duty cycle means the servo is in the most left position */
#define SERVO_MSEC_LEFT         MSEC_TO_NSEC(1.0f)

/** 2ms PWM duty cycle means the servo is in the most right position */
#define SERVO_MSEC_RIGHT        MSEC_TO_NSEC(2.0f)

/** maximum angle supported by the servo */
#define SERVO_MAX_ANGLE         180

/**
 * @struct nxp_servo
 * @brief Represents the MG996R servo motor
 */
struct nxp_servo {
	/** pointer to PWM device */
	const struct device *pwm_dev;
	/** PWM period (in nanoseconds) */
	uint32_t period;
	/** PWM channel to use */
	uint32_t channel;
};

/**
 * @brief Set the angle of the servo motor
 *
 * @param servo pointer to the structure representing the MG996R servo motor
 * @param angle angle to set (between 0 and 180 degrees)
 *
 * @retval 0 on success
 * @retval negative errno code if failure
 */
int servo_set_angle(struct nxp_servo *servo, uint32_t angle);

#endif /* _SERVO_H_ */
