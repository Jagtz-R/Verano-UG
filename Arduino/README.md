# Controlador de Robot RR de 2 GDL

Controlador para un robot planar RR de 2 grados de libertad utilizando Arduino y motores paso a paso.

El sistema recibe comandos desde una ESP32 mediante comunicación serial y ejecuta movimientos articulares absolutos. Además, cuenta con un proceso de calibración automática usando potenciómetros para establecer la posición cero de cada articulación.

## Características

- Control de posición angular para dos articulaciones.
- Calibración automática mediante potenciómetros.
- Movimiento absoluto por ángulo.
- Retorno a posición Home.
- Paro de movimiento mediante comando STOP.
- Comunicación serial sencilla basada en texto.
- Máquina de estados para gestionar el funcionamiento del robot.

## Hardware

### Motores

| Señal | Pin |
|---------|---------|
| STEP Motor 1 | 2 |
| DIR Motor 1 | 5 |
| STEP Motor 2 | 3 |
| DIR Motor 2 | 6 |

### Potenciómetros

| Sensor | Pin |
|---------|---------|
| Potenciómetro 1 | A8 |
| Potenciómetro 2 | A9 |

## Comunicación

La comunicación se realiza mediante `Serial2` a 115200 baudios.

### Comandos de entrada

#### Calibrar robot

```text
CMD:CALIBRATE
```

Inicia el proceso de calibración utilizando los potenciómetros.

---

#### Ir a Home

```text
CMD:HOME
```

Mueve ambas articulaciones a la posición cero.

---

#### Mover robot

```text
CMD:MOVE,th1,th2
```

Donde:

- `th1` = ángulo de la articulación 1 en grados.
- `th2` = ángulo de la articulación 2 en grados.

Ejemplo:

```text
CMD:MOVE,45,-30
```

---

#### Detener movimiento

```text
CMD:STOP
```

Detiene el movimiento actual.

---

#### Consultar estado

```text
CMD:STATUS
```

Consulta si el robot está disponible o ejecutando una tarea.

## Respuestas

### Comando aceptado

```text
ACK:OK
```

### Robot ocupado

```text
ACK:BUSY
```

### Robot listo

```text
ACK:READY
```

### Error de comando

```text
ACK:ERROR
```

## Estados del Robot

El controlador utiliza una máquina de estados simple:

| Estado | Descripción |
|----------|----------|
| CALIBRATING | Calibración mediante potenciómetros |
| READY | Listo para recibir comandos |
| MOVING | Ejecutando movimiento |
| HOMING | Regresando a la posición cero |
| ERROR | Estado de error |

## Secuencia de arranque

Al encender el sistema:

1. Se inicializan las comunicaciones seriales.
2. Se configuran los motores.
3. El robot entra automáticamente en modo calibración.
4. Se busca la posición cero utilizando los potenciómetros.
5. Una vez calibrado se envía:

```text
ACK:READY
```

y el sistema queda listo para recibir comandos.

## Librerías

- AccelStepper

## Ejemplo de uso

Consultar estado:

```text
CMD:STATUS
```

Respuesta:

```text
ACK:READY
```

Mover el robot:

```text
CMD:MOVE,30,45
```

Respuesta inmediata:

```text
ACK:OK
```

Al finalizar el movimiento:

```text
ACK:READY
```
