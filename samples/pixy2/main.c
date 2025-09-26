/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Demonstrate the functionality of the Pixy2 camera
 *
 * VERY IMPORTANT:
 *
 * If your Pixy2 camera is powered from an independent source (i.e. if you
 * power off the board's power, the Pixy2 camera remains powered on) (e.g. USB
 * cable) and you with to re-run this sample (e.g. by power cycling the board),
 * please make sure you also power cycle the Pixy2 camera. Chances are you'll
 * stop the board in the middle of a transaction. Consequently, if the camera
 * is not reset, it will keep its state from the previous iteration and will
 * end up sending invalid data over during the next run of the sample. This
 * behaviour was especially observed with SPI.
 */

#include <zephyr/logging/log.h>

#include "pixy2_command.h"

LOG_MODULE_REGISTER(main);

#define PIXY2_INTERPOLATION_STEPS	100

#define PIXY2_TRANSPORT_PTR(transport)			\
	COND_CODE_1(CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT,	\
		    (&(transport)->t), (transport))

static struct pixy2_led colors[] = {
	{ .red = 255, .green = 0, .blue = 0 },
	{ .red = 0, .green = 255, .blue = 0 },
	{ .red = 0, .green = 0, .blue = 255 },
};

static int interpolate(struct pixy2_transport *t,
		       struct pixy2_led *start,
		       struct pixy2_led *end)
{
	float step;
	int i, ret;
	struct pixy2_led result;

	for (i = 0; i < PIXY2_INTERPOLATION_STEPS; i++) {
		step = (float) i / PIXY2_INTERPOLATION_STEPS;

		result.red = start->red + (end->red - start->red) * step;
		result.green = start->green + (end->green - start->green) * step;
		result.blue = start->blue + (end->blue - start->blue) * step;

		ret = pixy2_set_led(t, &result);
		if (ret) {
			LOG_ERR("failed to set LED color: %d", ret);
			return ret;
		}
	}

	return 0;
}

#if defined(CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT)
static struct pixy2_i2c_transport transport = {
	.t.ctlr = DEVICE_DT_GET(DT_NODELABEL(lpi2c3)),
	.t.api = &pixy2_transport_i2c_api,
#define PIXY2_I2C_ADDRESS		0x54
	.address = PIXY2_I2C_ADDRESS,
};
#elif defined(CONFIG_NXPCUP_PIXY2_SPI_TRANSPORT)
static struct pixy2_spi_transport transport = {
	.t.ctlr = DEVICE_DT_GET(DT_NODELABEL(lpspi3)),
	.t.api = &pixy2_transport_spi_api,
#define PIXY2_SPI_SLAVE_INDEX		0x0
	.sidx = PIXY2_SPI_SLAVE_INDEX,
};
#else
#error "No transport protocol selected"
#endif

int main(void)
{
	int ret, i;
	struct pixy2_led *start, *end;
	struct pixy2_lamp lamp = { .upper = true };
	struct pixy2_transport *t = &transport.t;

	/* print some information about the camera */
	ret = pixy2_print_version(t);
	if (ret) {
		LOG_ERR("failed to print camera version: %d", ret);
		return ret;
	}

	/* turn on the two LEDs at the top of the camera */
	ret = pixy2_set_lamp(t, &lamp);
	if (ret) {
		LOG_ERR("failed to toggle upper lamps: %d", ret);
		return ret;
	}

	i = 0;

	/* do cross-fade with the bottom RGB LED */
	while (true) {
		start = &colors[i];
		end = &colors[(i + 1) % ARRAY_SIZE(colors)];

		ret = interpolate(t, start, end);
		if (ret) {
			LOG_ERR("failed to interpolate: %d", ret);
			return ret;
		}

		i = (i + 1) % ARRAY_SIZE(colors);
	}

	return 0;
}
