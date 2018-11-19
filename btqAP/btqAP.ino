#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

const char *ssid = "BTQpap";
const char *pass = "penjara666";

IPAddress localip(192,168,4,1);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

void setup() 
{
  delay(1000);
  Serial.begin(115200);
  Serial.print("Conf WiFi => ");
  Serial.println(WiFi.config(localip, gateway, subnet) ? "OK" : "NOK");
  WiFi.softAP(ssid, pass);
  IPAddress myIP = WiFi.softAPIP();

  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() 
{
  /* do nothing */
}
