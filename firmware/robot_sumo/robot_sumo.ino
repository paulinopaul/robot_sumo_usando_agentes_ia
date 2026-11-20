#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"

// Pines Puente H L9110S
#define MOTOR_A_A 27
#define MOTOR_A_B 26
#define MOTOR_B_A 25
#define MOTOR_B_B 33

// Pines Sensor HC-SR04 y LED RGB
#define TRIG_PIN 5
#define ECHO_PIN 18
#define LED_R 15
#define LED_G 2
#define LED_B 4

LiquidCrystal_I2C lcd(0x27, 16, 2);
BluetoothSerial SerialBT;

typedef struct struct_message {
  char comando;
} struct_message;

struct_message datosRecibidos;

bool modoAutonomo = false;
bool solicitarActivarBT = false;
bool bluetoothActivo = false;

// Procesa comandos tanto desde ESP-NOW como desde la App Bluetooth
void procesarComando(char cmd) {
  Serial.print("[COMANDO RECIBIDO] -> '");
  Serial.print(cmd);
  Serial.print("' | Via: ");
  Serial.println(bluetoothActivo ? "Bluetooth (App)" : "ESP-NOW (Control)");

  if (cmd == 'A') { 
    modoAutonomo = false;
    detenerMotores();
    setLED(0, 0, 255); // Azul
    Serial.println("[ESTADO] Cambiado a Modo MANUAL (LED Azul)");
    actualizarLCD("Modo: Manual", bluetoothActivo ? "Control: App BT" : "Control: ESP-NOW");
  } 
  else if (cmd == 'X') { 
    modoAutonomo = true;
    setLED(255, 0, 0); // Rojo
    Serial.println("[ESTADO] Cambiado a Modo AUTONOMO (LED Rojo)");
    if (!bluetoothActivo) {
      solicitarActivarBT = true;
    } else {
      actualizarLCD("Modo: Autonomo", "BT: Buscando...");
    }
  }
  else if (cmd == '#') {
    modoAutonomo = false;
    detenerMotores();
    setLED(0, 0, 255);
    Serial.println("[ESTADO] Solicitud de cambio directo a Bluetooth en Modo MANUAL");
    if (!bluetoothActivo) {
      solicitarActivarBT = true;
    }
  }
  else if (cmd == 'F' || cmd == 'B' || cmd == 'L' || cmd == 'R' || cmd == 'S') {
    if (modoAutonomo) {
      modoAutonomo = false;
      setLED(0, 0, 255);
      Serial.println("[ESTADO] Interrupcion manual desde App -> Saliendo de Modo Autonomo");
      actualizarLCD("Control: App BT", "Modo: Manual");
    }
    ejecutarMovimientoManual(cmd);
  }
}

// Callback de recepción ESP-NOW
void OnDataRecv(const esp_now_recv_info_t * info, const uint8_t *incomingData, int len) {
  memcpy(&datosRecibidos, incomingData, sizeof(datosRecibidos));
  procesarComando(datosRecibidos.comando);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\n========================================");
  Serial.println("      INICIANDO ROBOT SUMO (ESP32 #1)   ");
  Serial.println("========================================");

  pinMode(MOTOR_A_A, OUTPUT);
  pinMode(MOTOR_A_B, OUTPUT);
  pinMode(MOTOR_B_A, OUTPUT);
  pinMode(MOTOR_B_B, OUTPUT);
  detenerMotores();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  lcd.init();
  lcd.backlight();
  actualizarLCD("Robot Sumo Listo", "Control: ESP-NOW");
  setLED(0, 0, 255);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    esp_now_register_recv_cb(OnDataRecv);
    Serial.println("[OK] Antena ESP-NOW activa. Esperando ordenes del control remoto...");
  } else {
    Serial.println("[ERROR] Fallo al iniciar ESP-NOW.");
  }
}

void loop() {
  // 1. Transición de antena: Apagar ESP-NOW y encender Bluetooth en el Robot
  if (solicitarActivarBT && !bluetoothActivo) {
    solicitarActivarBT = false;
    bluetoothActivo = true;
    detenerMotores();

    Serial.println("\n[ANTENA] Apagando ESP-NOW y Wi-Fi...");
    esp_now_deinit();
    WiFi.mode(WIFI_OFF);
    delay(200);

    SerialBT.begin("ESP32_Sumo_Robot");
    Serial.println("[ANTENA] BLUETOOTH ACTIVADO!");
    Serial.println("[BT] Nombre visible para vincular: ESP32_Sumo_Robot\n");

    if (modoAutonomo) {
      actualizarLCD("BT: Activo (Auto)", "ESP32_Sumo_Robot");
    } else {
      actualizarLCD("BT: Activo (Man)", "ESP32_Sumo_Robot");
    }
  }

  // 2. Escuchar órdenes desde la aplicación de MIT App Inventor
  if (bluetoothActivo && SerialBT.available()) {
    char datoApp = SerialBT.read();
    // Ignorar saltos de línea o retornos de carro si la app los envía
    if (datoApp != '\n' && datoApp != '\r') {
      procesarComando(datoApp);
    }
  }

  // 3. Ejecutar rutina de combate si está en Modo Autónomo
  if (modoAutonomo) {
    long distancia = medirDistancia();

    Serial.print("[SENSOR HC-SR04] Distancia: ");
    if (distancia == 999) {
      Serial.print("Fuera de rango (>4m)");
    } else {
      Serial.print(distancia);
      Serial.print(" cm");
    }

    if (distancia > 0 && distancia < 30) {
      Serial.println(" -> ¡OBJETIVO DETECTADO! EMBESTIDA (PWM 255)");
      lcd.setCursor(0, 1);
      lcd.print("EMBESTIDA!      ");
      moverMotores(255, 0, 255, 0);
    } else {
      Serial.println(" -> Buscando oponente (Girando PWM 140)");
      lcd.setCursor(0, 1);
      lcd.print("Buscando...     ");
      moverMotores(140, 0, 0, 140);
    }
    delay(100); // Pausa de 100ms para no saturar el Monitor Serie y estabilizar el sensor
  }
}

void ejecutarMovimientoManual(char cmd) {
  switch (cmd) {
    case 'F': 
      Serial.println("[MOTORES] Adelante (F)");
      moverMotores(230, 0, 230, 0); 
      break;
    case 'B': 
      Serial.println("[MOTORES] Atras (B)");
      moverMotores(0, 230, 0, 230); 
      break;
    case 'L': 
      Serial.println("[MOTORES] Giro Izquierda (L)");
      moverMotores(0, 200, 200, 0); 
      break;
    case 'R': 
      Serial.println("[MOTORES] Giro Derecha (R)");
      moverMotores(200, 0, 0, 200); 
      break;
    case 'S': 
      Serial.println("[MOTORES] Detenidos (S)");
      detenerMotores(); 
      break;
  }
}

void moverMotores(int a1, int a2, int b1, int b2) {
  analogWrite(MOTOR_A_A, a1);
  analogWrite(MOTOR_A_B, a2);
  analogWrite(MOTOR_B_A, b1);
  analogWrite(MOTOR_B_B, b2);
}

void detenerMotores() { moverMotores(0, 0, 0, 0); }

long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duracion = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duracion == 0) return 999;
  return duracion * 0.034 / 2;
}

void setLED(int r, int g, int b) {
  analogWrite(LED_R, r);
  analogWrite(LED_G, g);
  analogWrite(LED_B, b);
}

void actualizarLCD(String linea1, String linea2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(linea1);
  lcd.setCursor(0, 1);
  lcd.print(linea2);
}
