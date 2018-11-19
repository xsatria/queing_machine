#include <DFPlayer_Mini_Mp3.h>
#include <EEPROM.h>
#include "Adafruit_Thermal.h"
#include "tangerang.h"

#define NOL       1
#define SATU      2
#define DUA       3 
#define TIGA      4
#define EMPAT     5
#define LIMA      6
#define ENAM      7
#define TUJUH     8
#define DELAPAN   9
#define SEMBILAN  10
#define NOMOR     11
#define ANTRIAN   12
#define MENUJU    13
#define LOKET_A   14
#define LOKET_B   15
#define ATR_A     16
#define ATR_B     17

#define Audio Serial1
#define PrinterPort Serial2

#define BUT_A     6
#define BUT_B     4
#define BUT_R     2

/* Printer */
Adafruit_Thermal printer(&PrinterPort, 13); 

int Busy = 20;

int urutanA = 0;
int urutanB = 0;

int lastA = 0;
int lastB = 0;

/* EEPROM SAVE */
int addrANT_A = 0;
int addrANT_B = 8;

/* AUDIO CALL */
int callNow = 0;
char lockNow = 0;
int callLast = 0;
char lockLast = 0;
int recallNow = 0;

void playAudio(int NUM)
{
    mp3_play(NUM);
    if (NUM != MENUJU)
      delay(1000);
    else
      delay(2500);
}

int callAntrian(int antrian, char loket)
{
  int LOC;

  int satuan = 0;
  int puluhan = 0;
  int ratusan = 0;

  if (antrian < 10) {
    satuan = antrian;
    puluhan = 0;
    ratusan = 0;
  }

  if ((antrian > 9) && (antrian <= 99)) {
    satuan = antrian % 10;
    puluhan = (antrian/10) % 10;
    ratusan = 0;
  }

  if ((antrian > 99) && (antrian <= 999)) {
    satuan = antrian % 10;
    puluhan = (antrian/10) % 10;
    ratusan = (antrian/100) % 10;
  }

  satuan += 1;
  puluhan += 1;
  ratusan += 1;
    
  if (loket == 'A')
    LOC = LOKET_A;

  if (loket == 'B')
    LOC = LOKET_B;

  /* ANTRIAN */
  playAudio(ANTRIAN);
  playAudio(NOMOR);

  if (loket == 'A')
    playAudio(ATR_A);

  if (loket == 'B')
    playAudio(ATR_B);

  /* NOMOR */
  playAudio(ratusan);
  playAudio(puluhan);
  playAudio(satuan);

  /* MENUJU LOKET */
  playAudio(MENUJU);

  /* LOKET */
  playAudio(LOC);
  
}

void setup () {
  delay(100);
	Serial.begin (9600);

  /* Printer Setup */
  PrinterPort.begin(9600);
  printer.begin(160);
  
  /* Audio Setup */
  Audio.begin(9600);
	mp3_set_serial (Audio);
  mp3_set_debug_serial(Serial);
	mp3_set_volume (25);
  
  /* Busy Pins */
  pinMode(Busy, INPUT);
  digitalWrite(Busy, HIGH);

  /* Buttons */
  pinMode(6, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);

  /* last queue */
  urutanA = EEPROM.read(addrANT_A);
  urutanB = EEPROM.read(addrANT_B);
}


void printAntrian(int noAntrian, char loket)
{
  String noAnt = "";
  noAnt += loket;
  if (noAntrian < 10)
    noAnt += "00";
  if ((noAntrian >= 10) && (noAntrian < 99))
    noAnt += "0";
  noAnt += String(noAntrian, DEC);
  printer.justify('C');
  printer.printBitmap(tangerang_width, tangerang_height, tangerang_data);
  printer.justify('C');
 // printer.feed(1);
  printer.println(F("-----------------------"));
  printer.println(F("NOMOR ANTRIAN ANDA"));
  printer.feed(1);
  printer.doubleHeightOn();
  printer.setSize('L');        // Set type size, accepts 'S', 'M', 'L'
  printer.println(noAnt);
  printer.setSize('S');
  printer.doubleHeightOff();
  printer.feed(3);
  printer.println(F("Terima Kasih"));
  printer.println(F("Atas Kedatangan Anda"));
  printer.feed(3);
  printer.cutPaper(HALF_CUT);
}

void loop () {    
  int iter;    
  char locArr[10];
  
  if (Serial.available()) {
    String data = Serial.readStringUntil(0x0A);
    String callNowTmp = getValue(data, '#', 0);
    String lockNowTmp = getValue(data, '#', 1);
    String recallTmp = getValue(data, '#', 2);

    if (recallNow != "") {
      recallNow =  recallTmp.toInt();
    }
    
    if (callNowTmp != "") {
      callNow = callNowTmp.toInt();  
    }

    if (lockNowTmp != "") {
      lockNowTmp.toCharArray(locArr, 10);
      lockNow = locArr[0];   
    }
  }

  /* resets the eeprom */
  if (digitalRead(BUT_R)==0)
  {
    delay(100);
    EEPROM.write(addrANT_A, 0);
    EEPROM.write(addrANT_B, 0);
    urutanA = 0;
    urutanB = 0;
    Serial.write('#');
  }

  if (digitalRead(BUT_A)==0)
  {
    delay(100);
    printAntrian(urutanA,'A');
    urutanA++;  
    EEPROM.update(addrANT_A, urutanA);
  }

  if (digitalRead(BUT_B)==0)
  {
    delay(100);
    printAntrian(urutanB,'B');
    urutanB++;  
    EEPROM.update(addrANT_B, urutanB);
  }
  
  if ((callNow != callLast) || (lockNow != lockLast) || (recallNow == 1)) {
    callAntrian(callNow, lockNow);
    callLast = callNow;
    lockLast = lockNow;
    recallNow = 0;
  }
  
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
