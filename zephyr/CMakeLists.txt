# Copyright(C) 2024 eSOL Co., Ltd.
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0

if(CONFIG_RMW_ZENOH_PICO)
  set(RMW_ZENOH_PICO_ZID "-1")
  set(RMW_ZENOH_PICO_TRANSPORT_MODE "client")
  set(RMW_ZENOH_PICO_TRANSPORT_UNICAST ON)
  set(RMW_ZENOH_PICO_CONNECT "127.0.0.1")
  set(RMW_ZENOH_PICO_CONNECT_PORT "7447")
  set(RMW_ZENOH_PICO_LISTEN "127.0.0.1")
  set(RMW_ZENOH_PICO_LISTEN_PORT "-1")
  set(RMW_ZENOH_PICO_MAX_LINENESS_LEN
      ${CONFIG_RMW_ZENOH_PICO_MAX_LIVELINESS_LENGTH})

  set(rmw_zenoh_pico_generated_include
      ${ZEPHYR_BINARY_DIR}/include/generated/rmw_zenoh_pico)
  file(MAKE_DIRECTORY ${rmw_zenoh_pico_generated_include})
  configure_file(${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/src/config.h.in
                 ${rmw_zenoh_pico_generated_include}/config.h)

  include(
    ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/cmake/rmw_zenoh_pico_sources.cmake
  )

  zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/include)
  zephyr_library_named(rmw_zenoh_pico)
  zephyr_library_sources(${RMW_ZENOH_PICO_SOURCES})
  zephyr_library_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/include
    ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/src
    ${ZEPHYR_BINARY_DIR}/include/generated)
  zephyr_library_compile_definitions(
    HAVE_C_TYPESUPPORT
    ZENOH_DEBUG_ENABLE
    ZENOH_ZEPHYR)
  zephyr_library_compile_options(
    -Wall -Wextra -Werror=implicit-function-declaration -Wno-unused-function
    -Wno-unused-parameter -Wno-unused-variable)
  zephyr_library_link_libraries(microros)
endif()

if(CONFIG_RMW_ZENOH_PICO_SIMPLE_PUB)
  zephyr_library_named(rmw_zenoh_pico_simple_pub)
  zephyr_library_sources(
    ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/src/zephyr_simple_pub.c)
  zephyr_library_include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/include)
endif()
