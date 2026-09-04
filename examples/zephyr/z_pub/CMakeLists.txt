cmake_minimum_required(VERSION 3.20.0)

# Add rmw_zenoh_pico as a Zephyr extra module.
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/../../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rmw_zenoh_pico_z_pub)

target_sources(app PRIVATE src/main.c)

if(CONFIG_ZENOH_PICO_CLIENT_MODE)
  if(NOT DEFINED ZENOH_ROUTER_IP OR "${ZENOH_ROUTER_IP}" STREQUAL "")
    if(CONFIG_WIFI)
      message(FATAL_ERROR "ZENOH_ROUTER_IP must be provided for a Wi-Fi build")
    endif()
    set(ZENOH_ROUTER_IP "127.0.0.1")
  endif()
  target_compile_definitions(app PRIVATE ZENOH_ROUTER_IP="${ZENOH_ROUTER_IP}")
endif()

# WiFi credentials (passed via -DWIFI_SSID and -DWIFI_PASS)
if(DEFINED WIFI_SSID)
  target_compile_definitions(app PRIVATE WIFI_SSID="${WIFI_SSID}")
endif()

if(DEFINED WIFI_PASS)
  target_compile_definitions(app PRIVATE WIFI_PASS="${WIFI_PASS}")
endif()

# Use common WiFi module when CONFIG_WIFI is enabled
if(CONFIG_WIFI)
  add_subdirectory(../common ${CMAKE_CURRENT_BINARY_DIR}/common)
  target_link_libraries(app PRIVATE zephyr_common)
  # Add common include directory
  target_include_directories(app PRIVATE ../common/include)
  # Pass WiFi credentials to common module too
  if(DEFINED WIFI_SSID)
    target_compile_definitions(zephyr_common PRIVATE WIFI_SSID="${WIFI_SSID}")
  endif()
  if(DEFINED WIFI_PASS)
    target_compile_definitions(zephyr_common PRIVATE WIFI_PASS="${WIFI_PASS}")
  endif()
endif()
