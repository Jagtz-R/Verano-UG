/* 
 * Código para el controlador del robot de 2 grados de libertada
 * planar RR. Este código será el encargado de controlar al robot
 * en función de los comandos enviados por la ESP32.
 * 
 * El controlador solo es de posiciones angulares.
 *
 * Comandos de entrada:
 *    CMD:CALIBRATE
 *    CMD:HOME
 *    CMD:MOVE,th1,th2
 *    CMD:STOP
 *    CMD:STATUS
 *
 * Comandos de salida:
 *    ACK:OK
 *    ACK:BUSY
 *    ACK:READY
 *    ACK:ERROR
 */

// LIBRERIAS

#include <AccelStepper.h>


// CONFIGURACIÓN DE PINES

// Motor 1
#define STEP1_PIN 2
#define DIR1_PIN  5

// Motor 2
#define STEP2_PIN 3
#define DIR2_PIN  6

// Potenciómetros
#define POT1_PIN A8
#define POT2_PIN A9

// Creación de los objetos motor
AccelStepper stepper1(1, STEP1_PIN, DIR1_PIN);
AccelStepper stepper2(1, STEP2_PIN, DIR2_PIN);


// PARÁMETROS DEL SISTEMA

// Paso por grado
const float PASOS_GRADO_1 = 11.1111111111;
const float PASOS_GRADO_2 = 8.8888888888;

// Datos del pot en cero grados
const int POT1_CERO = 65;
const int POT2_CERO = 515;

// Tolerancia para considera que llegó al cero
const int TOL_POT = 1;

// Estados booleanos
bool busy = false;
bool home1 = false;
bool home2 = false;


// ESTADOS DEL ROBOT

// Definición de los estados del robot
enum RobotState {
  CALIBRATING,
  READY,
  MOVING,
  HOMING,
  ERROR
};

// Definición de la variable de estado del robot
RobotState state = CALIBRATING; 

// DEFINICIONES DE FUNCIÓNES

// Función de respuesta
void sendACK(const String &msg) {
  // Debug
  Serial.print("ACK:");
  Serial.println(msg);
  // Esp32
  Serial2.print("ACK:");
  Serial2.println(msg);
}

// Filtro de media para la lectura de los potenciómetros
int readPot(int pin) {
    long suma = 0;

    for(int i = 0; i < 20; i++) {
        suma += analogRead(pin);
        delay(2);
    }

    return suma / 20;
}


// Calibración
void calibrate() {

  // Leer los potenciómetros
  int pot1 = readPot(POT1_PIN);
  int pot2 = readPot(POT2_PIN);

  // Calcular el eror
  int e1 = pot1 - POT1_CERO;
  int e2 = pot2 - POT2_CERO;

  // Motor 1
  if (abs(e1) <= TOL_POT) {
    stepper1.stop();
    home1 = true;
  }
  else {
    stepper1.setSpeed(e1 > 0 ? 100 : -100);
    stepper1.runSpeed();
  }

  // Motor 2
  if (abs(e2) <= TOL_POT) {
    stepper2.stop();
    home2 = true;
  }
  else {
    stepper2.setSpeed(e2 > 0 ? 100 : -100);
    stepper2.runSpeed();
  }

  if (home1 && home2) {
    // Definir la posición 0
    stepper1.setCurrentPosition(0);
    stepper2.setCurrentPosition(0);

    // Estoy READY
    state = READY;
    busy = false;

    sendACK("READY");
  }
}

// Home
void home() {

  // Ir a ceros
  stepper1.moveTo(0);
  stepper2.moveTo(0);

  state = HOMING;
  busy = true;

  sendACK("OK");
}

// Move to
void moveRobot(float t1, float t2) {

  // Calcular los pasos necesarios
  long p1 = -(long)(t1 * PASOS_GRADO_1);
  long p2 = -(long)(t2 * PASOS_GRADO_2);

  // Mover los moteores
  stepper1.moveTo(p1);
  stepper2.moveTo(p2);

  state = MOVING;
  busy = true;

  sendACK("OK");
}

// Stop
void stopRobot() {

  // Detener motores
  stepper1.stop();
  stepper2.stop();

  state = READY;
  busy = false;

  sendACK("OK");
}

// Leer comandos
void readCommands() {

  // Si no hay mensaje regresa
  if (!Serial2.available()) return;

  // Leer la cadena
  String cmd = Serial2.readStringUntil('\n');
  // Eliminar carácteres fantasma (\n, \r, etc.)
  cmd.trim();

  // CALIBRATE
  if (cmd == "CMD:CALIBRATE") {
    
    // Comprobar si esta ocupado
    if (busy) {
      sendACK("BUSY");
      return;
    }

    state = CALIBRATING;
    home1 = false;
    home2 = false;
    busy = true;

    sendACK("OK");
    return;
  }

  // HOME
  if (cmd == "CMD:HOME") {
    
    // Comprobar si esta ocupado
    if (busy) {
      sendACK("BUSY");
      return;
    }

    home();
    return;
  }

  // STOP
  if (cmd == "CMD:STOP") {

    // Comprobar si se está calibrando
    if (state == CALIBRATING) {
      sendACK("BUSY");
      return;
    }

    stopRobot();
    return;
  }

  // STATUS
  if (cmd == "CMD:STATUS") {
    
    if (busy) {
      sendACK("BUSY");
    }
    else {
      sendACK("READY");
    }

    return;
  }

  // MOVE
  if (cmd.startsWith("CMD:MOVE,")) {

    // Comprobar si esta ocupado
    if (busy) {
      sendACK("BUSY");
      return;
    }

    // Obtener las posiciones de las comas
    int c1 = cmd.indexOf(',');
    int c2 = cmd.indexOf(',', c1 + 1);

    // Extraer los ángulos
    float t1 = cmd.substring(c1 + 1, c2).toFloat(); 
    float t2 = cmd.substring(c2 + 1).toFloat();

    moveRobot(t1, t2);
    return;
  }

  sendACK("ERROR");
}


// SETUP
void setup() {
  
  // Iniciar las comunicacipnes seriales
  Serial.begin(115200);
  Serial2.begin(115200);

  // Velocidades máximas
  stepper1.setMaxSpeed(125);
  stepper2.setMaxSpeed(125);

  // Aceleraciones
  stepper1.setAcceleration(50);
  stepper2.setAcceleration(50);

  delay(1000);

  state = CALIBRATING;
  sendACK("BUSY");

}


// LOOP
void loop() {
  
  // Leer los comandos
  readCommands();

  // Filtrar el estado
  switch (state) {

    case CALIBRATING:
      calibrate();
      break;

    case MOVING:
      stepper1.run();
      stepper2.run();

      if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
        state = READY;
        busy = false;
        sendACK("READY");
      }

      break;

    case HOMING:
      stepper1.run();
      stepper2.run();

      if (stepper1.distanceToGo() == 0 && stepper2.distanceToGo() == 0) {
        state = READY;
        busy = false;
        sendACK("READY");
      }

      break;

    case READY:
      stepper1.run();
      stepper2.run();

    case ERROR:
      break;
  }
}
