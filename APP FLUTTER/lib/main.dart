import 'package:flutter/material.dart'; //Librería base para widgets visuales (MaterialApp, ThemeData, Scaffold, Colors)
import 'screens/login_screen.dart'; //Importación del archivo LOCAL de la panrtilla de inicio de sesión para el arranque de la APP

//--- Función principal que ejecuta la APP
void main() {
  runApp(MyApp()); //Realiza el Launching and Inflate (pasa de un diseño abstracto de código a uno gráfico) por medio de una instancia de MainApp
}

//--- Clase padre de la aplicación 
//Es heredada de StatelessWidget pues algunos aspectos como el título o el tema no cambian en la ejecución
class MyApp extends StatelessWidget { //StatelessWidget alberga los procesos y widgets especificos para instanciar objetos  de renderizado geometrico RenderObjectWidgets
  @override //Anotacion para redefinir el comportamiento de la clase StatelessWidget


  //--- Parámetros necesario para renderizar la pantalla a través de un objeto heredado de WIDGET
  Widget build(BuildContext context) { //context es la variable que definimos para almacenar la instancia concreta de BuildContext (interacción con el framework)

    return MaterialApp( //retorno de la configuración de software de la APP con los atributos de debajo
      title: 'Control Robot Planar', //--- TITULO DE LA APLICACIÓN
      theme: ThemeData( //theme describe la tipografía y colores de la aplicación
        primarySwatch: Colors.blue, //--- PALETA DE COLORES DE LA APLICACION
      ),
      home: LoginScreen(), //home define la pantalla por defecto al abrir la app, de la clase LoginScreen en el archivo exportado
      debugShowCheckedModeBanner: false, //booleano, con ello eliminamos la etiqueta de debug en la esquina de la pantalla
    );
  }
}
