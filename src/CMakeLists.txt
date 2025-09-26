# DON'T TOUCH
cmake_minimum_required(VERSION 3.20.0)

# DON'T TOUCH - pulls in the Zephyr build system and defines the "app" target
find_package(Zephyr)

# TODO: give your project a name
project(nxpcup)

# TODO: include your project sources here - you MUST use the "app" target
target_sources(app PRIVATE main.c)
