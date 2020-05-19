//Based on work by https://github.com/hansjny

#include <ESP8266WiFi.h>
#include "secrets.h"


#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println (x)
#define DEBUG_PRINT(x)    Serial.print(x)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#endif




int state = HIGH;
int sensorReading = 0;

const char on = LOW;
const char off = HIGH;

const int LED = LED_BUILTIN;
const int Sensor = A0;



const char* ssid = MYSSID;
const char* password = MYPASS;
const int port = 8080;
WiFiServer server(port);
WiFiClient client;

void setup()
{
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(Sensor, INPUT);
  digitalWrite(LED, off);

  DEBUG_PRINT("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN("WiFi connected");
  server.begin();  // Starts the Server
  DEBUG_PRINTLN("Server started");

  DEBUG_PRINT("IP Address of network: "); // Prints IP address on Serial Monitor
  DEBUG_PRINTLN(WiFi.localIP());
  DEBUG_PRINT("Copy and paste the following URL: http://");
  DEBUG_PRINT(WiFi.localIP());
  DEBUG_PRINT(":");
  DEBUG_PRINT(port);
  DEBUG_PRINTLN("/");
}

void loop()
{
  client = server.available();
  if (!client)
  {
    return;
  }
  DEBUG_PRINTLN("Waiting for new client");
  while (!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  DEBUG_PRINTLN(request);
  client.flush();


  if (request.indexOf("/LEDON") != -1)
  {
    digitalWrite(LED, on); // Turn ON LED
    state = on;
    sensorReading = analogRead(Sensor);
  }
  if (request.indexOf("/LEDOFF") != -1)
  {
    digitalWrite(LED, off); // Turn OFF LED
    state = off;
    sensorReading = analogRead(Sensor);
  }
  if (request.indexOf("/READING") != -1)
  {
    sensorReading = analogRead(Sensor);
  }
  serveWebsite();
}


void serveWebsite() {


  /*------------------HTML Page Creation---------------------*/

  client.println("HTTP/1.1 200 OK"); // standalone web server with an ESP8266
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<head>");
  client.println("<title>LED TOGGLE</title>");
  client.println("</head>");
  client.print("<body>");
  client.println("<header id=\"main-header\">");
  client.println("<h1>LED TOGGLE</h1>");
  client.println("<h4>by Green Shoe Garage</h4>");
  client.println("</header>");

  if (state == off)
  {
    client.print("<font size=\"14\"\" color=\"red\"\">OFF</font>");
  }
  else
  {
    client.print("<font size=\"14\"\" color=\"green\"\">ON</font>");
  }


  client.print("<br>");
  client.print("<br>");
  client.println("<a href=\"/LEDON\"\"><button class=\"button\">ON</button></a>");
  client.println("<a href=\"/LEDOFF\"\"><button class=\"button\">OFF</button></a>");
  client.print("<br>");
  client.print("<br>");

  if (sensorReading > 1)
  {
    client.print("<font size=\"14\"\" color=\"red\"\">Sensor Reading: </font>");
    client.print("<font size=\"14\"\" color=\"red\"\">");
    client.print(sensorReading);
    client.print("</font>");
  }
  else
  {
    client.print("<font size=\"14\"\" color=\"blue\"\">Sensor Reading: </font>");
    client.print("<font size=\"14\"\" color=\"blue\"\">");
    client.print(sensorReading);
    client.print("</font>");
  }

  client.print("<br>");
  client.println("<a href=\"/READING\"\"><button class=\"button\">GET SENSOR READING</button></a>");
  client.print("</body>");
  client.println("</html>");

  delay(1);
  DEBUG_PRINTLN("Client disonnected");
  DEBUG_PRINTLN("");
}
