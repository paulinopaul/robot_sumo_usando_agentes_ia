/**
 * @file sketch.ino
 * @brief Robot Sumo 2WD - Firmware Simplificado con 1 Solo ESP32
 * 
 * Hardware (1 Solo ESP32 DevKitC V4):
 *   - Teclado Matricial 4x4:
 *       * Filas: GPIO 13 (R1), GPIO 12 (R2), GPIO 19 (R3), GPIO 23 (R4)
 *       * Columnas: GPIO 33 (C1), GPIO 32 (C2), GPIO 16 (C3), GPIO 17 (C4)
 *   - Motores DC (Driver L293D / Puente H):
 *       * Motor Izquierdo: IN1 = GPIO 25, IN2 = GPIO 26
 *       * Motor Derecho:   IN3 = GPIO 27, IN4 = GPIO 14
 *   - LCD 1602 I2C (0x27): SDA = GPIO 21, SCL = GPIO 22
 *   - Sensor Ultrasónico: TRIG = GPIO 5, ECHO = GPIO 18
 *   - LED RGB Estado: Rojo = GPIO 15, Azul = GPIO 4
 * 
 * Mapa de Teclas (Control Directo de Dirección):
 *   '2' -> Adelante
 *   '8' -> Atrás
 *   '4' -> Giro Izquierda (pivot)
 *   '6' -> Giro Derecha (pivot)
 *   '5' o '*' -> Parar / Detener Motores
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// ==========================================
// PINES DE MOTORES DC
// ==========================================
const uint8_t PIN_MOTOR_L_IN1 = 25;
const uint8_t PIN_MOTOR_L_IN2 = 26;
const uint8_t PIN_MOTOR_R_IN3 = 27;
const uint8_t PIN_MOTOR_R_IN4 = 14;

// Velocidad de operación (PWM 0 - 255)
const uint8_t MOTOR_SPEED = 220;

// ==========================================
// CONFIGURACIÓN DEL TECLADO 4X4
// ==========================================
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char keys[KEYPAD_ROWS][KEYPAD_COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[KEYPAD_ROWS] = {13, 12, 19, 23}; // R1, R2, R3, R4
byte colPins[KEYPAD_COLS] = {33, 32, 16, 17}; // C1, C2, C3, C4
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, KEYPAD_ROWS, KEYPAD_COLS);

// ==========================================
// PERIFÉRICOS ADICIONALES (LCD, RGB, ULTRASONIC)
// ==========================================
LiquidCrystal_I2C lcd(0x27, 16, 2);
const uint8_t PIN_RGB_RED  = 15;
const uint8_t PIN_RGB_BLUE = 4;

// Declaración de funciones
void setMotors(int leftSpeed, int rightSpeed);
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMotors();
void updateDisplay(const char* statusText);
void setRgb(bool red, bool blue);

void setup() {
  Serial.begin(115200);
  Serial.println(F("========================================"));
  Serial.println(F("  ROBOT SUMO 2WD - 1 ESP32 (SIMPLIFICADO)"));
  Serial.println(F("========================================"));
  Serial.println(F("Control por Teclado:"));
  Serial.println(F("  '2' -> Adelante"));
  Serial.println(F("  '8' -> Atrás"));
  Serial.println(F("  '4' -> Giro Izquierda"));
  Serial.println(F("  '6' -> Giro Derecha"));
  Serial.println(F("  '5' / '*' -> Parar"));

  // Configuración de pines de motor
  pinMode(PIN_MOTOR_L_IN1, OUTPUT);
  pinMode(PIN_MOTOR_L_IN2, OUTPUT);
  pinMode(PIN_MOTOR_R_IN3, OUTPUT);
  pinMode(PIN_MOTOR_R_IN4, OUTPUT);

  // Configuración de LED RGB
  pinMode(PIN_RGB_RED, OUTPUT);
  pinMode(PIN_RGB_BLUE, OUTPUT);

  // Inicializar Pantalla LCD I2C
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("ROBOT SUMO ESP32");
  updateDisplay("LISTO / STOP");

  stopMotors();
}

void loop() {
  // 1. Leer tecla del keypad
  char key = keypad.getKey();

  // 2. Permitir también control desde el Monitor Serial si se desea probar
  if (!key && Serial.available() > 0) {
    key = Serial.read();
  }

  // 3. Procesar comando de dirección
  if (key) {
    Serial.print(F("[TECLADO] Tecla: "));
    Serial.println(key);

    switch (key) {
      case '2':
      case 'F':
      case 'f':
        moveForward();
        break;

      case '8':
      case 'B':
      case 'b':
        moveBackward();
        break;

      case '4':
      case 'L':
      case 'l':
        turnLeft();
        break;

      case '6':
      case 'R':
      case 'r':
        turnRight();
        break;

      case '5':
      case '*':
      case 'S':
      case 's':
        stopMotors();
        break;

      default:
        Serial.println(F("[INFO] Tecla no asignada a direccion"));
        break;
    }
  }
}

// ==========================================
// FUNCIONES DE CONTROL DE MOVIMIENTO
// ==========================================

void moveForward() {
  setMotors(MOTOR_SPEED, MOTOR_SPEED);
  updateDisplay("ADELANTE   (2)");
  setRgb(false, true); // Azul = en movimiento
  Serial.println(F(">> ADELANTE"));
}

void moveBackward() {
  setMotors(-MOTOR_SPEED, -MOTOR_SPEED);
  updateDisplay("ATRAS      (8)");
  setRgb(false, true);
  Serial.println(F(">> ATRAS"));
}

void turnLeft() {
  setMotors(-MOTOR_SPEED, MOTOR_SPEED);
  updateDisplay("GIRO IZQ   (4)");
  setRgb(false, true);
  Serial.println(F(">> GIRO IZQUIERDA"));
}

void turnRight() {
  setMotors(MOTOR_SPEED, -MOTOR_SPEED);
  updateDisplay("GIRO DER   (6)");
  setRgb(false, true);
  Serial.println(F(">> GIRO DERECHA"));
}

void stopMotors() {
  setMotors(0, 0);
  updateDisplay("PARADA     (5)");
  setRgb(true, false); // Rojo = detenido
  Serial.println(F(">> MOTORES DETENIDOS"));
}

/**
 * Controla el puente H con velocidad PWM (-255 a 255)
 * Valor positivo = adelante, negativo = atrás, 0 = detenido
 */
