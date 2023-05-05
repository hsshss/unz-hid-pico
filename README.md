# Overview

This project is an adapter to use USB HID with FM TOWNS / MSX using Raspberry Pi Pico.

# Limitation

Gamepad support currently only for PS4 / PS5 controllers.

# Hardware

https://oshwlab.com/hsshss/unz-hid-pico

### Notes

USB devices should be connected to the Raspberry Pi Pico using an OTG cable.

# Building

```bash
git clone https://github.com/hsshss/unz-hid-pico.git
cd unz-hid-pico
docker build -t unz-hid-pico-builder .
docker run --rm -v ${PWD}:/app -t unz-hid-pico-builder sh -c "mkdir -p build && cd build && cmake .. && make"
```

# Flashing

1. Disconnect all connectors
2. While holding down the BOOTSEL button and connect to PC with USB cable
3. A USB Mass Storage Device named `RPI-RP2` will be mounted
4. Drag and drop `unz-hid-pico.uf2` file to the `RPI-RP2`
