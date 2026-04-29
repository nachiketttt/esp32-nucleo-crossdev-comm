# ESP32 × Nucleo Cross-Device Communication

Implementation of embedded communication protocols between STM32F446RE and ESP32-WROOM-32.
- STM32 side uses custom bare-metal HAL drivers for SPI, USART, I2C and GPIO
- ESP32 side uses ESP-IDF's hardware abstraction layer

## Hardware & Stack

| | |
|---|---|
| **STM32 Board** | STM32F446RE (Nucleo-64) |
| **ESP32 Module** | ESP32-WROOM-32 |
| **IDE (STM32)** | STM32CubeIDE on Fedora Linux |
| **IDE (ESP32)** | ESP-IDF on Fedora Linux |
| **CAN Transceiver** | TJA1050 |
| **Debug** | PulseView with Logic Analyser, Moserial |

## Driver Implementation

| Driver | STM32 | ESP32 |
|--------|-------|-------|
| SPI | Custom bare-metal HAL | ESP-IDF HAL |
| USART | Custom bare-metal HAL | ESP-IDF HAL |
| I2C | Custom bare-metal HAL | ESP-IDF HAL |
| CAN | STM32 HAL | ESP-IDF TWAI |
| GPIO | Custom bare-metal HAL | ESP-IDF HAL |

## Protocols

| # | Protocol | Role (STM32 / ESP32) | Status |
|---|----------|----------------------|--------|
| 001 | SPI | Master / Slave | 🚧 In Progress |
| 002 | USART | TX-RX / TX-RX | ⏳ Planned |
| 003 | I2C | Master / Slave | ⏳ Planned |
| 004 | CAN (TWAI) | Node / Node | ⏳ Planned |

## Structure

Each protocol is contained in its own folder with a dedicated README covering
pin configuration, wiring, implementation details and logic analyser output.
