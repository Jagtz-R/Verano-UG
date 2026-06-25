/*
 * Gestion de Serial2.
 *
 * Protocolo hacia el controlador del robot:
 *   CMD:MOVE,<joint1>,<joint2>
 *   CMD:HOME
 *   CMD:STOP
 *   CMD:STATUS
 *
 * Respuestas esperadas desde el controlador:
 *   ACK:OK
 *   ACK:ERROR
 *   ACK:BUSY
 *   ACK:READY
 */

#include <serial_manager.h>
#include <robot_context.h>
#include <config.h>

void serialBegin() {
    Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, SERIAL_RX_PIN, SERIAL_TX_PIN);
}

void serialSend(const RobotCommand& command) {
    Serial.println("\n=============================================");
    Serial.println("Enviando comando por puerto Serial...");
    Serial.print("Comando: ");

    switch (command.type) {
        case CommandType::MOVE:
            Serial2.print("CMD:MOVE,");
            Serial2.print(command.joint1, 1);
            Serial2.print(",");
            Serial2.println(command.joint2, 1);

            Serial.print("CMD:MOVE,");
            Serial.print(command.joint1, 1);
            Serial.print(",");
            Serial.println(command.joint2, 1);
            break;

        case CommandType::HOME:
            Serial2.println("CMD:HOME");
            Serial.println("CMD:HOME");
            break;

        case CommandType::STOP:
            Serial2.println("CMD:STOP");
            Serial.println("CMD:STOP");
            break;

        case CommandType::STATUS:
            Serial2.println("CMD:STATUS");
            Serial.println("CMD:STATUS");
            break;

        case CommandType::CALIBRATE:
            Serial2.println("CMD:CALIBRATE");
            Serial.println("CMD:CALIBRATE");
            break;

        default:
            Serial.println("UNKNOWN");
            break;
    }

    Serial.println("=============================================");
}

void serialUpdate(RobotContext& context) {
    // Si command_manager dejo un comando listo, se envia una sola vez.
    if (context.state == RobotState::SENDING_SERIAL) {
        serialSend(context.currentCommand);
        context.serialSentAt = millis();
        // Cambiar el estado a espera de respuesta.
        context.state = RobotState::WAITING_SERIAL_RESPONSE;
        return;
    }

    // Si el controlador no responde a tiempo, se genera un error local.
    if (context.state == RobotState::WAITING_SERIAL_RESPONSE &&
        millis() - context.serialSentAt >= SERIAL_RESPONSE_TIMEOUT_MS) {
        Serial.println("\n=============================================");
        Serial.println("ERROR");
        Serial.println("Timeout esperando respuesta serial");
        Serial.println("=============================================");
        
        // Configuracion de respuesta de error.
        context.currentResponse = RobotResponse();
        context.currentResponse.type = ResponseType::ERROR;
        context.currentResponse.message = "serial_timeout";
        context.serialMessage = "";
        context.state = RobotState::RESPONSE_READY;
        return;
    }

    serialRead(context);
}

void serialRead(RobotContext& context) {
    while (Serial2.available() > 0) {
        char c = Serial2.read();

        // El controlador debe terminar cada respuesta con '\n'.
        if (c == '\n') {
            parseSerialResponse(context, context.serialMessage);
            context.serialMessage = "";
        }
        else {
            context.serialMessage += c;
        }
    }
}

void parseSerialResponse(RobotContext& context, const String& message) {
    Serial.println("\n=============================================");
    Serial.println("Mensaje recibido por serial");
    Serial.print("Mensaje: ");
    Serial.println(message);
    Serial.println("=============================================");

    // Limpia la respuesta anterior
    context.currentResponse = RobotResponse();

    // Filtra el tipo de respuesta
    if (message.indexOf("ACK:OK") >= 0) {
        context.currentResponse.type = ResponseType::OK;
    }
    else if (message.indexOf("ACK:ERROR") >= 0) {
        context.currentResponse.type = ResponseType::ERROR;
    }
    else if (message.indexOf("ACK:BUSY") >= 0) {
        context.currentResponse.type = ResponseType::BUSY;
    }
    else if (message.indexOf("ACK:READY") >= 0) {
        context.currentResponse.type = ResponseType::READY;
    }
    else {
        context.currentResponse.type = ResponseType::UNKNOWN;
    }

    // La respuesta queda pendiente para que mqtt_manager la publique.
    context.state = RobotState::RESPONSE_READY;
}
