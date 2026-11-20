# Entorno de Simulación Wokwi (Robot Sumo 1x ESP32)

Este directorio contiene los archivos necesarios para simular el comportamiento electromecánico y de sensores del Robot Sumo en la plataforma [Wokwi](https://wokwi.com/).

---

## 1. Contenido del Directorio

| Archivo | Descripción |
| :--- | :--- |
| [`diagram.json`](file:///home/paul/Documents/workfiles/robot_sumo_usando_agenteS_de_ia/simulacion/diagram.json) | Esquema esquemático visual con las conexiones entre el ESP32, el teclado matricial 4x4, pantalla LCD 1602 I2C, sensor ultrasónico HC-SR04, LED RGB y motores DC. |
| [`libraries.txt`](file:///home/paul/Documents/workfiles/robot_sumo_usando_agenteS_de_ia/simulacion/libraries.txt) | Lista de bibliotecas externas requeridas por el simulador (`LiquidCrystal_I2C`, `Keypad`). |
| [`sketch.ino`](file:///home/paul/Documents/workfiles/robot_sumo_usando_agenteS_de_ia/simulacion/sketch.ino) | Firmware unificado para la simulación en un único microcontrolador ESP32 DevKitC V4. |

---

## 2. Instrucciones de Ejecución

1. Abrir la extensión de Wokwi en Visual Studio Code o acceder al simulador web de Wokwi.
2. Cargar la carpeta `simulacion/` como espacio de trabajo de la simulación.
3. Presionar el botón **Play** en Wokwi.
4. Interactuar con el teclado de membrana virtual para observar la respuesta en los motores y en la pantalla LCD.
