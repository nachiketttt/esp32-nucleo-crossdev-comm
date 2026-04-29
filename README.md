# ESP32 × Nucleo Cross-Device Communication

<<<<<<< HEAD
Bare-metal STM32F446RE SPI master communicating with ESP32 WROOM 
SPI slave at 2MHz. Implemented without HAL using 
custom register-level SPI driver on the STM32 side and ESP-IDF 
SPI slave driver on the ESP32 side.
=======
Implementation of embedded communication protocols between STM32F446RE and ESP32-WROOM-32.
- STM32 side uses custom bare-metal HAL drivers for SPI, USART, I2C and GPIO
- ESP32 side uses ESP-IDF's hardware abstraction layer
>>>>>>> f596971 (refactor: restructure SPI project into 001_SPI folder)

## Hardware & Stack

| | |
|---|---|
| **STM32 Board** | STM32F446RE (Nucleo-64) |
| **ESP32 Module** | ESP32-WROOM-32 |
| **IDE (STM32)** | STM32CubeIDE on Fedora Linux |
| **IDE (ESP32)** | ESP-IDF on Fedora Linux |
| **CAN Transceiver** | TJA1050 |
| **Debug** | PulseView with Logic Analyser, Moserial |

<<<<<<< HEAD
| Signal | STM32 Pin | ESP32 GPIO |
|--------|-----------|------------|
| MOSI   | PA7       | GPIO12     |
| MISO   | PA6       | GPIO13     |
| SCLK   | PA5       | GPIO15     |
| CS     | PA4       | GPIO14     |
| GND    | GND       | GND        |
=======
## Driver Implementation
>>>>>>> f596971 (refactor: restructure SPI project into 001_SPI folder)

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

<<<<<<< HEAD
## A Problem I Solved
ESP32 received garbage data for over an hour. Logic analyzer 
confirmed STM32 was transmitting Hello World correctly 
(48 65 6C 6C 6F 20 57 6F 72 6C 64). Fault isolated to a wrongly 
coded slave select pin causing errors. 
Physical connection fixed, clean 
reception confirmed immediately.

## Verification

### Logic Analyzer — MOSI Signal (PulseView)
![Logic Analyzer Capture](images/001_pulseview.png)

### ESP32 Serial Monitor Output
![ESP32 Serial Output](images/001_esp32_output.png)
=======
Each protocol is contained in its own folder with a dedicated README covering
pin configuration, wiring, implementation details and logic analyser output.
>>>>>>> f596971 (refactor: restructure SPI project into 001_SPI folder)
