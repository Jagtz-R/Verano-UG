//--- ES NECESARIA LA INSTALACIÓN DEL PAQUETE MQTT_CLIENT
//--- para más información consultar el siguinte enlace: https://pub.dev/packages/mqtt_client
//--- basado en el cpodigo oficial de EMQX proporcionado en: https://www.emqx.com/en/blog/using-mqtt-in-flutter


import 'package:mqtt_client/mqtt_client.dart'; //Importación de las clases del paquete mqtt_client para la parte del protocolo
import 'package:mqtt_client/mqtt_server_client.dart'; //Importación de las clases para realizar la conexión al broker


//--- CLASE PRINCIPAL PARA GESTIONAR LA COMUNICACIÓN MQTT
class MqttService {

  MqttServerClient? client; //Declaraión del cliente MQTT (? por si es nulo inicialmente, es decir ser tipo nulo o contener los datos de cliente)

  // --- MÉTODO DE CONEXIÓN ASÍNCORNO (connect()) BASADO EN EMQX
  Future<bool> connect(String brokerIP, int port, String clientId) async { //Dirección de red, puerto, identificador (debe ser único)
   
    //--- FORMATEO ESTRICTO DEL URI PARA WEBSOCKETS
    String websocketUrl = brokerIP;
    if (!websocketUrl.startsWith('ws://') && !websocketUrl.startsWith('wss://')) { //Como el usuario no ingresa la URI correcta para ws
      websocketUrl = 'ws://$brokerIP/mqtt'; //Conversión de 'broker.emqx.io' -> 'ws://broker.emqx.io/mqtt'
    }


    //--- INICIALIZACION DEL CLIENTE
    // usamos XX!.XX en lugar de solo simplemente XX.XX para indicar quie no son objetos null (por defecto así se declaran inicialmente) 
    client = MqttServerClient(websocketUrl, clientId); //Inicialización d euna instancia con el cliente (ya en el formato) y un ID único  
    client!.useWebSocket = true; //Forzando la arquitectura WebSocket, y no el TCP general
    client!.port = 8083; //Forzamos puerto 8083 (estándar internacional para MQTT sobre WS sin cifrar) exclusivo para WebSockets
    client!.logging(on: true); //Activación de la impresión en terminal de los procesos internos del cliente (usar para debug)
    client!.keepAlivePeriod = 20;//Tiempo máximo en SEGUNDOS que el broker esperará sin recibir datos, despues se desconectará

    //--- ASIGNACION DE CALLBACKS/ FUNCIONES DE RESPUESTA (tomadas de la pagina oficial de EMQX)
    // Esto permite que la app reaccione a eventos del servidor
    client!.onDisconnected = onDisconnected;
    client!.onConnected = onConnected;
    client!.onSubscribed = onSubscribed;
    client!.pongCallback = pong;

    //--- !! E R R O R !!
    //--- CONSTRUCCION DEL MENSAJE DE CONEXIÓN (WILLMESSAGE o TESTAMENTO) 
    // Cada linea .XXX() corresponde a un byte OBLIGATORIO en la cabecera del paquete de conexión MQTT
    // 1- withClientIdentifier registra el ID único en el broker
    // 2- startClean configura la bandera CleanSession como true
    // 3- withWillTopic, withWillMessage y withWillQos están definidos obligatoriamente por EMQX
    final connMess = MqttConnectMessage() //Con final decimos que conMess es una variable de asignación ÚNICA, así que no cambiará en la compilación
        .withClientIdentifier(clientId) //Identificador para el broker
        .withWillTopic('robot2gdl/estado/will') //Tópico de emergencia a donde se manda el testamento
        .withWillMessage('Conexion perdida repentinamente con la App')//mensaje (EVITAR ERROR DE ACENTO UTF-8)
        .startClean()//Limpiamos sesiones de mensajes pasadas
        .withWillQos(MqttQos.atLeastOnce);// Calidad de servicio 1 para garantizar la entrega del mensaje
    client!.connectionMessage = connMess; // Asignación del mensaje de conexión configurado a nuestra instancia del cliente
   
    //--- INTENTO DE CONEXIÓN CON CAPTURA DE EXCEPCIONES ESPECÍFICAS (capturas de errores en ejecucion por medio de try-catch)
    try {
      print('Iniciando conexion en $brokerIP...'); //Mensaje de depuración
      await client!.connect(); //Await para pausar la ejecución en esta parte mientras se intentarealizar la conexión
    } 
    on NoConnectionException catch (e) { //Si ocurre un error en la red o socket (como el 1883 inicial), instancia una excepción
      print('Excepcion de Red: $e'); //Mensaje de depuracipon con ficha técnica de la excepción
      client!.disconnect(); //Método para limpiar los recursos del SOCKET
      return false; //Variable bolleana del widget para conexión asíncrona FUTURE
    } 
    catch (e) { //Captura de cualquier error (como el tipado del broker por el usuario)
      print('Excepcion General: $e');//Mensaje de depuracipon con ficha técnica de la excepción
      client!.disconnect();//Método para limpiar los recursos del SOCKET
      return false;//Variable bolleana del widget para conexión asíncrona FUTURE
    }

    //--- VERIFICACIÓN DEL ESTADO FINAL DE LA CONEXIÓN
    if (client!.connectionStatus!.state == MqttConnectionState.connected) { //Si definitivamente ya confirmamos como conectado el estado del cliente
      print('Conexion exitosa albroker'); //Mensaje de depuración
      return true; //Bandera de paso paso (para la siguiente pantalla)
    } else {
      print('Falla crítica. Estado: ${client!.connectionStatus!.state}');//inspección manualm de la propiedad interna de la biblioteca para depuración
      client!.disconnect();//Método para limpiar los recursos del SOCKET
      return false;//misma variable bolleana del widget para conexión asíncrona FUTURE
    }
  }

