#include <ESP8266WiFi.h>
#include "TM1637.h"
#include <Ticker.h>
#include <ESP8266HTTPClient.h>

#define CALL_BTN    4
#define RECALL_BTN  5

const char* ssid     = "BTQpap";
const char* password = "penjara666";
const char* host     = "http://192.168.4.200:3000/";
const char* getQue   = "?antrian=a";
const char* setQue   = "?antrian=AN";
const char* recQue   = "?antrian=AR";

IPAddress localip(192,168,4,201);
IPAddress gateway(192,168,4,1);
IPAddress subnet(255,255,255,0);

int CALL = 0;
int RECALL = 0;

tm1637 display(14, 12);
Ticker timer;
bool tock = false;

void tick(void)
{
    tock = true;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
//  Serial.print("Conf WiFi => ");
//  Serial.println(WiFi.config(localip, gateway, subnet) ? "OK" : "NOK");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* IO Setup */
  pinMode(CALL_BTN, INPUT_PULLUP);
  pinMode(RECALL_BTN, INPUT_PULLUP);

  /* 7 Segment */
  display.setBrightness(4);
  display.writeInteger(000);

  timer.attach(2.0, tick);
}


void loop() 
{
  HTTPClient http;
  int httpCode;
  String reqData = "";

  if (tock){
    tock = false;
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Wifi Broke, Recon...");
      WiFi.begin(ssid, password);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
    }    

    /* update number from server */
    /*
    reqData += host;
    reqData += getQue;
    Serial.print("REQ ==> ");
    Serial.println(reqData);
    http.begin(reqData);
    httpCode = http.GET();
    if(httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          display.writeInteger(payload.toInt());
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
*/

  }
  if ((digitalRead(CALL_BTN)) == 0){
    while(digitalRead(CALL_BTN)==0);
    delay(500);
    CALL = 1;  
  }

  if ((digitalRead(RECALL_BTN)) == 0){
    while(digitalRead(RECALL_BTN)==0);
    delay(500);
    RECALL = 1;  
  }

  if (CALL || RECALL) {
    Serial.print("IO ==> ");
    Serial.print(CALL);
    Serial.println(RECALL);

    reqData += host;

    /* send to server */
    if (CALL) {
      reqData += setQue;
    }

    if (RECALL) {
      reqData += recQue;
    }
    
    Serial.print("REQ ==> ");
    Serial.println(reqData);
    http.begin(reqData);
    http.addHeader("Content-Type", "text/plain");
    httpCode = http.POST("");
    if(httpCode > 0) {
      Serial.println("Update OK");
      if(httpCode == HTTP_CODE_OK) {
          String payload = http.getString();
          Serial.println(payload);
          display.writeInteger(payload.toInt());
      }
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
     http.end();
     
    /* reset state */
    CALL = 0;
    RECALL = 0;
  }

}
