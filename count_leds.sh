#!/bin/zsh

# Exit immediately if a command exits with a non-zero status
set -e

# PlatformIO binary path
PIO="$HOME/.platformio/penv/bin/pio"

if [ ! -f "$PIO" ]; then
    echo "Error: PlatformIO Core not found at $PIO"
    exit 1
fi

echo "=================================================="
echo "===> Compiling and Uploading to Arduino Nano...  "
echo "=================================================="
$PIO run --target upload

echo "\n=================================================="
echo "===> Detecting serial port...                     "
echo "=================================================="

# Search for common USB serial patterns on macOS
PORT=$(ls -1 /dev/cu.usbserial-* /dev/cu.usbmodem* /dev/cu.wchusbserial* 2>/dev/null | head -n 1)

if [ -z "$PORT" ]; then
    echo "Warning: USB serial port not automatically found via /dev/cu.*"
    echo "Attempting to use PlatformIO's built-in monitor autodetection..."
    $PIO device monitor -b 9600
    exit 0
fi

echo "Found port: $PORT"
echo "Starting Serial Monitor (9600 baud)..."
echo "--------------------------------------------------"
echo "To exit the serial monitor:"
echo "  - If tio starts: Press Ctrl+T, then Q"
echo "  - If minicom starts: Press Ctrl+A, then Q"
echo "  - If screen starts: Press Ctrl+A, then K (and confirm with Y)"
echo "  - If pio monitor starts: Press Ctrl+C"
echo "--------------------------------------------------"
sleep 1.5

# Check and use the best available serial tool
if command -v tio >/dev/null 2>&1; then
    echo "Using tio..."
    tio -b 9600 "$PORT"
elif [ -x "/opt/homebrew/bin/minicom" ]; then
    echo "Using minicom..."
    /opt/homebrew/bin/minicom -D "$PORT" -b 9600
elif [ -x "/usr/bin/screen" ]; then
    echo "Using screen..."
    /usr/bin/screen "$PORT" 9600
else
    echo "Using PlatformIO device monitor..."
    $PIO device monitor -b 9600 -p "$PORT"
fi
