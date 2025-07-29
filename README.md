# Embedded Codex

This repository collects various embedded systems projects. Directories are organized by microcontroller family so examples for different boards stay grouped together.

## Directory structure

- **ARM/** – Projects for ARM Cortex boards. The `Nucleo_64` folder contains a sample STM32F401RE project (`F401RE_MAX30102_HR_SPO2_1`).
- **AVR/** – Placeholder for projects targeting AVR microcontrollers.
- **ESP/** – Directories for ESP32 and ESP8266 code. The ESP8266 folder now includes a Firebase logging example.

## Building projects

### STM32F401RE example
The project under `ARM/Nucleo_64/STM32F401RE/F401RE_MAX30102_HR_SPO2_1` was created with STM32CubeIDE. To build it you can either open the directory in STM32CubeIDE or run `make` inside the `Debug` subdirectory if the ARM GCC toolchain is installed.

### AVR and ESP
These folders contain example sketches that can be built with the Arduino IDE.
