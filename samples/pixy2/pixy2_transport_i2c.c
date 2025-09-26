/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/i2c.h>
#include <zephyr/logging/log.h>

#include "pixy2_transport.h"

LOG_MODULE_REGISTER(pixy2_transport_i2c);

#define pixy2_i2c_send(i2c_t, b, l)\
	i2c_write((i2c_t)->t.ctlr, (uint8_t *)b, l, (i2c_t)->address)

#define pixy2_i2c_recv(i2c_t, b, l)\
	i2c_read((i2c_t)->t.ctlr, (uint8_t *)b, l, (i2c_t)->address)

static int pixy2_transport_i2c_recv(struct pixy2_i2c_transport *i2c_t,
				    struct pixy2_message *reply)
{
	int ret;
	uint8_t payload_len;

	payload_len = reply->hdr.len;

	/* TODO: messages with checksum are currently not supported */
	if (reply->checksum) {
		return -ENOTSUP;
	}

	/* get the header */
	ret = pixy2_i2c_recv(i2c_t, &reply->hdr, sizeof(reply->hdr));
	if (ret) {
		LOG_ERR("failed to get reply header: %d", ret);
		return ret;
	}

	/* check if the payload will fit */
	if (reply->hdr.len > payload_len) {
		LOG_ERR("reply size (%d) exceeds allowed size (%d)",
			reply->hdr.len, payload_len);
		return -EINVAL;
	}

	/* good to go, get the payload */
	ret = pixy2_i2c_recv(i2c_t, reply->payload, reply->hdr.len);
	if (ret) {
		LOG_ERR("failed to get reply payload: %d", ret);
		return ret;
	}

	return 0;
}

static int pixy2_transport_i2c_send(struct pixy2_i2c_transport *i2c_t,
				    struct pixy2_message *req)
{
	int ret;

	/* TODO: messages with checksum are currently not supported */
	if (req->checksum) {
		return -ENOTSUP;
	}

	/* send the header with no checksum */
	ret = pixy2_i2c_send(i2c_t, &req->hdr, sizeof(struct pixy2_header));
	if (ret) {
		LOG_ERR("failed to send message header: %d", ret);
		return ret;
	}

	/* send the payload */
	ret = pixy2_i2c_send(i2c_t, req->payload, req->hdr.len);
	if (ret) {
		LOG_ERR("failed to send message payload: %d", ret);
		return ret;
	}

	return 0;
}

static int pixy2_transport_i2c_transceive(struct pixy2_transport *t,
					  struct pixy2_message *req,
					  struct pixy2_message *reply)
{
	int ret;
	struct pixy2_i2c_transport *i2c_t;

	i2c_t = CONTAINER_OF(t, struct pixy2_i2c_transport, t);

	/* send the request */
	ret = pixy2_transport_i2c_send(i2c_t, req);
	if (ret) {
		LOG_ERR("failed to send request: %d", ret);
		return ret;
	}

	/* get the reply */
	ret = pixy2_transport_i2c_recv(i2c_t, reply);
	if (ret) {
		LOG_ERR("failed to receive reply: %d", ret);
		return ret;
	}

	return 0;
}

const struct pixy2_transport_api pixy2_transport_i2c_api = {
	.transceive = pixy2_transport_i2c_transceive,
};
