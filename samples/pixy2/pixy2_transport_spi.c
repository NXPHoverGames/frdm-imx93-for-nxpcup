/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/spi.h>
#include <zephyr/logging/log.h>

#include "pixy2_transport.h"

LOG_MODULE_REGISTER(pixy2_transport_spi);

/* frequency of the SPI bus */
#define PIXY2_SPI_FREQUENCY	2000000
/* nmber of words before timeout while waiting for SYNC0 */
#define PIXY2_SPI_TIMEOUT_WORDS 10
/* idle byte */
#define PIXY2_SPI_IDLE_BYTE	0x1

#define pixy2_spi_send(t, b, l)\
	pixy2_transport_xfer_raw(t, b, l, NULL, 0)

#define pixy2_spi_recv(t, b, l)\
	pixy2_transport_xfer_raw(t, NULL, 0, b, l)

static int pixy2_transport_xfer_raw(struct pixy2_spi_transport *spi_t,
				    void *tx_buf, size_t tx_len,
				    void *rx_buf, size_t rx_len)
{
	const struct spi_config spi_cfg = {
		.operation = SPI_OP_MODE_MASTER | SPI_MODE_CPOL |
			SPI_MODE_CPHA | SPI_TRANSFER_MSB | SPI_WORD_SET(8),
		.frequency = PIXY2_SPI_FREQUENCY,
		.slave = spi_t->sidx,
	};

	const struct spi_buf tx_buffer = {
		.buf = tx_buf,
		.len = tx_len,
	};

	const struct spi_buf rx_buffer = {
		.buf = rx_buf,
		.len = rx_len,
	};

	const struct spi_buf_set tx_buffer_set = {
		.buffers = &tx_buffer,
		.count = 1,
	};

	const struct spi_buf_set rx_buffer_set = {
		.buffers = &rx_buffer,
		.count = 1,
	};

	return spi_transceive(spi_t->t.ctlr, &spi_cfg,
			      tx_len ? &tx_buffer_set : NULL,
			      rx_len ? &rx_buffer_set : NULL);
}

static int pixy2_transport_spi_wait_for_sync0(struct pixy2_spi_transport *spi_t,
					      uint8_t *sync0)
{
	int ret, i;
	uint8_t recv_byte;

	recv_byte = PIXY2_SPI_IDLE_BYTE;

	for (i = 0; (i < PIXY2_SPI_TIMEOUT_WORDS) &&
	     (recv_byte == PIXY2_SPI_IDLE_BYTE); i++) {

		ret = pixy2_spi_recv(spi_t, &recv_byte, sizeof(recv_byte));
		if (ret) {
			LOG_ERR("failed to receive byte: %d", ret);
			return ret;
		}
	}

	if ((i == PIXY2_SPI_TIMEOUT_WORDS) &&
	    (recv_byte == PIXY2_SPI_IDLE_BYTE)) {
		LOG_ERR("timeout while waiting for reply data");
		return -ETIME;
	}

	*sync0 = recv_byte;

	return 0;
}

static int pixy2_transport_spi_recv(struct pixy2_spi_transport *spi_t,
				    struct pixy2_message *reply)
{
	int ret;
	uint8_t payload_len;

	payload_len = reply->hdr.len;

	/* TODO: messages with checksum are currently not supported */
	if (reply->checksum) {
		return -ENOTSUP;
	}

	/* wait for SYNC0 byte */
	ret = pixy2_transport_spi_wait_for_sync0(spi_t, &reply->hdr.sync0);
	if (ret) {
		LOG_ERR("failed to wait for SYNC0: %d", ret);
		return ret;
	}

	/* get the rest of the reply header */
	ret = pixy2_spi_recv(spi_t, &reply->hdr.sync1,
			     sizeof(reply->hdr) - sizeof(reply->hdr.sync0));
	if (ret) {
		LOG_ERR("failed to receive reply header: %d", ret);
		return ret;
	}

	/* check if the payload will fit */
	if (reply->hdr.len > payload_len) {
		LOG_ERR("reply size (%d) exceeds allowed size (%d)",
			reply->hdr.len, payload_len);
		return -EINVAL;
	}

	/* finally, get the payload */
	ret = pixy2_spi_recv(spi_t, reply->payload, reply->hdr.len);
	if (ret) {
		LOG_ERR("failed to receive reply payload: %d", ret);
		return ret;
	}

	return 0;
}

static int pixy2_transport_spi_send(struct pixy2_spi_transport *spi_t,
				    struct pixy2_message *req)
{
	int ret;

	/* TODO: messages with checksum are currently not supported */
	if (req->checksum) {
		return -ENOTSUP;
	}

	/* send the header with no checksum */
	ret = pixy2_spi_send(spi_t, &req->hdr, sizeof(struct pixy2_header));
	if (ret) {
		LOG_ERR("failed to send message header: %d", ret);
		return ret;
	}

	/* send the payload */
	ret = pixy2_spi_send(spi_t, req->payload, req->hdr.len);
	if (ret) {
		LOG_ERR("failed to send message payload: %d", ret);
		return ret;
	}

	return 0;
}

static int pixy2_transport_spi_transceive(struct pixy2_transport *t,
					  struct pixy2_message *req,
					  struct pixy2_message *reply)
{
	int ret;
	struct pixy2_spi_transport *spi_t;

	spi_t = CONTAINER_OF(t, struct pixy2_spi_transport, t);

	/* send the request */
	ret = pixy2_transport_spi_send(spi_t, req);
	if (ret) {
		LOG_ERR("failed to send request: %d", ret);
		return ret;
	}

	/* get the reply */
	ret = pixy2_transport_spi_recv(spi_t, reply);
	if (ret) {
		LOG_ERR("failed to receive reply: %d", ret);
		return ret;
	}

	return 0;
}

const struct pixy2_transport_api pixy2_transport_spi_api = {
	.transceive = pixy2_transport_spi_transceive,
};
