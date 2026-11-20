#include <esp_now.h>
#include <WiFi.h>
#include <Keypad.h>

// Dirección MAC del ESP32 receptor (Robot Sumo)
uint8_t macRobot[] = {0xD4, 0xE9, 0xF4, 0xE2, 0xF1, 0x24};

const byte FILAS = 4;
const byte COLUMNAS = 4;
char teclas[FILAS][COLUMNAS] = {
  {'1', '2', '3', 'A'}, // '2'=Adelante, 'A'=Modo Manual
  {'4', '5', '6', 'B'}, // '4'=Izq, '5'=Stop, '6'=Der, 'B'=Automático + Activar BT
  {'7', '8', '9', 'C'}, // '8'=Atrás, 'C'=Habilitar/Arrancar movimiento
  {'*', '0', '#', 'D'}  // '#'=Activar BT directo en Modo Manual
};

byte pinesFilas[FILAS] = {13, 12, 14, 27};
byte pinesColumnas[COLUMNAS] = {26, 25, 33, 32};

Keypad teclado = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);

typedef struct struct_message {
  char comando;
} struct_message;

struct_message datosEnviar;
esp_now_peer_info_t peerInfo;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error iniciando ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, macRobot, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Control ESP-NOW listo.");
}

void loop() {
  char tecla = teclado.getKey();

  if (tecla) {
    char cmdTraducido = 0;
    switch (tecla) {
      case '2': cmdTraducido = 'F'; break; // Adelante
      case '8': cmdTraducido = 'B'; break; // Atrás
      case '4': cmdTraducido = 'L'; break; // Izquierda
      case '6': cmdTraducido = 'R'; break; // Derecha
      case '5': 
      case '0': cmdTraducido = 'S'; break; // Detener
      case 'A': cmdTraducido = 'A'; break; // Modo Manual
      case 'B': cmdTraducido = 'X'; break; // Automático + BT
      case 'C': cmdTraducido = 'C'; break; // Habilitar movimiento del robot
      case '#': cmdTraducido = '#'; break; // Activar BT directo
    }

    if (cmdTraducido != 0) {
      datosEnviar.comando = cmdTraducido;
      esp_now_send(macRobot, (uint8_t *) &datosEnviar, sizeof(datosEnviar));
      Serial.print("Comando enviado: ");
      Serial.println(cmdTraducido);
    }
  }
}
