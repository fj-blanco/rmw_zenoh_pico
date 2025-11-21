# Zephyr RTOS Examples for rmw_zenoh_pico

This directory contains examples demonstrating both **bare Zenoh** communication and **ROS 2 micro-ROS** on Zephyr RTOS using the `rmw_zenoh_pico` middleware.

## Overview

The examples are organized into two categories:

1. **Bare Zenoh Examples** (`z_pub`, `z_sub`) - rmw_zenoh_pico wrapper over Zenoh-pico for raw pub/sub
2. **micro-ROS Examples** (`microros_talker`, `microros_listener`) - Full ROS 2 nodes using rmw_zenoh_pico

### Architecture

```
┌─────────────────────────────────────┐
│        ROS 2 Layer (micro-ROS)      │  ← microros_talker/listener
├─────────────────────────────────────┤
│      RMW Layer (rmw_zenoh_pico)     │
├─────────────────────────────────────┤
│    Zenoh Protocol (zenoh-pico)      │  ← z_pub/z_sub
├─────────────────────────────────────┤
│         Zephyr RTOS Kernel          │
└─────────────────────────────────────┘
```

## Prerequisites

1. **Zephyr SDK** - Follow [Zephyr Getting Started](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
2. **West** - Zephyr's meta-tool: `pip install west`
3. **CMake, Ninja** - Build tools
4. **ESP32 toolchain** (for ESP32 targets) - Installed via `west espressif install`
5. **micro-ROS** (for ROS 2 examples) - micro_ros_zephyr_module integrated in workspace

## Common WiFi Module

All examples share a common WiFi module located in `examples/zephyr/common/`:

```
common/
├── CMakeLists.txt       # Defines zephyr_common library
├── include/
│   └── wifi.h          # WiFi API
└── src/
    └── wifi.c          # WiFi implementation
```

This module provides:
- `wifi_init()` - Initialize WiFi subsystem
- `connect_to_wifi()` - Request WiFi connection
- `wait_for_wifi_connection()` - Block until connected
- `wifi_disconnect()` - Disconnect from WiFi

## Examples

### Bare Zenoh Examples

These examples use a thin rmw_zenoh_pico wrapper (internally using Zenoh-pico) for raw pub/sub communication without ROS 2.

#### 1. z_pub - Zenoh Publisher

**Location:** `examples/zephyr/z_pub/`

Publishes raw Zenoh messages on key expression `demo/example/zenoh-pico-pub`.

**Features:**
- Uses WiFi common module
- Configurable client/peer mode
- Publishes "Hello from Zephyr!" every second
- Works on ESP32 and native_sim

**Building for ESP32:**

```bash
cd examples/zephyr/z_pub
west build -b esp32_devkitc/esp32/procpu -- \
  -DCONFIG_WIFI_SSID="YourSSID" \
  -DCONFIG_WIFI_PASSWORD="YourPassword"
west flash
west espressif monitor
```

**Testing:** Use Python Zenoh subscriber:

```bash
python3 test_subscriber.py
```

#### 2. z_sub - Zenoh Subscriber

**Location:** `examples/zephyr/z_sub/`

Subscribes to raw Zenoh messages on key expression `demo/example/zenoh-pico-pub`.

**Features:**
- Mirror of z_pub with subscriber callback
- Receives and logs messages
- Same WiFi and mode configuration

**Note:** These bare Zenoh examples communicate via raw Zenoh protocol. They **do not** interoperate with ROS 2 topics unless using a Zenoh-DDS bridge.

---

### micro-ROS Examples

These examples use the full ROS 2 stack (rcl/rclc) with rmw_zenoh_pico middleware. They publish/subscribe to **ROS 2 topics** and can communicate with standard ROS 2 nodes on your host PC.

**Important:** Before building these examples, run the setup script to initialize the workspace and apply necessary patches:

```bash
cd examples/zephyr
./setup.sh
```

#### 3. microros_talker - ROS 2 Publisher

**Location:** `examples/zephyr/microros_talker/`

A micro-ROS publisher node that publishes `std_msgs/String` messages on `/chatter` topic.

**Features:**
- Full ROS 2 node using rclc
- Publishes to `/chatter` topic at 1 Hz
- Uses rmw_zenoh_pico as RMW layer
- Compatible with `ros2 topic echo /chatter`

**Building for ESP32:**

```bash
cd examples/zephyr/microros_talker
west build -b esp32_devkitc/esp32/procpu -- \
  -DCONFIG_WIFI_SSID="YourSSID" \
  -DCONFIG_WIFI_PASSWORD="YourPassword"
west flash
west espressif monitor
```

**Testing from ROS 2:**

```bash
# Terminal 1: Run rmw_zenohd router
ros2 run rmw_zenoh_cpp rmw_zenohd

# Terminal 2: Echo the topic
ros2 topic echo /chatter

# You should see: "Hello from Zephyr micro-ROS! Count: N"
```

#### 4. microros_listener - ROS 2 Subscriber

**Location:** `examples/zephyr/microros_listener/`

A micro-ROS subscriber node that listens to `std_msgs/String` messages on `/chatter` topic.

**Features:**
- Full ROS 2 node using rclc
- Subscribes to `/chatter` topic
- Logs received messages
- Compatible with `ros2 topic pub`

**Testing from ROS 2:**

```bash
# Terminal 1: Run rmw_zenohd router
ros2 run rmw_zenoh_cpp rmw_zenohd

# Terminal 2: Publish to topic
ros2 topic pub /chatter std_msgs/String "data: 'Hello from ROS 2'"

# Check ESP32 monitor - should show: Received: "Hello from ROS 2"
```

---

## Networking Modes

### Bare Zenoh (z_pub, z_sub)

- **Peer Mode** (default for native_sim): Auto-discovery via multicast
- **Client Mode** (default for ESP32): Connects to Zenoh router

Configure in `prj.conf`:

```properties
# Client mode - requires router
CONFIG_ZENOH_PICO_CLIENT_MODE=y

# Router address (hardcoded in main.c)
# Update main.c: zp_config_insert(..., "tcp/192.168.1.132:7447");
```

**Router:** Can use standalone `zenohd` or run without router in peer mode.

### micro-ROS (microros_talker, microros_listener)

- **Always uses client mode** connecting to `rmw_zenohd` router
- Router address configured via environment or code
- **Requires** the ROS 2 Zenoh router: `rmw_zenohd`

**Router Setup:**

```bash
# Start rmw_zenohd router (required for micro-ROS examples)
ros2 run rmw_zenoh_cpp rmw_zenohd

# It listens on tcp/0.0.0.0:7447 by default
```

---

## Key Differences: Bare Zenoh vs micro-ROS

| Aspect | Bare Zenoh (z_pub/z_sub) | micro-ROS (microros_*) |
|--------|--------------------------|------------------------|
| **API** | `rmw_zenoh_pico/zephyr_simple_pub.h` wrapper | ROS 2 `rcl`/`rclc` |
| **Topics** | Raw key expressions | ROS 2 topics (DDS-style) |
| **Message Format** | Raw bytes | ROS 2 message types (IDL) |
| **Interop with ROS 2** | No (unless using bridge) | Yes (native) |
| **Router** | Optional (peer mode works) | Required (`rmw_zenohd`) |
| **Use Case** | Lightweight M2M communication | Full ROS 2 integration |
| **Memory** | Lower overhead | Higher overhead |

---

## Building and Flashing

### Initialize Workspace (First Time)

## Architecture

```
rmw_zenoh_pico/
├── zephyr/                     # Zephyr module integration
│   ├── module.yml              # Module registration
│   ├── Kconfig                 # Configuration options
│   └── CMakeLists.txt          # Build integration
├── examples/zephyr/            # Zephyr examples
│   ├── z_pub_example/          # Basic zenoh-pico publisher
│   │   ├── west.yml            # Workspace manifest
│   │   ├── CMakeLists.txt      # Project build config
│   │   ├── prj.conf            # Zephyr configuration
│   │   └── src/main.c          # Application code
│   └── ros2_talker/            # ROS 2 publisher (in progress)
│       └── ...
└── scripts/
    └── build-zephyr.sh         # Build automation script
```

### Build System Flow

1. **West Workspace**: Each example is a self-contained west workspace
   - `west.yml` declares Zephyr version, zenoh-pico, and rmw_zenoh_pico modules
   - Workspace top is at `examples/zephyr/<example>/`

2. **Module Discovery**: Zephyr finds rmw_zenoh_pico via:
   - `zephyr/module.yml` registration
   - `ZEPHYR_EXTRA_MODULES` in CMakeLists.txt

3. **Configuration**: Via Kconfig (`prj.conf`):
   ```
   CONFIG_ZENOH_PICO=y         # Enable zenoh-pico
   CONFIG_RMW_ZENOH_PICO=y     # Enable RMW layer (when ready)
   ```

## Configuration Options

### Kconfig Options (menuconfig)

Access via: `west build -t menuconfig`

- `CONFIG_RMW_ZENOH_PICO` - Enable RMW zenoh-pico middleware
- `CONFIG_RMW_ZENOH_PICO_CONNECT_IP` - Zenoh router IP (default: "127.0.0.1")
- `CONFIG_RMW_ZENOH_PICO_CONNECT_PORT` - Zenoh router port (default: "7447")

### Environment Variables

The build script supports:
- `WIFI_SSID` - WiFi network name (for ESP32)
- `WIFI_PASS` - WiFi password (for ESP32)

## Supported Boards

### Tested
- `native_sim` - Native POSIX simulation (for development/testing)
- `esp32_devkitc/esp32/procpu` - ESP32 DevKit C

### Should Work (untested)
- Any Zephyr board with network support
- ESP32-based boards with WiFi
- STM32 boards with Ethernet

## Development Workflow

### 1. Test on native_sim

Fast iteration without hardware:
```bash
cd examples/zephyr/z_pub_example
../../../scripts/build-zephyr.sh --init
../../../scripts/build-zephyr.sh -b native_sim
./build/zephyr/zephyr.exe
```

### 2. Deploy to ESP32

With WiFi credentials:
```bash
export WIFI_SSID="MyNetwork"
export WIFI_PASS="MyPassword"
../../../scripts/build-zephyr.sh -b esp32_devkitc/esp32/procpu --clean

# Flash (requires esptool.py)
west flash
```

### 3. Monitor Output

```bash
# For ESP32
west espressif monitor

# Or direct serial
minicom -D /dev/ttyUSB0 -b 115200
```

## Integration Status

### ✅ Complete
- [x] Zenoh-pico module integration
- [x] Basic Zephyr module structure
- [x] Working zenoh-pico publisher example
- [x] Build script automation
- [x] ESP32 WiFi support
- [x] Native sim testing

### 🚧 In Progress
- [ ] Full RMW library build for Zephyr
- [ ] Micro-ROS stack integration (rcl, rclc, rosidl)
- [ ] ROS 2 message type support
- [ ] Complete ROS 2 publisher example
- [ ] ROS 2 subscriber example
- [ ] Service/client examples

### 📋 Planned
- [ ] Quality of Service (QoS) configuration
- [ ] Multiple publisher/subscriber support
- [ ] DDS compatibility layer
- [ ] Power management optimization
- [ ] Multi-board testing

## Troubleshooting

### Build Issues

**Problem:** `Module 'rmw_zenoh_pico' not found`
```bash
# Ensure ZEPHYR_EXTRA_MODULES is set in CMakeLists.txt
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules/rmw_zenoh_pico)
```

**Problem:** `CONFIG_ZENOH_PICO not found`
```bash
# Reinitialize workspace
../../../scripts/build-zephyr.sh --init --clean
```

### Runtime Issues

**Problem:** ESP32 won't connect to WiFi
- Check credentials: `WIFI_SSID` and `WIFI_PASS` environment variables
- Verify 2.4GHz network (ESP32 doesn't support 5GHz)
- Check `CONFIG_ESP32_WIFI_STA_AUTO_DHCPV4=y` in prj.conf

**Problem:** Zenoh session fails to open
- Ensure network is initialized first (check logs)
- Verify zenoh router is accessible
- Check `Z_CONFIG_MODE_KEY` setting (peer/client)

## References

- [Zephyr Documentation](https://docs.zephyrproject.org/)
- [Zenoh-Pico](https://github.com/eclipse-zenoh/zenoh-pico)
- [Zenoh-Pico Zephyr Support](https://github.com/eclipse-zenoh/zenoh-pico/tree/main/docs/zephyr.md)
- [Micro-ROS](https://micro.ros.org/)

## Contributing

See [CONTRIBUTING.md](../CONTRIBUTING.md) for development guidelines.

## License

Apache License 2.0 - See [LICENSE](../LICENSE)
