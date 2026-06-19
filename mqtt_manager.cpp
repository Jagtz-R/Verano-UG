/*
 * Gestion de MQTT.
 *
 * El ESP32 se comporta como puente:
 *   MQTT control topic -> command_manager -> Serial2 -> MQTT response topic
 */

#include <mqtt_manager.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <config.h>
#include <command_manager.h>
#include <ArduinoJson.h>

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Contexto definido en main.cpp. El callback MQTT lo usa para registrar comandos.
extern RobotContext robotContext;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String mqttMessagePayload = "";

    // PubSubClient entrega el payload como bytes: aqui se arma un String.
    for (unsigned int i = 0; i < length; i++) {
        mqttMessagePayload += (char)payload[i];
    }

    // El String se manda a descifrar.
    commandParse(robotContext, String(topic), mqttMessagePayload);
}

void mqttBegin() {
    mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);

    Serial.println("\n=============================================");
    Serial.println("Conectando a MQTT...");

    while (!mqttClient.connected()) {
        Serial.println("Intentando conectar a MQTT...");

        // ID unico para evitar conflictos con otros clientes MQTT.
        String clientId = "ESP32-" + String(random(0xffff), HEX);

        if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
            Serial.println("Conectado a MQTT");

            if (mqttClient.subscribe(MQTT_TOPIC_CONTROL)) {
                Serial.println("Suscripcion al topic '" MQTT_TOPIC_CONTROL "' exitosa");
            } else {
                Serial.println("Fallo en la suscripcion al topic '" MQTT_TOPIC_CONTROL "'");
            }
        }
        else {
            Serial.print("Fallo en la conexion, rc=");
            Serial.println(mqttClient.state());
            Serial.println("Intentando de nuevo...");
            delay(2000);
        }
    }

    Serial.println("=============================================");
}

void mqttUpdate() {
    mqttClient.loop();
}

void mqttPublishResponse(const RobotResponse& response) {
    JsonDocument doc;

    // Filtro segun la respuesta requerida.
    switch (response.type) {
        case ResponseType::OK:
            doc["status"] = "ok";
            break;

        case ResponseType::ERROR:
            doc["status"] = "error";
            doc["message"] = response.message;
            break;

        case ResponseType::READY:
            doc["status"] = "ready";
            break;

        case ResponseType::BUSY:
            doc["status"] = "busy";
            break;

        default:
            doc["status"] = "unknown";
            break;
    }

    // Publicar la respuesta
    String output;
    serializeJson(doc, output);
    mqttClient.publish(MQTT_TOPIC_RESPONSE, output.c_str());
}

void mqttResponseUpdate(RobotContext& context) {
    if (context.state != RobotState::RESPONSE_READY) {
        return;
    }

    mqttPublishResponse(context.currentResponse);

    // Una vez publicada la respuesta, el sistema queda listo para otro comando.
    context.state = RobotState::IDLE;
}
