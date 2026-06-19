/*
 * Procesamiento de comandos MQTT.
 *
 * Este modulo convierte el JSON recibido por MQTT a RobotCommand y avanza la
 * maquina de estados para que serial_manager lo envie al controlador del robot.
 */

#ifndef COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <Arduino.h>
#include <robot_context.h>

// Revisa si hay un comando listo y lo marca para envio por Serial2.
void commandUpdate(RobotContext& context);

// Interpreta el payload MQTT y actualiza el contexto con comando o error.
void commandParse(RobotContext& context, const String& topic, const String& payload);

#endif // COMMAND_MANAGER_H
