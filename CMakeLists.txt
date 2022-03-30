cmake_minimum_required(VERSION 3.10)

project(Embedded-Tools VERSION 0.0.1)

add_executable(xy-md02 modbus_xy-md02/main.c)
target_link_libraries(xy-md02 modbus)

add_executable(gpio_wait gpio_wait/main.c)
