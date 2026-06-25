/*
 * Estado compartido de la aplicacion.
 *
 * RobotContext funciona como una pequena maquina de estados. El loop principal
 * y los distintos managers lo usan para coordinar el flujo:
 *
 * COMMAND_READY -> SENDING_SERIAL -> WAITING_SERIAL_RESPONSE
 * -> RESPONSE_READY -> IDLE
 */

#ifndef ROBOT_CONTEXT_H
#define ROBOT_CONTEXT_H

#include <Arduino.h>
#include <robot_command.h>
#include <robot_response.h>

enum class RobotState {
    IDLE,                    // No hay trabajo pendiente.
    COMMAND_READY,           // Hay un comando MQTT validado y listo para procesar.
    SENDING_SERIAL,          // El comando debe enviarse al controlador por Serial2.
    WAITING_SERIAL_RESPONSE, // El comando ya fue enviado, falta la respuesta serial.
    RESPONSE_READY           // Hay una respuesta lista para publicar por MQTT.
};

struct RobotContext {
    RobotState state = RobotState::IDLE;

    // Buffer donde se arma una linea recibida por Serial2 hasta encontrar '\n'.
    String serialMessage = "";

    // Instante en el que se envio el ultimo comando serial, se usa para timeout.
    unsigned long serialSentAt = 0;

    RobotCommand currentCommand;
    RobotResponse currentResponse;
};

#endif // ROBOT_CONTEXT_H
