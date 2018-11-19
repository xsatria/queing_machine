#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <Ticker.h>

const char* ssid     = "BTQpap";
const char* password = "penjara666";

//IPAddress localip(192,168,4,200);
//IPAddress gateway(192,168,4,1);
//IPAddress subnet(255,255,255,0);

ESP8266WebServer server(3000);

Ticker timer;
bool tock = false;

int ATR_A = 0;
int ATR_B = 0;
char LOC = 'A';

int addrATR_A = 0;
int addrATR_B = 8;
char addrLOC = 16;

void handlePOST() 
{
//Serial.println("POST Received");

/* check incoming arguments */
/* POST AN */
if (server.arg("antrian")=="AN") {
  ATR_A++;
  if (ATR_A > 999)
    ATR_A = 0;
  LOC = 'A';
  String msg = String(ATR_A, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("POST RCV ==> ");
  msg += "#";
  msg += LOC;
  Serial.println(msg);        
  EEPROM.put(addrATR_A, ATR_A);
  EEPROM.put(addrLOC, LOC);
  EEPROM.commit();
}

/* POST AN */
if (server.arg("antrian")=="AR") {
  LOC = 'A';
  String msg = String(ATR_A, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("POST A RECALL");
  msg += "#";
  msg += LOC;
  msg += "#";
  msg += "1";
  Serial.println(msg);        
}

/* POST AN */
if (server.arg("antrian")=="BN") {
  ATR_B++;
  if (ATR_B > 999)
    ATR_B = 0;
  LOC = 'B';
  String msg = String(ATR_B, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("POST RCV ==> ");
  msg += "#";
  msg += LOC;
  Serial.println(msg);        
  EEPROM.put(addrATR_B, ATR_B);
  EEPROM.put(addrLOC, LOC);
  EEPROM.commit();
}

/* POST AN */
if (server.arg("antrian")=="BR") {
  LOC = 'B';
  String msg = String(ATR_B, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("POST B RECALL");
  msg += "#";
  msg += LOC;
  msg += "#";
  msg += "1";
  Serial.println(msg);        
}

/* END */
}

void handleGET() 
{
      
/* check incoming arguments */
/* GET A */
if (server.arg("antrian")=="a") {
  String msg = String(ATR_A, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("GET RCV ==> ");
//  Serial.println(msg);        
}

if (server.arg("antrian")=="b") {
  String msg = String(ATR_B, DEC);
  server.send(200, "text/plain", msg);        
//  Serial.print("GET RCV ==> ");
//  Serial.println(msg);        
}

if (server.arg("antrian")=="x") {
  String msgx = "";
  if (LOC == 'A') {
    msgx += String(ATR_A, DEC);
    msgx += "#";
    msgx += LOC;  
  }
  if (LOC == 'B') {
    msgx += String(ATR_B, DEC);
    msgx += "#";
    msgx += LOC;  
  }
  server.send(200, "text/plain", msgx);        
//  Serial.print("GET RCV ==> ");
  Serial.println(msgx);        
}

}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  delay(1000);
  Serial.begin(9600);
  delay(10);

  WiFi.mode(WIFI_OFF);
  delay(500);
//  Serial.println();
//  Serial.print("Conf WiFi => ");
  //Serial.println(WiFi.config(localip, gateway, subnet) ? "OK" : "NOK");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
//    Serial.print(".");
  }

//  Serial.println("");
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());

  /* Server Handler */
  server.on("/", HTTP_GET, handleGET);
  server.on("/", HTTP_POST, handlePOST);
  server.onNotFound(handleNotFound);

  /* Server Initial */
  server.begin();
//  Serial.println("HTTP server started");

  timer.attach(1.0, tick);
  
  EEPROM.begin(512);
  
  /* read last value */
  ATR_A = EEPROM.read(addrATR_A);
  ATR_B = EEPROM.read(addrATR_B);
  LOC = EEPROM.read(addrLOC);
}


void loop() 
{
  if (Serial.available()) {
    char rst = Serial.read();
    if (rst == '#') {
      EEPROM.put(addrATR_A, 0);
      EEPROM.put(addrATR_B, 0);
      EEPROM.put(addrLOC, 'A');
      EEPROM.commit();
      ATR_A = 0;
      ATR_B = 0;
      LOC = 'A';
    }
  }
  
  server.handleClient();
  if (tock){
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wifi Broke, Recon...");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
  //      Serial.print(".");
      }
    }
    tock = false;
  }
}

void tick(void)
{
    tock = true;
}
