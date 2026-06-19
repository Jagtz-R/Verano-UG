/*
 * Gestion de la conexion WiFi.
 *
 * Espera indefinidamente hasta conectarse.
 */

#include <wifi_manager.h>
#include <WiFi.h>
#include <config.h>

void wifiBegin() {
    Serial.println("\n=============================================");
    Serial.println("Conectando a WiFi...");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando...");
    }

    Serial.println("Conectado a WiFi");
    Serial.print("Direccion IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("=============================================");
}