  //--- CALLBACKS OFICIALES (DISPARADAS SEGÚN EVENTOS DEL SERVIDOR, TAMBIÉN TOMADAS DEL SITIO OFICIAL DE EMQX)
  
  void onSubscribed(String topic) { //recibe el tópico com parámetro
    print('EMQX Callback: Suscrito al topico $topic'); //Mensaje de depuración
  }

  void onDisconnected() {
    print('EMQX Callback: Cliente desconectado'); //Mensaje de depuración
    //instancia de client!. propiedad del objeto (status)!. propiedad de enumeración (ENUM origin) == constante provista por el paquete MQTT para la desconexión
    //Revisa el reporte de estatus, extrae la causa raiz y verifica si fue solicitada intenconalmente por el usuario
    if (client!.connectionStatus!.disconnectionOrigin == MqttDisconnectionOrigin.solicited) {  //usada para el botón disconnect
      print('EMQX: La desconexión solicitada por la APP'); //Mensaje de depuración
    }
  }

  void onConnected() {
    print('EMQX Callback: Conexion establecida (CB onConnected)');//Mensaje de depuración
  }

  void pong() {
    print('EMQX Callback: Ping de mantenimiento (CB pong)');//Mensaje de depuración
  }

  //--- LÓGICA DE PUBLICACIÓN DE MENSAJES (COMANDOS AL ROBOT)
  void publishMessage(String topic, String message) { //recibe como parámetros el tópico y el mensaje
    
    // 1- Validación: Comprobamos que el cliente no sea null (que exista) y esté conectado por medio de la constante MqttConnectionState.connected
    if (client == null || client!.connectionStatus!.state != MqttConnectionState.connected) {
      print('EMQX Error: Cliente desconectado'); //Mensaje de depuración
      return; //Al ser tipo void, esto detiene como una interrupción la compilación evitando un cierre forzado 
    }

    // 2- Construcción del Payload: NOTA la librería exige usar un constructor de paquetes
    final builder = MqttClientPayloadBuilder();
    
    // 3- Añadimos el formato JSON que proviene como String al constructor
    builder.addString(message);//metodo que convierte STRING a UTF-8

    // 4- Publicación en el broker
    //El formato es (topico - a donde va, QoS - con que seguridad se entrega, playload - datos que lleva)
    client!.publishMessage(topic, MqttQos.atLeastOnce, builder.payload!);  // MqttQos.atLeastOnce garantiza que el mensaje llegue al menos una vez al robot y '!' asegura a Dart que el payload no es nulo
    
    print('EMQX Callback: Mensaje publicado en [$topic] -> $message'); //Mensaje de depuracion
  }
  
}