/*
 * Modelo de comandos aceptados por el robot.
 *
 * Los comandos llegan por MQTT en JSON y se convierten a esta estructura antes
 * de enviarse por Serial2 al controlador del robot.
 */

#ifndef ROBOT_COMMAND_H
#define ROBOT_COMMAND_H

#include <Arduino.h>

enum class CommandType {
    MOVE,    // Mover articulaciones a los valores indicados.
    HOME,    // Llevar el robot a su posicion de referencia.
    STOP,    // Solicitar paro del movimiento actual.
    STATUS,  // Consultar estado del controlador del robot.
    UNKNOWN  // Comando no reconocido.
};

struct RobotCommand {
    CommandType type = CommandType::UNKNOWN;
    int joint1 = 0;
    int joint2 = 0;
};

#endif // ROBOT_COMMAND_H
