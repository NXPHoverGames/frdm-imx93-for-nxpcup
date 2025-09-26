/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include "pixy2_command.h"

LOG_MODULE_REGISTER(pixy2_command);

/* needs to be packed */
struct pixy2_version {
	uint8_t hw_minor;
	uint8_t hw_major;
	uint8_t fw_major;
	uint8_t fw_minor;
	uint16_t fw_build;
#define PIXY2_VERSION_FW_TYPE_MAX_BYTES 10
	char fw_type[PIXY2_VERSION_FW_TYPE_MAX_BYTES];
} __packed;

static int pixy2_get_version(struct pixy2_transport *t, struct pixy2_version *v)
{
	struct pixy2_message req = PIXY2_REQUEST(PIXY2_REQUEST_GET_VERSION,
						 0x0, NULL, false);
	struct pixy2_message reply = PIXY2_REPLY(sizeof(*v), v, false);

	return pixy2_protocol_transceive(t, &req, &reply);
}

int pixy2_print_version(struct pixy2_transport *t)
{
	int ret;
	struct pixy2_version version;

	ret = pixy2_get_version(t, &version);
	if (ret) {
		LOG_ERR("failed to query version: %d", ret);
		return ret;
	}

	LOG_INF("pixy2 camera HW version %d.%d, FW version %d.%d.%d, %s type",
		version.hw_major, version.hw_minor, version.fw_major,
		version.fw_minor, version.fw_build, version.fw_type);

	return 0;
}

int pixy2_set_led(struct pixy2_transport *t, struct pixy2_led *led)
{
	int ret;
	int32_t result;
	struct pixy2_message req = PIXY2_REQUEST(PIXY2_REQUEST_SET_LED,
						 sizeof(*led), led, false);
	struct pixy2_message reply = PIXY2_REPLY(sizeof(result), &result, false);

	ret = pixy2_protocol_transceive(t, &req, &reply);
	if (ret) {
		LOG_ERR("failed to send setLED command: %d", ret);
		return ret;
	}

	return pixy2_to_errno(result);
}

int pixy2_set_lamp(struct pixy2_transport *t, struct pixy2_lamp *lamp)
{
	int ret;
	int32_t result;
	struct pixy2_message req = PIXY2_REQUEST(PIXY2_REQUEST_SET_LAMP,
						 sizeof(*lamp), lamp, false);
	struct pixy2_message reply = PIXY2_REPLY(sizeof(result), &result, false);

	ret = pixy2_protocol_transceive(t, &req, &reply);
	if (ret) {
		LOG_ERR("failed to send setLAMP command: %d", ret);
		return ret;
	}

	return pixy2_to_errno(result);
}
