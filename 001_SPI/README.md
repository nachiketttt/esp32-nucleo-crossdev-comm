# STM32 ↔ ESP32 SPI Cross-Device Communication

Bare-metal STM32F446RE SPI master communicating with ESP32 WROOM 
SPI slave over 3 wires at 2MHz.

## Tests Completed
- [x] STM32 sends Hello World, ESP32 receives and prints
- [ ] ESP32 sends, STM32 receives
- [ ] Simultaneous bidirectional exchange

## Wiring

| Signal | STM32 Pin | ESP32 GPIO |
|--------|-----------|------------|
| MOSI   | PA7       | GPIO12     |
| SCLK   | PA5       | GPIO15     |
| CS     | PA4       | GPIO14     |
| GND    | GND       | GND        |

## SPI Configuration

| Parameter    | Value              |
|--------------|--------------------|
| Mode         | 0 (CPOL=0, CPHA=0) |
| Data Frame   | 8-bit              |
| Bit Order    | MSB First          |
| Clock Speed  | 2MHz               |
| NSS          | Software (SSM=1)   |

## Verification

### Logic Analyzer — MOSI Signal (PulseView)
![Logic Analyzer Capture](../images/001_pulseview.png)

### ESP32 Serial Monitor Output
![ESP32 Serial Output](../images/001_esp32_output.png)
