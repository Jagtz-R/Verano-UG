/*
 * Modelo de respuestas que se publican por MQTT.
 *
 * Las respuestas se originan al interpretar los ACK recibidos desde Serial2 o
 * al detectar errores locales, por ejemplo JSON invalido o timeout serial.
 */

#ifndef ROBOT_RESPONSE_H
#define ROBOT_RESPONSE_H

#include <Arduino.h>

enum class ResponseType {
    OK,
    ERROR,
    BUSY,
    READY,
    UNKNOWN
};

struct RobotResponse {
    ResponseType type = ResponseType::UNKNOWN;
    String message = "";
};

#endif // ROBOT_RESPONSE_H
