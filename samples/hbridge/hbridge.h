/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file hbridge.h
 * @brief L298N H-BRIDGE API definition
 *
 * This file offers the API required for working with the L298N H-BRIDGE module.
 */

#ifndef _HBRIDGE_H_
#define _HBRIDGE_H_

#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/pwm.h>

/** maximum number of GPIOs per motor */
#define NXP_HBRIDGE_NUM_GPIOS 2

/** maximum allowed speed */
#define NXP_HBRIDGE_MAX_SPEED 100

/**
 * @defgroup MotorFlags
 * @brief Left/right motor flag definitions
 *
 * @{
 */

/** invert the direction of the motor before applying it */
#define NXP_HBRIDGE_MOTOR_INVERT BIT(0)

/**
 * @}
 */

/**
 * @struct nxp_hbridge
 * @brief Represents the L298N H-BRIDGE module
 */
struct nxp_hbridge {
	/** pointer to GPIO device */
	const struct device *gpio_dev;
	/** pointer to PWM device */
	const struct device *pwm_dev;
	/** left/right PWM channel period (in nanoseconds) */
	uint32_t period;
	/** left GPIO pins */
	int lgpios[NXP_HBRIDGE_NUM_GPIOS];
	/** right GPIO pins */
	int rgpios[NXP_HBRIDGE_NUM_GPIOS];
	/** left PWM channel index (ENA) */
	int lchan;
	/** right PWM channel index (ENB) */
	int rchan;
	/** left motor flags - one of \ref MotorFlags */
	uint32_t lflags;
	/** right motor flags - one of \ref MotorFlags */
	uint32_t rflags;
};

/**
 * @enum nxp_hbridge_direction
 * @brief Represents the spinning direction of a motor
 */
enum nxp_hbridge_direction {
	/** motor is stopped/braking */
	NXP_HBRIDGE_DIRECTION_OFF = 0,
	/** motor is spinning forward */
	NXP_HBRIDGE_DIRECTION_FORWARD = 1,
	/** motor is spinning backwards */
	NXP_HBRIDGE_DIRECTION_BACKWARDS = 2,
};

/**
 * @brief Initialize the L298 H-BRIDGE module.
 *
 * Put the L298 H-BRIDGE hardware into a known state. This consists of:
 *
 * - ENA and ENB pins set to logic HIGH.
 * - IN1, IN2, IN3, and IN4 pins set to logic LOW.
 *
 * @param hbridge pointer to structure representing the H-BRIDGE
 *
 * @retval 0 on success
 * @retval negative errno code if failure
 */
int nxp_hbridge_init(struct nxp_hbridge *hbridge);

/**
 * @brief Set the spinning direction of the car's motors.
 *
 * @param hbridge pointer to the structure representing the H-BRIDGE
 * @param direction one of #nxp_hbridge_direction
 *
 * @retval 0 on success
 * @retval negative errno code if failure
 */
int nxp_hbridge_set_direction(struct nxp_hbridge *hbridge, int direction);

/**
 * @brief Set the speed of the car's motors.
 *
 * Set the speed of the car's motors based on a given percentage, where:
 *
 * - 0% means no speed (i.e. car is stopped/braking)
 * - 100% means full speed
 *
 * @param hbridge pointer to the structure representing the H-BRIDGE
 * @param speed percentage to set (from 0% to 100%)
 *
 * @retval 0 on success
 * @retval negative errno code if failure
 */
int nxp_hbridge_set_speed(struct nxp_hbridge *hbridge, uint32_t speed);

#endif /* _HBRIDGE_H_ */
