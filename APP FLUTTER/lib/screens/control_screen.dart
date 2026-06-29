import 'dart:convert'; // Librería necesaria para serializar mapas de Dart a formato JSON
import 'package:flutter/material.dart'; //Librería base para widgets visuales (MaterialApp, ThemeData, Scaffold, Colors)
import '../services/mqtt_service.dart'; //Importación del servicio MQTT actualizado del archivo mqtt_service

class ControlScreen extends StatefulWidget {// StatefulWidget porque la interfaz cambiará al mostrar el indicador de carga
  // Variables tipo final provenientes de Login Screen (declaradas aqui por la inmutabilidad de ambas)
  final String topic; 
  final MqttService mqttService; 

  // Constructor para recolectar los datos del archivo login_screen por medio de Navigator.push
  const ControlScreen({
    Key? key, //Nombramiento de los parámetros a continuación
    required this.topic, //paso de los datos de topic y asignación a su variable final
    required this.mqttService //paso de los datos de MQTT y asignación a su variable final
  }) : super(key: key); //Inicialización del padre con la llave configurada con los parametros anteriores

  @override //Anotacion para redefinir el comportamiento de la clase StatefulWidget
  _ControlScreenState createState() => _ControlScreenState(); //Se crea un objeto mutable que no destruye el objeto principal LoginScreen
}

class _ControlScreenState extends State<ControlScreen> {
  // Controladores de texto para leer los ángulos de cada articulación
  final TextEditingController _theta1Controller = TextEditingController();
  final TextEditingController _theta2Controller = TextEditingController();

  //--- LÓGICA DE ENVÍO DE COMANDOS MQTT EN FORMATO JSON
  
  //--- FUNCION ANGULOS
  void _enviarComandoMove() {
    // 1. Conversión segura de texto a valores de punto flotante tipo double
    final double? theta1 = double.tryParse(_theta1Controller.text); //Nullable Type - metodo para PARSING
    final double? theta2 = double.tryParse(_theta2Controller.text); //Nullable Type - metodo para PARSING

    // 2. Validación para alerta SNACKBAR 1 - asegurar que los valores no sean nulos (envío con los angulos vaciós)
    if (theta1 != null && theta2 != null) {

      // 3. Validaci+on del ESPACIO DE TRABAJO  SNACKBAR 2 - Theta 1: [0, 90] y Theta 2: [-90, 90]
      if (theta1 < 0 || theta1 > 90 || theta2 < -90 || theta2 > 90) {

        //SNACKBAR 2 - Mensaje de error si no se está en los limites del WS
        ScaffoldMessenger.of(context).showSnackBar( //context hace referencia al Scaffold independiente de la pantalla y no como el localizador
          const SnackBar( //Parámetros del SNACKBAR 2
            content: Text('Los ángulos están fuera del espacio de trabajo.'), //Mensaje de texto
            backgroundColor: Colors.red, //Color del SNACKBAR
          ),
        );
        return; // Se aborta la función para no enviar datos al robot y para la compilación aquí
      }

      // 4. Empaquetado de los datos en un mapa estructurado si pasan la validación
      final Map<String, dynamic> payload = {// Mapa o Diccionario (key-String / Value-Dynamic para usar texto y flotantes indistintamente) 
        "command": "move",
        "joint1": theta1,
        "joint2": theta2 //Payload formato JSON
      };
      
      // 5. Codificación del mapa a una cadena de texto JSON
      String jsonString = jsonEncode(payload);
      
      // 6. Invocación del servicio para publicar el mensaje armado
      widget.mqttService.publishMessage(widget.topic, jsonString); //Calback para la publicación de mensaje
      
      print("Comando MOVE enviado: $jsonString"); //mensaje en terminal para depuración
    } else {
      // SNACKBAR 1 - Mensaje de error si la conversión de texto a número falla
      ScaffoldMessenger.of(context).showSnackBar( //Scaffold independiente 
        const SnackBar(content: Text('Ingrese ángulos válidos.')), //Parametros del SNACKBAR 1, solo mensaje
      );
    }
  }

  //--- FUNCION HOME
  void _enviarComandoHome() { 
    final Map<String, dynamic> payload = {
      "command": "home"
    };
    
    String jsonString = jsonEncode(payload);
    widget.mqttService.publishMessage(widget.topic, jsonString);
    print("Comando HOME enviado: $jsonString");
  }

  //--- FUNCION STOP
  void _enviarComandoStop() {
    final Map<String, dynamic> payload = {
      "command": "stop"
    };
    
    String jsonString = jsonEncode(payload);
    widget.mqttService.publishMessage(widget.topic, jsonString);
    print("Comando STOP enviado: $jsonString");
  }

  //--- FUNCION CALIBRATE
  void _enviarComandoCalibrate() {
    final Map<String, dynamic> payload = {
      "command": "calibrate"
    };
    
    String jsonString = jsonEncode(payload);
    widget.mqttService.publishMessage(widget.topic, jsonString);
    print("Comando CALIBRATE enviado: $jsonString");
  }

  
  //--- CONSTRUCCIÓN DE LA INTERFAZ GRÁFICA  

