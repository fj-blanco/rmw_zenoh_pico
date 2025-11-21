cmake_minimum_required(VERSION 3.20.0)

# Add rmw_zenoh_pico as a Zephyr extra module (module root is the repository root, containing module.yml)
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_LIST_DIR}/../../..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(rmw_zenoh_pico_talker)

target_sources(app PRIVATE src/main.c)

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