void setMotors(int leftSpeed, int rightSpeed) {
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  // Motor Izquierdo
  if (leftSpeed > 0) {
    analogWrite(PIN_MOTOR_L_IN1, leftSpeed);
    analogWrite(PIN_MOTOR_L_IN2, 0);
  } else if (leftSpeed < 0) {
    analogWrite(PIN_MOTOR_L_IN1, 0);
    analogWrite(PIN_MOTOR_L_IN2, -leftSpeed);
  } else {
    analogWrite(PIN_MOTOR_L_IN1, 0);
    analogWrite(PIN_MOTOR_L_IN2, 0);
  }

  // Motor Derecho
  if (rightSpeed > 0) {
    analogWrite(PIN_MOTOR_R_IN3, rightSpeed);
    analogWrite(PIN_MOTOR_R_IN4, 0);
  } else if (rightSpeed < 0) {
    analogWrite(PIN_MOTOR_R_IN3, 0);
    analogWrite(PIN_MOTOR_R_IN4, -rightSpeed);
  } else {
    analogWrite(PIN_MOTOR_R_IN3, 0);
    analogWrite(PIN_MOTOR_R_IN4, 0);
  }
}

void updateDisplay(const char* statusText) {
  lcd.setCursor(0, 1);
  lcd.print("                "); // Limpia la segunda linea
  lcd.setCursor(0, 1);
  lcd.print(statusText);
}

void setRgb(bool red, bool blue) {
  digitalWrite(PIN_RGB_RED, red ? HIGH : LOW);
  digitalWrite(PIN_RGB_BLUE, blue ? HIGH : LOW);
}