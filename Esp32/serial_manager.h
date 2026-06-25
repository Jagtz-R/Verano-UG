/*
 * Comunicacion Serial2 con el controlador del robot.
 *
 * Este modulo traduce RobotCommand a mensajes de texto tipo CMD:* y convierte
 * los ACK recibidos por Serial2 en RobotResponse.
 */

#ifndef SERIAL_MANAGER_H
#define SERIAL_MANAGER_H

#include <Arduino.h>
#include <robot_context.h>

// Inicializa Serial2 con los pines y baudios definidos en config.h.
void serialBegin();

// Atiende la fase serial de la maquina de estados: enviar, leer o timeout.
void serialUpdate(RobotContext& context);

// Envia un comando ya validado al controlador del robot.
void serialSend(const RobotCommand& command);

// Lee bytes de Serial2 hasta formar una linea terminada en '\n'.
void serialRead(RobotContext& context);

// Interpreta una linea recibida por Serial2 y prepara la respuesta MQTT.
void parseSerialResponse(RobotContext& context, const String& message);

#endif // SERIAL_MANAGER_H
