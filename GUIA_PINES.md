# Guía de pines — Arduino UNO + HC-05

Sketch: `alarma_uno/alarma_uno.ino` (abrir esa carpeta en Arduino IDE).

Placa: **Arduino Uno**. Velocidad: **9600 baud**.

## Bluetooth HC-05 (igual que el semáforo)

| Arduino UNO | HC-05 |
| --- | --- |
| Pin 10 (RX) | TX |
| Pin 9 (TX) | RX |
| 5V | VCC |
| GND | GND |

## LEDs (pines 11, 12 y 13)

| Arduino UNO | LED | Significado |
| --- | --- | --- |
| Pin 11 | Verde | Desarmada |
| Pin 12 | Amarillo | Armada |
| Pin 13 | Rojo | Alerta |
| GND | Cátodos | Resistencia 220 Ω en cada ánodo |

## Sensores y buzzer

| Arduino UNO | Componente |
| --- | --- |
| Pin 2 | OUT del PIR (VCC a 5V, GND a GND) |
| Pin 3 | Reed switch / contacto de puerta (el otro extremo a GND, pull-up interno) |
| Pin 8 | Buzzer activo (+) |
| GND | Buzzer (−) |

Puerta **cerrada**: imán junto al reed → pin 3 en LOW.  
Puerta **abierta**: imán lejos → pin 3 en HIGH.

## Comandos

Enviar un carácter por Monitor Serie o por Bluetooth:

- `A` armar
- `D` desarmar
- `E` consultar estado

Cierra el Monitor Serie antes de usar `python3 firmware/puente_serial.py`.
