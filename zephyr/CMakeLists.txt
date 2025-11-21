# Zephyr build integration for rmw_zenoh_pico

zephyr_library()

# Public include directory for wrapper headers
# Expose headers to applications using this module
zephyr_include_directories(${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/include)

# Sources compiled into a static library linked to the Zephyr app
zephyr_library_sources(
  ${CMAKE_CURRENT_LIST_DIR}/../rmw_zenoh_pico/src/zephyr_simple_pub.c
)
