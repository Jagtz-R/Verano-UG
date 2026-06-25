/*
 * Parser y despachador de comandos remotos.
 *
 * Entrada esperada por MQTT:
 *   {"command":"move","joint1":10,"joint2":20}
 *   {"command":"home"}
 *   {"command":"stop"}
 *   {"command":"status"}
 *   {"command":"calibrate"}
 */

#include <command_manager.h>
#include <ArduinoJson.h>

void commandParse(RobotContext& context, const String& topic, const String& payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    // Si el JSON esta mal formado, se responde error solo si el robot esta libre.
    // Si ya hay una operacion en curso, no se pisa el estado actual.
    if (error) {
        Serial.print("Error al interpretar JSON: ");
        Serial.println(error.c_str());

        if (context.state != RobotState::IDLE) {
            return;
        }

        // Configuración de respuesta de error.
        context.currentResponse = RobotResponse();
        context.currentResponse.type = ResponseType::ERROR;
        context.currentResponse.message = "invalid_json";
        context.state = RobotState::RESPONSE_READY;
        return;
    }

    String command = doc["command"] | "";

    // Evita sobrescribir un comando que ya esta en proceso. Se permite STOP para
    // poder detener el robot incluso si habia otro movimiento pendiente.
    if (context.state != RobotState::IDLE && command != "stop") {
        Serial.println("\n=============================================");
        Serial.println("DEBUG");
        Serial.println("Robot ocupado, comando ignorado");
        Serial.println("=============================================");
        return;
    }

    // Borra los comandos anteriores.
    context.currentCommand = RobotCommand();

    // Se filtra en comando recibido.
    if (command == "move") {
        context.currentCommand.type = CommandType::MOVE;
        context.currentCommand.joint1 = doc["joint1"] | 0.0f;
        context.currentCommand.joint2 = doc["joint2"] | 0.0f;
    }
    else if (command == "home") {
        context.currentCommand.type = CommandType::HOME;
    }
    else if (command == "stop") {
        context.currentCommand.type = CommandType::STOP;
    }
    else if (command == "status") {
        context.currentCommand.type = CommandType::STATUS;
    }
    else if (command == "calibrate") {
        context.currentCommand.type = CommandType::CALIBRATE;
    }
    else {
        context.currentCommand.type = CommandType::UNKNOWN;
    }

    // El siguiente paso es el debug del comando.
    context.state = RobotState::COMMAND_READY;
}

void commandUpdate(RobotContext& context) {
    // Si no hay comando no hagas nada.
    if (context.state != RobotState::COMMAND_READY) {
        return;
    }

    // Debug para comando recibido.
    Serial.println("\n=============================================");
    Serial.println("Procesando comando...");

    // Filtro para el comando recibido.
    switch (context.currentCommand.type) {
        case CommandType::MOVE:
            Serial.print("Comando: MOVE, Joint1: ");
            Serial.print(context.currentCommand.joint1, 1);
            Serial.print(", Joint2: ");
            Serial.println(context.currentCommand.joint2, 1);
            break;

        case CommandType::HOME:
            Serial.println("Comando: HOME");
            break;

        case CommandType::STOP:
            Serial.println("Comando: STOP");
            break;

        case CommandType::STATUS:
            Serial.println("Comando: STATUS");
            break;

        case CommandType::CALIBRATE:
            Serial.println("Comando: CALIBRATE");
            break;

        default:
            Serial.println("Comando: UNKNOWN");
            Serial.println("=============================================");
            // Configuracion de respuesta de error.
            context.currentResponse = RobotResponse();
            context.currentResponse.type = ResponseType::ERROR;
            context.currentResponse.message = "unknown_command";
            context.state = RobotState::RESPONSE_READY;
            return;
    }

    // El siguiente paso del loop sera enviar el comando por Serial2.
    context.state = RobotState::SENDING_SERIAL;

    Serial.println("=============================================");
}
