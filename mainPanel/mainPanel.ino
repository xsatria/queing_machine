#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Ticker.h>
#include <DMD2.h>
#include <fonts/Arial_Black_16.h>
#include <fonts/SystemFont5x7.h>

#define pin_A     16
#define pin_B     12
#define pin_sclk  5
#define pin_clk   14
#define pin_r     13
#define pin_noe   15

#define DISPLAYS_WIDE 2
#define DISPLAYS_HIGH 1

SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH, pin_noe, pin_A, pin_B, pin_sclk);
DMD_TextBox box(dmd, 0, 0, 64, 16);

const char* ssid     = "BTQpap";
const char* pass     = "penjara666";
const char* host     = "http://192.168.4.200:3000/?";
const char* getQue   = "antrian=x";

/* flag for new queue */
bool dispUpdated = 0;
int lastQue = 0;
String lastLoc = "";

int RESTcount = 0;

//IPAddress localip(192,168,4,203);
//IPAddress gateway(192,168,4,1);
//IPAddress subnet(255,255,255,0);

/* timer updater */
Ticker timer;
bool tock = false;

void setup() {
  delay(1000);
  /* Serial Init */
  Serial.begin(115200);
  
  /* IP init */
  WiFi.mode(WIFI_OFF);   
  delay(500);
  Serial.println();
  //Serial.print("Conf WiFi => ");
  //Serial.println(WiFi.config(localip, gateway, subnet) ? "OK" : "NOK");
  Serial.print("Sets WiFi STA => ");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  /* Matrix Init */
  dmd.begin();
  dmd.setBrightness(127);
  dmd.clearScreen();

  timer.attach(1.5, tick);
}

void loop() 
{

  /* check newer QUE */
  if (tock){
    HTTPClient http;
    int httpCode;
    tock = false;
    
    String reqData = "";
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
        String LOC = getValue(payload,'#',1);
        Serial.println(LOC);
        int QUE = getValue(payload,'#',0).toInt();
        String dataDisp = Konversi(QUE);
        if ((QUE != lastQue) || (LOC != lastLoc)){
          dmd.clearScreen();
          dmd.selectFont(Arial_Black_16);
          dmd.drawString(2, 1, dataDisp);
          dmd.selectFont(SystemFont5x7);
          dmd.drawString(33, 5, F("==>"));
          dmd.selectFont(Arial_Black_16);
          dmd.drawString(52, 1, (LOC));
          lastQue = QUE;
          lastLoc = LOC;
        } else {
          Serial.println("Queue Same | Not Updating");  
        }
      }
    }  else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        RESTcount++;
        if (RESTcount > 2) {
          ESP.restart();          
        }
    }
    http.end();
  }
  
}

void tick(void)
{
    tock = true;
}

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

String Konversi(int sInput) {
  if (sInput < 10)
  {
    return"00" + String(sInput, DEC);
  }
  
  if ((sInput > 9) && (sInput <= 99))
  {
    return"0" + String(sInput, DEC);
  }

  if (sInput > 99)
  {
    return String(sInput, DEC);
  }
  
}
