#include <Arduino.h>
#include <BluetoothSerial.h>
#include <LoRa.h>
#include <SPI.h>
#include <HardwareSerial.h>
#include <Preferences.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <TinyGPS++.h>
#include <axp20x.h>
#include <maidenhead.h>
#include <OneButton.h>


const unsigned char satellite_dish [] PROGMEM = {
	0x00, 0x00, 0x02, 0x00, 0x03, 0x00, 0x07, 0x00, 0x07, 0x98, 0x07, 0xb8, 0x07, 0xc0, 0x07, 0xc0, 
	0x03, 0xe0, 0x01, 0xe0, 0x02, 0xf0, 0x07, 0x80, 0x0f, 0xc0, 0x0f, 0xc0, 0x00, 0x00
};

//Definisco l'interfaccia per il modulo GPS

#define GPS_SERIAL    1
#define GPS_RX        34
#define GPS_TX        12

//Definisco l'interfaccia Lora per TBeam
#define LORA_MOSI     27
#define LORA_SCLK     5
#define LORA_CS       18
#define LORA_DIO      26
#define LORA_REST     23
#define LORA_MISO     19

//Definisco l'interfaccia OLED I2C

#define OLED_SDA      21
#define OLED_SCL      22
#define OLED_WIDTH    128
#define OLED_HEIGHT   64


#define bottone       38





BluetoothSerial BT;

Preferences pref;

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire);

TinyGPSPlus gpsmodule;

String comando = "";

HardwareSerial GPS(1);

AXP20X_Class axp;



//Dati di primo avvio:

String qrz = "IU7IGU-10";
const char* beacon_message = "HamLoraChat info: www.github.com/iu7igu/HamLoraChat";
const bool beacon = false;
const int beacon_time = 5;  // In minuti
const bool gps = true;
const long lora_freq = 433775000;
const int lora_power = 20;
const int lora_spread = 12;
const float lora_bandw = 125000;
const bool oled = true; 
const char* locatore = "JN70BN";
const bool diretta = false;


bool newmex = false;
String messaggio = "";

unsigned long primabeacon = 0;
unsigned long primalcd = 0;

void updatelcd(){
  if(pref.getBool("oled")){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false);
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(30,0);
    display.print("HamLoraChat");
    display.setCursor(10,14);
    display.print(qrz);
    display.setCursor(10,25);
    if (pref.getBool("gps", false)){
      display.print("GPS= ON");
      display.setCursor(10,37);
      display.println("SAT= " + String(gpsmodule.satellites.value()));
    }else if (!pref.getBool("gps", false)){
      display.print("GPS= OFF");
      display.setCursor(10,37);
      display.println("SAT= 0");
    }

    display.setCursor(10, 49);
    if (pref.getBool("beacon", false)){
      display.print("BEACON= ON");
    }else if (!pref.getBool("beacon", false)){
      display.print("BEACON= OFF");
    }

    if(axp.isBatteryConnect()){
      display.setCursor(80, 14);
      display.print(String(axp.getBattVoltage(), 0) + "V");
      if(axp.getBattChargeCurrent() > 0){
        display.setCursor(80, 25);
        display.print(String(axp.getBattChargeCurrent() , 0) + "mA");
      }else{
      display.setCursor(80, 25);
      display.print(String(axp.getBattDischargeCurrent() * -1, 0) + "mA");
      }
    }
    
    if(gpsmodule.location.isValid()){
      display.drawBitmap(95, 45, satellite_dish, 15, 15, WHITE);
    }
    

    display.display();
  }
  else{
    display.clearDisplay();
  }
}


void beacon_tx(){
    if (gpsmodule.location.isValid() || gpsmodule.satellites.value() > 0){
      String posizione;
      float lati, lon;
      lon = gpsmodule.location.lng();
      lati = gpsmodule.location.lat();
      posizione = String(lati, 6) + "#" + String(lon, 6);
      LoRa.beginPacket();
      LoRa.print("beacon#" + qrz + "#" + posizione + "#" + pref.getString("beacon_message"));
      LoRa.endPacket(true);
      display.setCursor(80,49);
      display.println("TX");
      display.display();
    }}


