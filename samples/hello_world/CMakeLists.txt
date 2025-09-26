# taken and adjusted from: https://docs.zephyrproject.org/latest/develop/application/index.html

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(my_zephyr_app)

target_sources(app PRIVATE main.c)
