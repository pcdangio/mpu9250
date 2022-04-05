cmake_minimum_required(VERSION 3.2)

# Set up project.
project(mpu9250
        VERSION 1.0
        DESCRIPTION "A library for interfacing with an Invensense MPU9250 IMU.")

# Create library.
add_library(mpu9250 STATIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/driver.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/rpi_driver.cpp)

# Set include directories for internal/external use.
target_include_directories(mpu9250 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)