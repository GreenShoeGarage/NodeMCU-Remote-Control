//Based on work by https://github.com/hansjny

#include <ESP8266WiFi.h>
#include "secrets.h"

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

  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  server.begin();  // Starts the Server
  Serial.println("Server started");

  Serial.print("IP Address of network: "); // Prints IP address on Serial Monitor
  Serial.println(WiFi.localIP());
  Serial.print("Copy and paste the following URL: http://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.print(port);
  Serial.println("/");
}

void loop()
{
  client = server.available();
  if (!client)
  {
    return;
  }
  Serial.println("Waiting for new client");
  while (!client.available())
  {
    delay(1);
  }

  String request = client.readStringUntil('\r');
  Serial.println(request);
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
  client.print("<font size=\"14\"\" color=\"blue\"\">Sensor Reading: </font>");
  client.print("<font size=\"14\"\" color=\"blue\"\">");
  client.print(sensorReading);
  client.print("</font>");
  client.print("<br>");
  client.println("<a href=\"/READING\"\"><button class=\"button\">GET SENSOR READING</button></a>");
  client.print("</body>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
