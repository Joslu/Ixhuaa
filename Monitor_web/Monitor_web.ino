#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels


#define OLED_RESET     D5 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);




#define DHTTYPE DHT11   // DHT 11
// Definimos parámetros
#define WIFISID "MiHogar"
#define WIFIPASS "Kamilo2016"
 
// Definimos los parámetros para el IFTTT
#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "Lab_control"
#define IFTTTKEY "POikm4lflsmBUpxBmYSS9"





#define SDHT D7
#define SGROUND A0
#define BUZZER D6
#define INDICADOR 2

DHT dht(SDHT, DHTTYPE); 

ESP8266WiFiMulti WiFiMulti;
 
// Utilizamos la conexión SSL del ESP8266
WiFiClientSecure client;
 
// Variable que permite ejecutar una sola vez la función
bool ejecutado = false;
 
void setup() {
  Serial.begin(115200);
  dht.begin();
  delay(10);
  
  pinMode(INDICADOR,OUTPUT);
  pinMode(BUZZER,OUTPUT);
  pinMode(SDHT,OUTPUT);
  pinMode(SGROUND, OUTPUT);
  digitalWrite(INDICADOR,HIGH);
  // Conectamos a la red WiFi
  WiFiMulti.addAP(WIFISID, WIFIPASS);
 
  Serial.println();
  Serial.println();
  Serial.print("Eperando a conectar a la WiFi... ");
 
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(INDICADOR,HIGH);
    delay(500);
    digitalWrite(INDICADOR,LOW);
    delay(500);
  }
 
  Serial.println("");
  Serial.println("WiFi conectada");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());
 
  delay(500);
 
  //Establecemos la semilla en un pin analogico
  randomSeed(analogRead(A0));
}
 
void loop() {
  
  if (!ejecutado)
  {
    // Obtenemos los valores
    int Temperatura = dht.readTemperature();    
    int HumedadA = dht.readHumidity();
    int HumedadS = LecturaHG();
 
    Serial.println(Temperatura);
    Serial.println(HumedadA);
    Serial.println(HumedadS);
 
    enviar_datos(Temperatura, HumedadA, HumedadS);
    ejecutado = true;
  }
}
 
void enviar_datos(int valor1, int valor2, int valor3)
{
  // Cerramos cualquier conexión anterior
  if (client.connected())
  {
    client.stop();
  }
 
  // Esperamos hasta que se hayan enviado todos los datos
  client.flush();

 client.setInsecure();
  // Hacemos la petición mediante SSL
  if (client.connect(HOSTIFTTT, 443)) {
    // Construimos la petición HTTP
    String toSend = "POST /trigger/";
    toSend += EVENTO;
    toSend += "/with/key/";
    toSend += IFTTTKEY;
    toSend += "?value1=";
    toSend += valor1;
    toSend += "&value2=";
    toSend += valor2;
    toSend += "&value3=";
    toSend += valor3;
    toSend += " HTTP/1.1\r\n";
    toSend += "Host: ";
    toSend += HOSTIFTTT;
    toSend += "\r\n";
    toSend += "Connection: close\r\n\r\n";
 
    client.print(toSend);
   
  }
 
  // Esperamos hasta que se hayan enviado todos los datos
  client.flush();
  // Desconectamos del cliente
  client.stop();
}


int LecturaHG(){

  int valorHumedad = map(analogRead(SGROUND), 0, 1023, 100, 0);
  return valorHumedad;
  }
