/*
 * Comunicacion MQTT.
 *
 * Recibe comandos remotos desde MQTT_TOPIC_CONTROL y publica respuestas en
 * MQTT_TOPIC_RESPONSE. La conversion del payload a comando queda a cargo de
 * command_manager.
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <robot_context.h>

// Configura el cliente MQTT, conecta al broker y se suscribe al topic de control.
void mqttBegin();

// Mantiene activa la conexion MQTT y permite ejecutar callbacks entrantes.
void mqttUpdate();

// Publica una respuesta cuando el contexto esta en RESPONSE_READY.
void mqttResponseUpdate(RobotContext& context);

// Convierte RobotResponse a JSON y lo publica en MQTT_TOPIC_RESPONSE.
void mqttPublishResponse(const RobotResponse& response);

#endif // MQTT_MANAGER_H
