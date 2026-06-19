/*
 * Este archivo mantiene el loop principal lo mas simple posible. La logica
 * especifica vive en los managers: WiFi, MQTT, comandos y Serial.
 */

#include <Arduino.h>
#include <config.h>
#include <wifi_manager.h>
#include <mqtt_manager.h>
#include <command_manager.h>
#include <robot_context.h>
#include <serial_manager.h>

// Contexto global compartido por los managers.
RobotContext robotContext;

void setup() {
  // Inicializar los puertos seriales.
  // Debug y Serial2.
  Serial.begin(SERIAL_BAUD_RATE);
  serialBegin();

  // Inicializa la comunicación WiFi y enlace con el servidor MQTT.
  wifiBegin();
  mqttBegin();
}

void loop() {
  // Mantiene viva la sesion MQTT y procesa mensajes entrantes.
  mqttUpdate();

  // Convierte comandos listos en acciones de la maquina de estados.
  commandUpdate(robotContext);

  // Envia comandos por Serial2, lee respuestas y detecta timeouts.
  serialUpdate(robotContext);

  // Publica por MQTT cualquier respuesta que ya este lista.
  mqttResponseUpdate(robotContext);
}
