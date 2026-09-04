# Common utilities for Zephyr examples
# This provides shared functionality like WiFi management

cmake_minimum_required(VERSION 3.20.0)

# Define a library target for common utilities
zephyr_library_named(zephyr_common)

# Add source files
zephyr_library_sources(src/wifi.c)

# Expose the Wi-Fi API to applications linking this library.
target_include_directories(zephyr_common PUBLIC include)
