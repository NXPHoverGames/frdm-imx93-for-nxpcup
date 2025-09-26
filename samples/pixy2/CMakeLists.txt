cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(pixy2)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE pixy2_protocol.c)
target_sources(app PRIVATE pixy2_command.c)

target_sources_ifdef(CONFIG_NXPCUP_PIXY2_I2C_TRANSPORT app PRIVATE pixy2_transport_i2c.c)
target_sources_ifdef(CONFIG_NXPCUP_PIXY2_SPI_TRANSPORT app PRIVATE pixy2_transport_spi.c)
