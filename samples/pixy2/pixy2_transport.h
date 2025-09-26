/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file pixy2_transport.h
 * @brief Pixy2 transport layer API
 *
 * This file offers the transport layer API required for delivering and
 * receiving messages to/from the Pixy2 camera.
 */

#ifndef _PIXY2_TRANSPORT_H_
#define _PIXY2_TRANSPORT_H_

#include <zephyr/device.h>

/**
 * @struct pixy2_checksum_header
 * @brief Pixy2 message header with checksum included
 *
 * Pixy2 message header as described in [1], "General Format"
 *
 * [1]: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:porting_guide
 */
struct pixy2_checksum_header {
	/** SYNC0 byte */
	uint8_t sync0;
	/** SYNC1 byte */
	uint8_t sync1;
	/** command type */
	uint8_t type;
	/** command payload length */
	uint8_t len;
	/** header checksum */
	uint16_t checksum;
} __packed;

/**
 * @struct pixy2_header
 * @brief Pixy2 message header with no checksum included
 *
 * Pixy2 message header as described in [1], "General Format"
 *
 * [1]: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:porting_guide
 */
struct pixy2_header {
	/** SYNC0 byte */
	uint8_t sync0;
	/** SYNC1 byte */
	uint8_t sync1;
	/** command type */
	uint8_t type;
	/** command payload length */
	uint8_t len;
} __packed;

/*
 * @struct pixy2_message
 * @brief Pixy2 message
 *
 * A Pixy2 message is either a request or a reply.
 */
struct pixy2_message {
	/** message header */
	struct pixy2_checksum_header hdr;
	/** true if checksum verification is enabled, false otherwise */
	bool checksum;
	/** payload data */
	uint8_t *payload;
};

/**
 * @struct pixy2_transport
 * @brief Pixy2 transport layer structure
 *
 * Generic transport layer structure. If your transport layer driver
 * requires no additional structure fields then use this. Otherwise,
 * create a new structure and place this one as the first field.
 */
struct pixy2_transport {
	/* peripheral controller device */
	const struct device *ctlr;
	/* transport API */
	const struct pixy2_transport_api *api;
};

/**
 * @struct pixy2_i2c_transport
 * @brief Pixy2 I2C transport structure
 *
 * Pixy2 I2C-based transport structure.
 */
struct pixy2_i2c_transport {
	/** generic transport layer data */
	struct pixy2_transport t;
	/** I2C address of the Pixy2 camera */
	const uint32_t address;
};

/**
 * @struct pixy2_spi_transport
 * @brief Pixy2 SPI transport structure
 *
 * Pixy2 SPI-based transport structure.
 */
struct pixy2_spi_transport {
	/** generic transport layer data */
	struct pixy2_transport t;
	/** SPI slave index */
	const uint32_t sidx;
};

/**
 * @struct pixy2_transport_api
 * @brief Pixy2 transport API
 *
 * API implemented by the transport layer drivers (e.g. I2C, SPI, etc..)
 */
struct pixy2_transport_api {
	int (*transceive)(struct pixy2_transport *t,
			  struct pixy2_message *req,
			  struct pixy2_message *reply);
};

/**
 * @brief Send a request and wait for its reply
 *
 * This function is used to send a request to the Pixy2 camera and wait
 * for its reply.
 *
 * @param t pointer to the generic transport layer data
 * @param req pointer to the request data
 * @param reply pointer to the reply data
 *
 * @retval 0 if success
 * @retval negative errno code if error
 */
static inline int pixy2_transport_transceive(struct pixy2_transport *t,
					     struct pixy2_message *req,
					     struct pixy2_message *reply)
{
	/* sanity checks */
	if (!t || !t->ctlr || !t->api->transceive) {
		return -EINVAL;
	}

	return t->api->transceive(t, req, reply);
}

#ifdef CONFIG_NXPCUP_PIXY2_SPI_TRANSPORT
extern const struct pixy2_transport_api pixy2_transport_spi_api;
#endif /* CONFIG_PIXY2_SPI_TRANSPORT */

#ifdef CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT
extern const struct pixy2_transport_api pixy2_transport_i2c_api;
#endif /* CONFIG_PIXY2_I2C_TRANSPORT */

#endif /* _PIXY2_TRANSPORT_H_ */
