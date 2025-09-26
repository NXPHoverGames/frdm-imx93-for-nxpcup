/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/logging/log.h>

#include "pixy2_protocol.h"

LOG_MODULE_REGISTER(pixy2_protocol);

static int get_reply_type(int request_type)
{
	switch (request_type) {
	case PIXY2_REQUEST_GET_VERSION:
		return PIXY2_REPLY_GET_VERSION;
	case PIXY2_REQUEST_SET_LED:
		return PIXY2_REPLY_SET_LED;
	case PIXY2_REQUEST_SET_LAMP:
		return PIXY2_REPLY_SET_LAMP;
	case PIXY2_REQUEST_GET_MAIN_FEATURES:
		return PIXY2_REPLY_GET_MAIN_FEATURES;
	default:
		LOG_ERR("unknown request type: 0x%x", request_type);
		return -EINVAL;
	}

	CODE_UNREACHABLE;
}

int pixy2_to_errno(int32_t result)
{
	switch (result) {
	case PIXY2_OK:
		return 0;
	case PIXY2_BUSY:
		return -EBUSY;
	case PIXY2_TIMEOUT:
		return -ETIME;
	case PIXY2_ERROR:
	case PIXY2_CHECKSUM_ERROR:
	case PIXY2_BUTTON_OVERRIDE:
	case PIXY2_PROG_CHANGING:
		return -EIO;
	default:
		LOG_ERR("unknown result value: %d", result);
		return -EINVAL;
	}

	CODE_UNREACHABLE;
}

int pixy2_protocol_transceive(struct pixy2_transport *t,
			      struct pixy2_message *req,
			      struct pixy2_message *reply)
{
	int ret, expected_reply;

	/* sanity checks */
	if (!t || !req || !reply || !reply->payload) {
		return -EINVAL;
	}

	expected_reply = get_reply_type(req->hdr.type);
	if (expected_reply < 0) {
		LOG_ERR("invalid request type: 0x%x", expected_reply);
		return expected_reply;
	}

	/* send the request and get its reply */
	ret = pixy2_transport_transceive(t, req, reply);
	if (ret) {
		LOG_ERR("failed to transceive: %d", ret);
		return ret;
	}

	/* validate SYNC0 byte */
	if (reply->hdr.sync0 != PIXY2_REPLY_SYNC0) {
		LOG_ERR("invalid SYNC0 byte: 0x%x (expected) vs 0x%x (actual)",
			PIXY2_REPLY_SYNC0, reply->hdr.sync0);
		return -EINVAL;
	}

	/* validate SYNC1 byte */
	if (reply->hdr.sync1 != PIXY2_REPLY_SYNC1) {
		LOG_ERR("invalid SYNC1 byte: 0x%x (expected) vs 0x%x (actual)",
			PIXY2_REPLY_SYNC1, reply->hdr.sync1);
		return -EINVAL;
	}

	/* pixy2 may answer with ERROR type instead of the expected type */
	if (reply->hdr.type == PIXY2_REPLY_ERROR) {
		LOG_ERR("received error reply with status: %d",
			*(int32_t *)reply->payload);
		return pixy2_to_errno(*(int32_t *)reply->payload);
	}

	/* validate reply type */
	if (expected_reply != reply->hdr.type) {
		LOG_ERR("reply type mis-match: 0x%x (expected) vs 0x%x (actual)",
			expected_reply, reply->hdr.type);
		return -EINVAL;
	}

	return 0;
}
