# Verano-UG

El objetivo de este proyecto es servir como una base modular para controlar remotamente un robot planar serial de dos grados de libertad usando un ESP32 como intermediario entre una interfaz remota y el controlador físico del robot.

El presente repositorio contiene la base de la aplicación para el proyecto **"Control Remoto para un Robot Planar Serial de 2 GDL"**.

Este proyecto está desarrollado para la plataforma **ESP32** y comprende las siguientes funciones principales:

- Conexión a una red WiFi.
- Comunicación con un servidor MQTT.
- Recepción de comandos enviados por MQTT.
- Procesamiento de comandos para el robot.
- Comunicación serial con el controlador del robot.
- Publicación de respuestas hacia el servidor MQTT.

La arquitectura del proyecto es la siguiente:

<p align="center">
<img width="405" height="362" alt="image" src="https://github.com/user-attachments/assets/3081ccf0-9337-4ef9-838e-9437106b9698" />
</p>

# Explicación General

El código se divide de la siguiente manera:

## Archivos Manager

Los archivos tipo **manager** se encargan de controlar módulos específicos del sistema, por ejemplo: WiFi, MQTT, Serial y comandos.

Cada manager tiene una responsabilidad principal y no debe encargarse de funciones que pertenecen a otros módulos. Por ejemplo, el manager de WiFi solo se encarga de establecer la conexión inalámbrica, pero no procesa comandos ni publica respuestas MQTT.

Los managers principales son:

- `wifi_manager`: gestiona la conexión del ESP32 a la red WiFi.
- `mqtt_manager`: gestiona la conexión con el broker MQTT, la recepción de mensajes y la publicación de respuestas.
- `command_manager`: interpreta los comandos recibidos por MQTT y actualiza el estado del sistema.
- `serial_manager`: envía comandos al controlador del robot mediante Serial2 y recibe sus respuestas.

## Archivos Robot

Los archivos **Robot** definen las estructuras de datos y estados usados para representar el comportamiento del sistema.

Estos archivos no controlan directamente hardware ni comunicaciones, sino que sirven como modelos compartidos entre los managers.

- `robot_command.h`: define los tipos de comandos que puede recibir el robot, como `MOVE`, `HOME`, `STOP` y `STATUS`.
- `robot_response.h`: define los tipos de respuesta que puede generar el sistema, como `OK`, `ERROR`, `BUSY`, `READY` y `UNKNOWN`.
- `robot_context.h`: contiene el estado global del robot y permite coordinar el flujo entre MQTT, comandos y comunicación serial.

## Archivo Config

El archivo `config.h` concentra los parámetros principales de configuración del sistema.

En este archivo se definen:

- Nombre y contraseña de la red WiFi.
- Dirección y puerto del broker MQTT.
- Topics MQTT de control y respuesta.
- Velocidad de comunicación serial.
- Pines RX y TX usados por Serial2.
- Tiempo máximo de espera para una respuesta serial.

Esto permite modificar la configuración principal del proyecto desde un solo lugar.

## Archivo Main

El archivo `main.cpp` es el punto de entrada del código.

Su función es inicializar los módulos principales y ejecutar continuamente el ciclo de trabajo del sistema.

Durante `setup()` se inicializan:

- Monitor serial.
- Comunicación Serial2.
- Conexión WiFi.
- Conexión MQTT.

Durante `loop()` se ejecutan las tareas principales:

- Mantener activa la comunicación MQTT.
- Procesar comandos pendientes.
- Enviar comandos al controlador del robot.
- Leer respuestas seriales.
- Publicar respuestas por MQTT.

# Flujo General del Sistema

El funcionamiento general del sistema es el siguiente:

1. El ESP32 se conecta a una red WiFi.
2. Después se conecta al broker MQTT.
3. El ESP32 se suscribe al topic de control.
4. Un cliente remoto publica un comando MQTT.
5. El ESP32 recibe el comando y lo interpreta como JSON.
6. El comando se convierte a una estructura interna `RobotCommand`.
7. El comando se envía por Serial2 al controlador del robot.
8. El controlador responde con un mensaje tipo `ACK`.
9. El ESP32 interpreta la respuesta serial.
10. Finalmente, publica una respuesta en el topic MQTT de respuesta.

# Máquina de Estados

El sistema utiliza una máquina de estados sencilla para coordinar el flujo entre MQTT y Serial.

Los estados principales son:

- `IDLE`: el sistema está libre y puede recibir un nuevo comando.
- `COMMAND_READY`: hay un comando MQTT válido listo para procesarse.
- `SENDING_SERIAL`: el comando debe enviarse al controlador del robot.
- `WAITING_SERIAL_RESPONSE`: el comando ya fue enviado y se espera respuesta serial.
- `RESPONSE_READY`: hay una respuesta lista para publicarse por MQTT.

El flujo normal es:

```text
IDLE -> COMMAND_READY -> SENDING_SERIAL -> WAITING_SERIAL_RESPONSE -> RESPONSE_READY -> IDLE
````
# Formato de Comandos MQTT

Los comandos se envían al topic definido en `MQTT_TOPIC_CONTROL`.

Ejemplos de comandos válidos:

## Movimiento

```json
{
  "command": "move",
  "joint1": 30,
  "joint2": 45
}
```

## Home

```json
{
  "command": "home"
}
```

## Stop

```json
{
  "command": "stop"
}
```

## Status

```json
{
  "command": "status"
}
```

# Formato de Comunicación Serial

El ESP32 se comunica con el controlador del robot mediante `Serial2`.

Los comandos enviados por el ESP32 tienen el siguiente formato:

```text
CMD:MOVE,<joint1>,<joint2>
CMD:HOME
CMD:STOP
CMD:STATUS
```

Ejemplo:

```text
CMD:MOVE,30,45
```

El controlador del robot debe responder con mensajes tipo `ACK`, por ejemplo:

```text
ACK:OK
ACK:ERROR
ACK:BUSY
ACK:READY
```

Cada respuesta debe terminar con salto de línea `\n`, ya que el ESP32 procesa las respuestas por línea completa.

# Respuestas MQTT

Las respuestas se publican en el topic definido en `MQTT_TOPIC_RESPONSE`.

Ejemplos:

```json
{
  "status": "ok"
}
```

```json
{
  "status": "error",
  "message": "serial_timeout"
}
```

```json
{
  "status": "busy"
}
```

# Timeout Serial

Después de enviar un comando por Serial2, el ESP32 espera una respuesta del controlador del robot.

Si no recibe respuesta dentro del tiempo definido por:

```cpp
#define SERIAL_RESPONSE_TIMEOUT_MS 3000
```

el sistema genera una respuesta de error:

```json
{
  "status": "error",
  "message": "serial_timeout"
}
```

Esto evita que el ESP32 quede bloqueado indefinidamente esperando una respuesta.

# To do

Aún falta el desarrollo de la aplicación y la modificación del controlador del robot para que responda a los comandos anteriormente definidos. Si se requiere agregar comandos o respuestas solo hay necesidad de modificar los archivos `robot` para incluir los nuevos comandos y realizar su correcta implementación. 

Finalmente el proyecto fue desarrollado para que todo fuera modular, es decir, que fuera fácil de modificar e implementar. Sin más que decir, esto ha sido la descripción general del proyecto.
