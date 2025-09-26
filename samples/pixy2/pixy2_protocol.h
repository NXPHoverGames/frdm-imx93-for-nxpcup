/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file pixy2_protocol.h
 * @brief Pixy2 protocol layer API
 *
 * This file offers the protocol layer API required for delivering and
 * receiving messages to/from the Pixy2 camera. As opposed to the transport
 * layer, the protocol layer also performs some extra validations on the
 * received messages.
 */

#ifndef _PIXY2_PROTOCOL_H_
#define _PIXY2_PROTOCOL_H_

#include "pixy2_transport.h"

/**
 * @defgroup Pixy2RequestTypes
 * @brief Pixy2 request types
 *
 * Possible values for @ref pixy2_checksum_header::type when issuing
 * a request.
 *
 * @{
 */

/** getVersion() command request type */
#define PIXY2_REQUEST_GET_VERSION		0xe
/** setLED() command request type */
#define PIXY2_REQUEST_SET_LED			0x14
/** setLamp() command request type */
#define PIXY2_REQUEST_SET_LAMP			0x16
/** getMainFeatures() command request type */
#define PIXY2_REQUEST_GET_MAIN_FEATURES		0x30

/**
 * @}
 */

/**
 * @defgroup Pixy2ReplyTypes
 * @brief Pixy2 reply types
 *
 * Possible values for @ref pixy2_checksum_header::type when receiving
 * a reply.
 *
 * @{
 */

/** getVersion() command reply type */
#define PIXY2_REPLY_GET_VERSION			0xf
/** setLED() command reply type */
#define PIXY2_REPLY_SET_LED			0x1
/** setLamp() command reply type */
#define PIXY2_REPLY_SET_LAMP			0x1
/** getMainFeatures() command reply type */
#define PIXY2_REPLY_GET_MAIN_FEATURES		0x31
/** error reply type */
#define PIXY2_REPLY_ERROR			0x3

/**
 * @}
 */


/**
 * @defgroup Pixy2StatusCodes
 * @brief Pixy2 reply status codes
 *
 * Pixy2 reply error codes as documented in [1], "Error codes".
 *
 * [1]: https://docs.pixycam.com/wiki/doku.php?id=wiki:v2:pixy2_full_api
 *
 * @{
 */

/** command executed successfully */
#define PIXY2_OK				0
/** general error result */
#define PIXY2_ERROR				-1
/** Pixy2 has no more data */
#define PIXY2_BUSY				-2
/** data packet received contains a checksum error */
#define PIXY2_CHECKSUM_ERROR			-3
/** Pixy2 has taken too long to return its result */
#define PIXY2_TIMEOUT				-4
/** user is interacting with the button and can't perform requested action */
#define PIXY2_BUTTON_OVERRIDE			-5
/** TODO: meaning not shown in pixy2 documentation? */
#define PIXY2_PROG_CHANGING			-6

/**
 * @}
 */

/**
 * @defgroup Pixy2SyncCodes
 * @brief Pixy2 SYNC0/SYNC1 values
 *
 * @{
 */

/** request SYNC1 value if checksum is disabled */
#define PIXY2_REQUEST_SYNC0_NO_CHECKSUM		0xae
/** request SYNC0 value if checksum is disabled */
#define PIXY2_REQUEST_SYNC1_NO_CHECKSUM		0xc1
/** request SYNC0 value if checksum is enabled */
#define PIXY2_REQUEST_SYNC0_CHECKSUM		0xaf
/** request SYNC1 value if checksum is enabled */
#define PIXY2_REQUEST_SYNC1_CHECKSUM		0xc1

/** reply SYNC0 value */
#define PIXY2_REPLY_SYNC0			0xaf
/** reply SYNC1 value */
#define PIXY2_REPLY_SYNC1			0xc1

/**
 * @brief Get the value of SYNC0 based on checksum being enabled or not.
 *
 * @param checksum boolean indicating if checksum if enabled or not
 *
 * @retval #PIXY2_REQUEST_SYNC0_CHECKSUM if checksum is enabled
 * @retval #PIXY2_REQUEST_SYNC0_NO_CHECKSUM otherwise
 */
#define PIXY2_REQUEST_SYNC0(checksum)					\
	((checksum) ?							\
	 PIXY2_REQUEST_SYNC0_CHECKSUM : PIXY2_REQUEST_SYNC0_NO_CHECKSUM)

/**
 * @brief Get the value of SYNC1 based on checksum being enabled or not.
 *
 * @param checksum boolean indicating if checksum if enabled or not
 *
 * @retval #PIXY2_REQUEST_SYNC1_CHECKSUM if checksum is enabled
 * @retval #PIXY2_REQUEST_SYNC1_NO_CHECKSUM otherwise
 */
#define PIXY2_REQUEST_SYNC1(checksum)					\
	((checksum) ?							\
	 PIXY2_REQUEST_SYNC1_CHECKSUM : PIXY2_REQUEST_SYNC1_NO_CHECKSUM)

/**
 * @}
 */


/**
 * @brief Convert a Pixy2 status code to errno-like code
 *
 * @param result status code from a Pixy2 reply
 *
 * @retval errno-like converted status code
 */
int pixy2_to_errno(int32_t result);

/**
 * @brief Send a request, wait for its reply and validate it.
 *
 * This function is used to send a request to the Pixy2 camera,
 * wait for its reply and then perform some basic sanity checks
 * on it. Upper layers should use this instead of @ref pixy2_transport_transceive.
 *
 * @param t pointer to the generic transport layer data
 * @param req pointer to the request data
 * @param reply pointer to the reply data
 *
 * @retval 0 if success
 * @retval negative errno code if error
 */
int pixy2_protocol_transceive(struct pixy2_transport *t,
			      struct pixy2_message *req,
			      struct pixy2_message *reply);

#endif /* _PIXY2_PROTOCOL_H_ */
