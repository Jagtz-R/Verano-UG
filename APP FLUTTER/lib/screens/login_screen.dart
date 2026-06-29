
import 'package:flutter/material.dart'; //Librería base para widgets visuales (MaterialApp, ThemeData, Scaffold, Colors)
import '../services/mqtt_service.dart'; //Importación del servicio MQTT actualizado del archivo mqtt_service
import 'control_screen.dart';//Importación de la pantalla de control a la que navegaremos después del login


//--- CLASE "PUENTE" A LA PANTALLA DE LOGIN
class LoginScreen extends StatefulWidget { // StatefulWidget esta vez porque la interfaz cambiará al mostrar el indicador de carga
  @override //Anotacion para redefinir el comportamiento de la clase StatefulWidget
  _LoginScreenState createState() => _LoginScreenState(); //Se crea un objeto mutable que no destruye el objeto principal LoginScreen
}

//-- CLASE "ESTADO" MUTABLE DE LA PANTALLA LOGIN (PRINCIPAL)
class _LoginScreenState extends State<LoginScreen> { //Clase que heredará las propiedades del estado anterior
  
  final MqttService _mqttService = MqttService(); //Instancia del servicio de comunicación (la clase principal para la gestión de MQTT)
  
  //Controladores para leer el texto ingresado por el usuario. 
  final TextEditingController _ipController = TextEditingController(text: 'broker.emqx.io'); //Texto por defecto que será mostrado inicialmente
  final TextEditingController _topicController = TextEditingController(text: 'ug/mecatronica/robot/control'); //Texto por defecto que será mostrado inicialmente
  bool _isLoading = false; //Bandera booleana para controlar la visibilidad del ícono de carga

  //--- FUNCIÓN ASÍNCRONA PARA EL PROCESO DE CONEXION AL PRESIONAR EL BOTON 
  void _connectToBroker() async {
    
    setState(() {
      _isLoading = true;//Actualizamos la bandera para mostrar el círculo de carga
    });

    // 1. Leemos los valores actuales de las cajas de texto
    String ip = _ipController.text;
    String topic = _topicController.text;
    
    // 2. Llamamos al método connect (MÉTODO DE CONEXIÓN ASÍNCORNO DEFINIDO POR FUTURE)
    bool connected = await _mqttService.connect(ip, 1883, 'robot_app_client');

    setState(() {
      _isLoading = false;//Ocultamos el círculo de carga pasando el lapso de keepalive definido en el archivo mqtt service (20s)
    });

    // 3. Evaluación del resultado de la conexión
    if (connected) { //Si la conexión fue exitosa navegamos a ControlScreen pasando la instancia MQTT y el tópico
      Navigator.push( //.push para "empujar" hacia la siguiente pantalla 
        context, // localizador
        MaterialPageRoute( //Clase de adaptación (Andorid /IOS)
          builder: (context) => ControlScreen( //Versión simplificada del constructor
            mqttService: _mqttService,//Pasamos el servicio ya conectado
            topic: topic,//Pasamos el tópico definido
          ),
        ),
      );
    } else { // Si falló, mostramos una alerta momentánea en la parte inferior de la pantalla (SNACKBAR)
      ScaffoldMessenger.of(context).showSnackBar( //Scaffold independiente para usar las alertas entre pantallas
        SnackBar(content: Text('Error al conectar.')), //Mensaje en Snackbar
      );
    }
  }


  //---DISEÑO DE LA PANTALLA LOGIN
  @override //anotación para redefinir el comportamiento de la clase
  Widget build(BuildContext context) {
    return Scaffold( //redefinimos los parametros visuales de _LoginScreenState para redibujar la pantalla con esta nueva estructura
      // Appbar para titulo
      appBar: AppBar(title: Text('LOGIN - CONEXION MQTT'), //Parte superior de la pantalla
      centerTitle: true,),//Centrado del título
      
      // BODY de lienzo
      body: Padding( 
        padding: const EdgeInsets.all(16.0), //Margen general de 16 píxeles
        child: Column( //Alieación de las cajas customizables de forma vertical, uno debajo del otro
          mainAxisAlignment: MainAxisAlignment.center, //Centrado vertical
          
          // HIJOS O CAJAS CUSTOMIZABLES PARA LOS BOTONES
          children: [
            // Campo de entrada para la dirección IP 
            TextField(
              controller: _ipController,//controlador respectivo, definido anteriormente
              decoration: InputDecoration(//constructor para la configuración estética
                labelText: 'IP del Broker MQTT',//etiqueta flotante del campo
                border: OutlineInputBorder(), //Generacion de un cuadro rectangular alrededor de la caja de texto
              ),
            ),

            // Separador vertical
            SizedBox(height: 16), 
            
            // Campo de entrada para el Tópico MQTT
            TextField(
              controller: _topicController,//controlador respectivo, definido anteriormente
              decoration: InputDecoration( //constructor para la configuración estética
                labelText: 'Tópico de Comunicación', //etiqueta flotante del campo
                border: OutlineInputBorder(), //Generacion de un cuadro rectangular alrededor de la caja de texto
              ),
            ),

            // Separador vertical (doble para el boton o animación de conecting)
            SizedBox(height: 32),
            
            // Operador ternario para el muestreo del botón o la animación:
            // si se esta conectando (isLoading true) se ejecuta CircularProgressIndicator
            // si ya esta libre (isLoading false) devuelve el botón para pulsar y reintentar
            _isLoading //Bandera definida por SetSate anteriormente
              ? CircularProgressIndicator() //Widget nativo para la animacion de un circulo de carga
              : ElevatedButton( //Clase constructora para un ELEVATED BUTTON (color solido y leve sombreado)
                  onPressed: _connectToBroker,
                  child: Text('Conectar al Robot', style: TextStyle(fontSize: 18)),//No confundir con children, esta es una propiedad única para este widget
                  style: ElevatedButton.styleFrom(//Parámetro para definir las imensiones geométricas
                    minimumSize: Size(double.infinity, 50), // Botón expandido al ancho total con double.infinity
                  ),
                ),
          ],
        ),
      ),
    );
  }
}