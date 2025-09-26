/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file pixy2_command.h
 * @brief Pixy2 command API
 *
 * This file offers some utility macros and implementation of various
 * commands documented in [1].
 *
 * [1]: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:pixy2_full_api
 */

#ifndef _PIXY2_COMMAND_H_
#define _PIXY2_COMMAND_H_

#include "pixy2_protocol.h"

/**
 * @brief Prepare a Pixy2 request message
 *
 * Use this to create a @ref pixy2_message structure
 * required for sending a request to the Pixy2 camera.
 *
 * @param t request type - one of @ref Pixy2RequestTypes
 * @param l request payload length
 * @param p pointer to payload data
 * @param c boolean indicating if checksum should be performed or not
 *
 * @retval newly created @ref pixy2_message structure
 */
#define PIXY2_REQUEST(t, l, p, c)		\
{						\
	.hdr.sync0 = PIXY2_REQUEST_SYNC0(c),	\
	.hdr.sync1 = PIXY2_REQUEST_SYNC1(c),	\
	.hdr.type = t,				\
	.hdr.len = l,				\
	.checksum = c,				\
	.payload = (uint8_t *)p,		\
}

/**
 * @brief Prepare a Pixy2 reply message
 *
 * Use this to create a @ref pixy2_message structure
 * required for receiving a reply from the Pixy2 camera.
 *
 * @param l expected reply payload length
 * @param p address to use for storing the payload data
 * @param c boolean indicating if checksum should be performed or not
 *
 * @retval newly created @ref pixy2_message structure
 */
#define PIXY2_REPLY(l, p, c)			\
{						\
	.hdr.sync0 = PIXY2_REPLY_SYNC0,		\
	.hdr.sync1 = PIXY2_REPLY_SYNC1,		\
	.hdr.len = l,				\
	.hdr.checksum = c,			\
	.payload = (uint8_t *)p,		\
}

/**
 * @struct pixy2_led
 * @brief Pixy2 setLED() command arguments
 */
struct pixy2_led {
	/** value for red LED (0 - 255) */
	uint8_t red;
	/** value for blue LED (0 - 255) */
	uint8_t blue;
	/** value for green LED (0 - 255) */
	uint8_t green;
} __packed;

/**
 * @struct pixy2_lamp
 * @brief Pixy2 setLamp() command arguments
 */
struct pixy2_lamp {
	/** true if the upper two LEDs should be powered on, false otherwise */
	uint8_t upper;
	/** true if the bottom RGB LED should be powered on, false otherwise */
	uint8_t lower;
} __packed;

/**
 * @brief Print firmware and hardware information
 *
 * Use this to print the firmware and hardware version of the Pixy2 camera.
 *
 * @param t pointer to the generic transport layer data
 *
 * @retval 0 if success
 * @retval negative errno code if error
 */
int pixy2_print_version(struct pixy2_transport *t);

/**
 * @brief Send the setLED command
 *
 * Use this to configure the RGB LED (via the setLED() command).
 *
 * @param t pointer to the generic transport layer data
 * @param led pointer to the setLED command arguments
 *
 * @retval 0 if success
 * @retval negative errno code if error
 */
int pixy2_set_led(struct pixy2_transport *t, struct pixy2_led *led);

/**
 * @brief Send the setLamp command
 *
 * Use this to toggle the upper/lower LEDs (via the setLamp() command).
 *
 * @param t pointer to the generic transport layer data
 * @param lamp pointer to the setLamp command arguments
 *
 * @retval 0 if success
 * @retval negative errno code if error
 */
int pixy2_set_lamp(struct pixy2_transport *t, struct pixy2_lamp *lamp);

#endif /* _PIXY2_COMMAND_H_ */
