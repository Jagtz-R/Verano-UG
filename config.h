/*
 * Parametros centrales del sistema.
 *
 * Este archivo concentra la configuracion que normalmente cambia entre
 * instalaciones: red WiFi, broker MQTT, topics y comunicacion serial.
 */

#ifndef CONFIG_H
#define CONFIG_H

// Credenciales de la red local a la que se conectara el ESP32.
#define WIFI_SSID "Megacable-DBD0"
#define WIFI_PASSWORD "C2xnfDseJa"

// Broker MQTT usado para recibir comandos remotos y publicar respuestas.
#define MQTT_SERVER "broker.emqx.io"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASSWORD ""

// Topics MQTT del protocolo de la aplicacion.
#define MQTT_TOPIC_CONTROL "robot2gdl/control"
#define MQTT_TOPIC_RESPONSE "robot2gdl/response"

// Puerto serial usado para comunicarse con el controlador del robot.
#define SERIAL_BAUD_RATE 115200
#define SERIAL_RX_PIN 16
#define SERIAL_TX_PIN 17

// Tiempo maximo que se espera una respuesta despues de enviar un comando.
#define SERIAL_RESPONSE_TIMEOUT_MS 3000

#endif // CONFIG_H