void initvolt(){
        
      axp.setPowerOutPut(AXP192_LDO2, AXP202_ON); //Alimentazione LORA
      axp.setPowerOutPut(AXP192_LDO3, AXP202_ON); //Alimentzione GPS
      axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON); //Alimentazione OLED
      axp.adc1Enable(AXP202_BATT_CUR_ADC1, 1);
      axp.setChargeControlCur(AXP1XX_CHARGE_CUR_100MA);
      
}


void modulogps(){
  if(pref.getBool("gps", false)){
        axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
        //Serial.print("GPS ON");
    }
    else{
      axp.setPowerOutPut(AXP192_LDO3, AXP202_OFF); // Spengo il gps
      //Serial.print("GPS OFF");
    }
}

boolean isNumeric(String str) {
    unsigned int stringLength = str.length();
 
    if (stringLength == 0) {
        return false;
    }
 
    boolean seenDecimal = false;
 
    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }
 
        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}

String splitta(String data, char separator, int index)
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


void setup() {
  Serial.begin(9600);
  BT.begin("HamLoraChat");
  pref.begin("parametri", false);
  GPS.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
      //Serial.println("AXP192 Begin PASS");
  } else {
      //Serial.println("AXP192 Begin FAIL");
      axp.setDCDC1Voltage(3300);
    }

  initvolt();

  
if (!pref.getBool("avvio")){  
    pref.putBool("avvio", true);
    //pref.putString("qrz", qrz);
    pref.putString("beacon_message", beacon_message);
    pref.putBool("beacon", beacon);
    pref.putInt("beacon_time", beacon_time);
    pref.putBool("gps", gps);
    pref.putLong("lora_freq", lora_freq);
    pref.putInt("lora_power", lora_power);
    pref.putInt("lora_spread", lora_spread);
    pref.putFloat("lora_bandw", lora_bandw);
    pref.putBool("oled", oled);
    pref.putString("locatore", locatore);
  }

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_REST, LORA_DIO);
  LoRa.setSpreadingFactor(pref.getInt("lora_spread"));
  LoRa.setTxPower(pref.getInt("lora_power"));
  LoRa.setSignalBandwidth(pref.getFloat("lora_bandw"));
  LoRa.begin(pref.getLong("lora_freq"));
  
  modulogps();

  updatelcd();
}


