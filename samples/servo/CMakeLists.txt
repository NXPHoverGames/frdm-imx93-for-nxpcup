cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr)
project(servo)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE servo.c)
