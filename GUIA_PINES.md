# Guía de pines — maqueta real / Tinkercad

Sketch: `alarma_uno/alarma_uno.ino` (abrir esa carpeta en Arduino IDE).

Placa: **Arduino Uno**. Velocidad: **9600 baud**.

Esta maqueta **no usa reed switch ni sensor de puerta**. El único sensor es el HC-SR04.

## Sensor (1)

| Arduino UNO | Componente |
| --- | --- |
| Pin 7 | TRIG del HC-SR04 |
| Pin 6 | ECHO del HC-SR04 |
| 5V / GND | VCC y GND del HC-SR04 |

Objeto a **menos de 20 cm** con la alarma armada → alerta.

## Actuadores (4)

| Arduino UNO | Componente | Significado |
| --- | --- | --- |
| Pin 11 | LED verde + 220 Ω | Desarmada |
| Pin 12 | LED amarillo + 220 Ω | Armada |
| Pin 13 | LED rojo + 220 Ω | Alerta |
| Pin 8 | Buzzer / piezo | Pitido en alerta |
| GND | Cátodos de LEDs y buzzer | |

## Bluetooth HC-05 (opcional, no está en Tinkercad)

| Arduino UNO | HC-05 |
| --- | --- |
| Pin 10 (RX) | TX |
| Pin 9 (TX) | RX |
| 5V | VCC |
| GND | GND |

## Comandos

Monitor Serie a 9600:

- `A` armar
- `D` desarmar
- `E` consultar estado