  @override //Anotación para redefinir el comportamiento de la clase
  Widget build(BuildContext context) {
    return Scaffold( //redefinimos los parametros visuales de _LoginScreenState para redibujar la pantalla con esta nueva estructura
      
      // Appbar para titulo
      appBar: AppBar(title: const Text("Control del Robot Planar")), //Texto superior de la segunda pantalla
      // BODY de lienzo
      body: Padding(
        padding: const EdgeInsets.all(16.0), //Margen general de 16 píxeles
        child: Column( //Alineación de los scaffolds de forma vertical, uno debajo del otro
          mainAxisAlignment: MainAxisAlignment.center,//Centrado vertical

          // HIJOS PARA LOS BOTONES
          children: [
            // Campo de entrada para Angulo THETA1
            TextField(
              controller: _theta1Controller, //controlador respectivo
              keyboardType: const TextInputType.numberWithOptions(decimal: true),// restricción a solo entradas numericas configurando teclado numérico
              decoration: const InputDecoration(//constructor para la configuración estética
                labelText: "Theta 1 (0° a 90°)", //etiqueta flotante del campo
                border: OutlineInputBorder(),//Generacion de un cuadro rectangular alrededor de la caja de texto
                prefixIcon: Icon(Icons.rotate_right), //ícono
              ),
            ),

            // Separador vertical
            const SizedBox(height: 20),
            
            // Campo de entrada para Angulo THETA2
            TextField(
              controller: _theta2Controller,//controlador respectivo
              keyboardType: const TextInputType.numberWithOptions(decimal: true),// restricción a solo entradas numericas configurando teclado numérico
              decoration: const InputDecoration( //constructor para la configuración estética
                labelText: "Theta 2 (-90° a 90°)",//etiqueta flotante del campo
                border: OutlineInputBorder(),//Generacion de un cuadro rectangular alrededor de la caja de texto
                prefixIcon: Icon(Icons.rotate_right),//ícono
              ),
            ),

            //Separador vertical
            const SizedBox(height: 30),

            // BOTON MOVE
            SizedBox( //Contenedor
              width: double.infinity, //Ancho completo
              height: 50, //Altura en pixeles
              child: ElevatedButton( //Propiedades únicas del widget(SIZED BOX)
                onPressed: _enviarComandoMove, //Disparo a la funcion de envio de comandos JSON
                child: const Text("MOVE", style: TextStyle(fontSize: 18)),//Propiedades del widget (ELEVATED BUTTON)
              ),
            ),

            // Separador vertical
            const SizedBox(height: 15),

            // BOTON STOP
            SizedBox( //Contenedor
              width: double.infinity, //Ancho completo
              height: 50, //Altura en pixeles
              child: ElevatedButton.icon( //Propiedades únicas del widget(SIZED BOX)
                onPressed: _enviarComandoStop, //Disparo a la funcion de envio de comandos JSON
                icon: const Icon(Icons.stop_circle, color: Colors.white), //ícono de stop
                label: const Text("STOP", style: TextStyle(color: Colors.white, fontSize: 18)), //Formato de texto contrastante
                style: ElevatedButton.styleFrom( //Configuracion del ELEVATED BUTTON
                  backgroundColor: Colors.red.shade700, //Color de fondo ROJO
                ),
              ),
            ),

            //Separador vertical
            const SizedBox(height: 30), 

            // LINEA SEPARADORA
            const Divider(height: 20, thickness: 1),
            const SizedBox(height: 10),

            // PARTICION HORIZONTAL PARA HOME Y CALIBRATE (dentro de CHILD para seguir con la pila vertical)
            Row( //Widget estructural qie alinea a sus hijos (CHILDREN) deizquierda a derecha
              mainAxisAlignment: MainAxisAlignment.spaceEvenly, //Tomamos el espacio horzizontal libre para ordenar simetricamente todo 
              children: [
                // Botón HOME
                Expanded( //Widget que expande su tamaño, segun el espacio en el renglon
                  child: ElevatedButton.icon(
                    onPressed: _enviarComandoHome, //Disparo a la funcion de envio de comandos JSON
                    icon: const Icon(Icons.home), //ícono de home
                    label: const Text("HOME"), //etiqueta del ELEVATED BUTTON
                    style: ElevatedButton.styleFrom( //Parametros adicionales 
                      padding: const EdgeInsets.symmetric(vertical: 15),//Padding para centrar texto e icono (arriba y abajo)
                    ),
                  ),
                ),

                //Separador horizontal
                const SizedBox(width: 15), 
                
                // Botón CALIBRATE
                Expanded(
                  child: ElevatedButton.icon( 
                    onPressed: _enviarComandoCalibrate, //Disparo a la funcion de envio de comandos JSON
                    icon: const Icon(Icons.tune), //ícono de adecuado para calibrate
                    label: const Text("CALIBRATE"), //etiqueta del ELEVATED BUTTON
                    style: ElevatedButton.styleFrom( //Parametros adicionales
                      padding: const EdgeInsets.symmetric(vertical: 15), //Padding para centrar texto e icono (arriba y abajo)
                    ),
                  ),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}