void loop() {

  while(GPS.available() > 0){
    gpsmodule.encode(GPS.read());
  }

  unsigned long start = millis();

  if (pref.getBool("beacon", false)){
    if (start - primabeacon >= pref.getInt("beacon_time") * 60000){
      primabeacon = start;
      beacon_tx();
    }
  }

  if(pref.getBool("oled", false)){
    if (start - primalcd >= 2000){
      primalcd = start;
      updatelcd();
    }
  }

  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String stringa = "";
    // read packet
    while (LoRa.available()) {
      stringa = LoRa.readString();
    }
    if(splitta(stringa, '#', 1) =! qrz){          //    chat#IU7IGU-10#lat#log#messaggio#QRZGTW

      if(splitta(stringa, '#', 0) == "diretta"){  //    diretta#IU7IGU-10#lat#log#messaggio

          BT.println("DIRETTA >"+ splitta(stringa, '#', 1) + ":"+ splitta(stringa, '#', 2) + "#" + splitta(stringa, '#', 3) + "> "+ splitta(stringa, '#', 4));

      }else if ( splitta(stringa, '#', 0) == "chat" and  splitta(stringa, '#', 5) != ""){
          BT.println(splitta(stringa, '#', 5) + ">" +  splitta(stringa, '#', 1) + ">> " + splitta(stringa, '#', 4));
      }
  }}

  while(BT.available()){
        
        pref.begin("parametri", false);
        String parametro = "";
        String posizione = "";
        comando = BT.readStringUntil('\n');
        comando.remove(comando.length()-1, 1);
        
        if (comando == "#menu"){
          BT.println("Menu di configurazione");
          BT.println("/gps on/off : Attiva#disattiva il gps");
          BT.println("/beacon on/off : Attiva#disattiva beacon");
          BT.println("/beacontime 5 : Modifica minuti delay beacon");
          BT.println("/freq 433775000 : Modifica frequenza Lora(richiede riavvio)");
          BT.println("/locatore JN10BN : Modifica il locatore");
          BT.println("/direct on/off : Abilita o disattiva la modalita diretta");
          BT.println("/stato : Stato del dispositivo");
        }
        else if(comando == "/gps on"){
          pref.putBool("gps", true);
          BT.println("GPS attivato");
          modulogps();
        }
        else if(comando == "/gps off"){
          pref.putBool("gps", false);
          BT.println("GPS disattivato");
          modulogps();
        }
        else if(comando == "/direct on"){
          pref.putBool("diretta", true);
          BT.println("Diretta attivata");
        }
        else if(comando == "/direct off"){
          pref.putBool("diretta", false);
          BT.println("Diretta disattivata");
        }
        else if(comando.indexOf("/freq") != -1){
          String frequenza = splitta(comando, ' ', 1);
          if (isNumeric(frequenza)){
              char freq[20] ;
              frequenza.toCharArray(freq, 20);
              pref.putLong("lora_freq", atol(freq));
              BT.println("Frequenza impostata su " + frequenza);
          }
        }
        else if (comando == "/beacon on"){
          pref.putBool("beacon", true);
          BT.println("Beacon attivata");
        }
        else if(comando == "/beacon off"){
          pref.putBool("beacon", false);
          BT.println("Beacon disattivata");
        }
        else if(comando.indexOf("/beacontime") != -1){
          String bctime = splitta(comando, ' ', 1);
          if (isNumeric(bctime)){
              pref.putFloat("beacon_time", bctime.toFloat());
              BT.println("Beacon time " + bctime + " minuti");
          }
        }
        else if (comando.indexOf("#locatore") != -1){
          String locatore = splitta(comando, ' ', 1);
          if(isDigit(locatore.charAt(2)) and isDigit(locatore.charAt(3))){
              pref.putString("locatore", locatore);
              BT.println("Locatore impostato " + locatore);
          }
        }
        else if (comando == "/stato"){
           if (pref.getBool("gps", false)){
            BT.println("GPS: ON");
          }
          else{
            BT.println("GPS: OFF");
          }
          BT.println("Satelliti GPS: " + String(gpsmodule.satellites.value()));
          BT.println("Coordinate Lat: " + String(gpsmodule.location.lat())+ "Lon: " + String(gpsmodule.location.lng()));
          if (pref.getBool("beacon", false)){
            BT.println("Beacon: ON");
          }
          else{
            BT.println("Beacon: OFF");
          }
          BT.println("Beacon delay: " + String(pref.getInt("beacon_time", false)));
          BT.println("Diretta: " + String(pref.getBool("diretta", false)));
          BT.println("Frequenza: " + String(pref.getLong("lora_freq", false)));
          BT.println("Locatore: " + pref.getString("locatore"));
        }
        else{
            float lati;
            float lon;
            if (pref.getBool("gps", false)){
            if (!gpsmodule.location.isValid()){
              lati = 0;
              lon = 0;
            }else{
              lon = gpsmodule.location.lng();
              lati = gpsmodule.location.lat();
            }
            posizione = String(lati, 6) + "#" + String(lon, 6);
          }else if (!pref.getBool("gps", false) ){
            char locat[10];
            pref.getString("locatore").toCharArray(locat, 20);
            posizione = String(mh2lat(locat), 6) + "#" +  String(mh2lon(locat), 6);
          }
          LoRa.beginPacket();
          if(pref.getBool("diretta", false)){
              LoRa.print("diretta#" + qrz + "#" + posizione + "#" + comando);
          }
          else{
              LoRa.print("chat#" + qrz + "#" + posizione + "#" + comando);
          }
          LoRa.endPacket(true);
          //Serial.println(comando);
          BT.flush();
        }
  }



}
