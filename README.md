# Robot Sumo 2WD - Control por Teclado Matricial (1 Solo ESP32)

Sistema simplificado para simulación en **Wokwi** y prototipado físico con **un único microcontrolador ESP32 DevKitC V4**, donde el teclado matricial 4x4 comanda directamente la tracción y dirección del robot sumo.

---

## 1. Mapeo de Pines (1x ESP32)

| Componente | Función / Señal | Pin ESP32 (GPIO) |
| :--- | :--- | :--- |
| **Teclado 4x4 (Filas)** | R1, R2, R3, R4 | **GPIO 13, 12, 19, 23** |
| **Teclado 4x4 (Columnas)**| C1, C2, C3, C4 | **GPIO 33, 32, 16, 17** |
| **Motor Izquierdo** | IN1 (Adelante), IN2 (Atrás) | **GPIO 25, 26** |
| **Motor Derecho** | IN3 (Adelante), IN4 (Atrás) | **GPIO 27, 14** |
| **LCD 1602 I2C (0x27)** | SDA, SCL | **GPIO 21, 22** |
| **LED RGB (Estado)** | Rojo (Parada), Azul (Marcha) | **GPIO 15, 4** |
| **HC-SR04 Ultrasónico** | TRIG, ECHO | **GPIO 5, 18** |

---

## 2. Mapa de Teclas de Dirección

| Tecla | Acción en Motores | Estado LCD |
| :---: | :--- | :--- |
| **'2'** | **Adelante**: Ambos motores avanzan | `ADELANTE (2)` |
| **'8'** | **Atrás**: Ambos motores retroceden | `ATRAS (8)` |
| **'4'** | **Giro Izquierda**: Pivote sobre su eje | `GIRO IZQ (4)` |
| **'6'** | **Giro Derecha**: Pivote sobre su eje | `GIRO DER (6)` |
| **'5'** o **'*'** | **Parada**: Ambos motores detenidos | `PARADA (5)` |

---

## 3. Archivos del Proyecto para Wokwi

1. [diagram.json](file:///home/paul/Documents/workfiles/interfaces_robot/diagram.json): Esquema de simulación con 1 ESP32, teclado 4x4, LCD y motores.
2. [sketch.ino](file:///home/paul/Documents/workfiles/interfaces_robot/sketch.ino): Firmware simplificado y directo.
3. [libraries.txt](file:///home/paul/Documents/workfiles/interfaces_robot/libraries.txt): Dependencias (`LiquidCrystal_I2C`, `Keypad`).
