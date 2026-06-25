# Verano-UG

El presente repositorio contiene la base de la aplicación para el proyecto **"Control Remoto para un Robot Planar Serial de 2 GDL"**.

El objetivo de este proyecto es servir como una base modular para controlar remotamente un robot planar serial de dos grados de libertad usando un ESP32 como intermediario entre una interfaz remota y el controlador físico del robot.

## Características Principales

Este proyecto está desarrollado para la plataforma **ESP32** en colaboración con **ARDUINO** Y **FLUTTER** y comprende las siguientes funciones principales:

- Conexión a una red WiFi.
- Comunicación con un servidor MQTT.
- Recepción de comandos enviados por MQTT.
- Procesamiento de comandos para el robot.
- Comunicación serial con el controlador del robot.
- Publicación de respuestas hacia el servidor MQTT.

## Arquitectura del Proyecto

La arquitectura general del proyecto es la siguiente:

<p align="center">
<img width="405" height="362" alt="image" src="https://github.com/user-attachments/assets/3081ccf0-9337-4ef9-838e-9437106b9698" />
</p>

Si se requiere agregar comandos o respuestas, solo es necesario modificar los archivos `robot` para incluir los nuevos comandos y realizar su correcta implementación.

El proyecto fue desarrollado de forma modular para que sea fácil de modificar, ampliar e implementar en futuras etapas.
