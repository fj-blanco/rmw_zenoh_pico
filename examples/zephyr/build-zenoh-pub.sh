#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Default values
EXAMPLE="z_pub"
BOARD="esp32_devkitc/esp32/procpu"
WIFI_SSID=""
WIFI_PSK=""
INIT_WEST=false
PRISTINE=false

usage() {
    cat <<EOF
Usage: $0 [OPTIONS]

Build Zenoh examples for Zephyr

OPTIONS:
    -e, --example EXAMPLE   Example name: z_pub or talker (default: z_pub)
    -b, --board BOARD       Board name (default: esp32_devkitc/esp32/procpu)
                           Examples: esp32_devkitc/esp32/procpu, native_sim
    --wifi-ssid SSID       WiFi SSID (required for WiFi boards)
    --wifi-pass PASS       WiFi password (required for WiFi boards)
    --init                 Initialize west workspace
    --pristine             Pristine build (clean build directory)
    -h, --help             Show this help message

EXAMPLES:
    # Build z_pub for ESP32 with WiFi
    $0 -e z_pub -b esp32_devkitc/esp32/procpu --wifi-ssid "MyWiFi" --wifi-pass "password123"
    
    # Build talker for ESP32 with WiFi
    $0 -e talker -b esp32_devkitc/esp32/procpu --wifi-ssid "MyWiFi" --wifi-pass "password123"
    
    # Build for native_sim
    $0 -e z_pub -b native_sim
    
    # Initialize west and build for ESP32
    $0 --init -e talker -b esp32_devkitc/esp32/procpu --wifi-ssid "MyWiFi" --wifi-pass "pass"
EOF
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -e|--example)
            EXAMPLE="$2"
            shift 2
            ;;
        -b|--board)
            BOARD="$2"
            shift 2
            ;;
        --wifi-ssid)
            WIFI_SSID="$2"
            shift 2
            ;;
        --wifi-pass)
            WIFI_PSK="$2"
            shift 2
            ;;
        --init)
            INIT_WEST=true
            shift
            ;;
        --pristine)
            PRISTINE=true
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Set example directory
EXAMPLE_DIR="$SCRIPT_DIR/$EXAMPLE"

# Validate example directory exists
if [ ! -d "$EXAMPLE_DIR" ]; then
    echo "Error: Example directory not found: $EXAMPLE_DIR"
    echo "Available examples: z_pub, talker"
    exit 1
fi

echo "=== Building Zenoh Example: $EXAMPLE for $BOARD ==="

cd "$EXAMPLE_DIR"

# Initialize west if requested
if [ "$INIT_WEST" = true ]; then
    echo "Initializing west workspace..."
    west update
fi

# Build command
BUILD_CMD="west build -b $BOARD"

if [ "$PRISTINE" = true ]; then
    BUILD_CMD="$BUILD_CMD --pristine"
fi

# Add WiFi credentials if provided
if [ -n "$WIFI_SSID" ]; then
    BUILD_CMD="$BUILD_CMD -- -DWIFI_SSID=\"$WIFI_SSID\""
fi

if [ -n "$WIFI_PSK" ]; then
    BUILD_CMD="$BUILD_CMD -DWIFI_PASS=\"$WIFI_PSK\""
fi

echo "Running: $BUILD_CMD"
eval "$BUILD_CMD"

echo ""
echo "=== Build Complete ==="
echo "Board: $BOARD"
echo "Build directory: $EXAMPLE_DIR/build"

if [[ "$BOARD" == esp32* ]]; then
    echo ""
    echo "To flash and monitor:"
    echo "  cd $EXAMPLE_DIR"
    echo "  west flash"
    echo "  west espressif monitor"
elif [[ "$BOARD" == "native_sim" ]]; then
    echo ""
    echo "To run:"
    echo "  $EXAMPLE_DIR/build/zephyr/zephyr.exe"
fi
