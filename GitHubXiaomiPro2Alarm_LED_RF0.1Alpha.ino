/*
## Alarmanlage mit RGB Steuerung und RF Fernbedienung.
##
## RF Empfänger (SRX882), WS2812B Stripe, Gyroskop GY-521 MPU 6050, Piezo (LAUT), Wemos D1 Mini, ...
## A = 0xBAFC21
## B = 0xBAFC22
## C = 0xBAFC28
## D = 0xBAFC24
##  RF PIN = D5 = 14
##  WS2812 PIN = D6 = 12
##  Piezo PIN = D7 = 13
##  GY-521 = I2C = D1, D2 = 5, 4
## 
## RF Fernbedienung; 4 Funktionen: Alarm EIN, Alarm AUS, LED ON/SET/OFF, Hotspot ON/OFF (Codes: ... )
##
##
## LED 0-18 = Links 19-45 = Rechts // insg 45stk.
##
##

-------------------------------------------------
########### Pro2 RGB Alarm Alpha 0.1 ###########
-------------------------------------------------

*/

#include <RCSwitch.h>
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "myTypes.h"
#include "LEDmodes.h"
#include "gyroscope.h"
#include "OTA.h"
 

#define RF_PIN      14     // Receiver on interrupt 0 => that is pin #2

#define piezopin 2//13


int addr = 0;                 // Adresse für EEPROM
int Programm;
int modus = 0;    // Currently-active animation mode, 0-9
int i2;
int geradeEmpfangen;

unsigned long timerblinkaktuell;
unsigned long timerblinkneu;
unsigned long timerRFaktuell;
unsigned long timerRFneu;

unsigned long ulReqcount;

//---------------------------------------------------------------
RCSwitch mySwitch = RCSwitch();
//---------------------------------------------------------------
//---------------------------------------------------------------

//---------------------------------------------------------------
WiFiServer server(80);
//---------------------------------------------------------------


void setup() {

  modus=0;
  data.LEDState = 5;
  Serial.begin(9600);
  //---------------------------------------------------------------
  mySwitch.enableReceive(RF_PIN);  
  //---------------------------------------------------------------
  pixels.begin(); // Initialisierung der NeoPixel
  clearStrip();
  //---------------------------------------------------------------
  Wire.begin();
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0); // wake up!
  Wire.endTransmission(true);
  
  //---------------------------------------------------------------
  //EEPROM.begin(512);
  //EEPROM.get(addr,data);      // werte aus dem EEPROM wiederherstellen
  //modus = data.modus;
          Grenzw_ACCEL_GradX = Altern_ACCEL_GradX;
          Grenzw_ACCEL_GradY = Altern_ACCEL_GradY;
          Grenzw_GYRO_BesX   = Altern_GYRO_BesX;
          Grenzw_GYRO_BesY   = Altern_GYRO_BesY;
          Grenzw_GYRO_BesZ   = Altern_GYRO_BesZ;
  //---------------------------------------------------------------
  pinMode(piezopin, OUTPUT);
  digitalWrite(piezopin, LOW);
  Serial.println("Moin");
  HotspotOFF();
}




void rfEmpfangen()  {

}



void rfAbrufen()  {
  if (geradeEmpfangen == 1)  {
    timerRFaktuell = millis();
    if (timerRFaktuell - timerRFneu >= 2000) { 
      geradeEmpfangen = 0;
    }
  }
  else {
    Serial.println("RF Loop");
    if (mySwitch.available()) {
      
      float value = mySwitch.getReceivedValue(); // Empfangene Daten werden unter der Variable "value" gespeichert.
      
      if (value == 0) {
        Serial.print("Unknown encoding");
      } else {
        Serial.print("Received ");
        Serial.print( mySwitch.getReceivedValue() );
        Serial.print(" / ");
        Serial.print( mySwitch.getReceivedBitlength() );
        Serial.print("bit ");
        Serial.print("Protocol: ");
        Serial.println( mySwitch.getReceivedProtocol() );
      }
  
      if (value == 123456) {
        piezoLED();
      }
      //---------------------------------------------------------------
      if (value == 12254242) {   // Sperren Taste
        geradeEmpfangen = 1;
        timerRFneu = millis();
        Serial.println("Sperren");
        piezoLED();
        LEDONAlarm();
        modus = 1;
        GyroskopAuslesen();
        FormatMpu6050();
        LageSichern();
         
        
      }
      //---------------------------------------------------------------
      if (value == 12254241) { //  Öffnen Taste
        geradeEmpfangen = 1;
        timerRFneu = millis();
        Serial.println("Öffnen");
        piezoLED();
        LEDOFFAlarm();
        modus = 0;
         
      }
      //---------------------------------------------------------------
      if (value == 12254248) { //  Glocken Taste
        geradeEmpfangen = 1;
        timerRFneu = millis();
        Serial.println("Glocke");
        piezoLED();
        if(modus==4)  {
          Serial.println("HotSpot OFF");
          HotspotOFF();
          modus = 0;
        }
        else if(modus==2)  {
          Serial.println("LEDState++");
          data.LEDState++;
          if (data.LEDState >=19)  {
            data.LEDState = 1;
          }
        }
        else  {
          Serial.println("Hotspot ON");
          HotspotON();
          modus = 4;
          data.LEDState = 15;
        }
        
      }
      //---------------------------------------------------------------
      if (value == 12254244) { //  Kofferraum Taste
        geradeEmpfangen = 1;
        timerRFneu = millis();
        Serial.println("KR Taste");
        piezoLED();
        if (modus == 2) {
          Serial.println(">>Modus 0");
          modus = 0;
        }
        else if (modus == 4) {
        }
        else  {
          Serial.println(">>Modus 2");
        modus = 2;
        }
         
      }
      //---------------------------------------------------------------
      mySwitch.resetAvailable();
      delay(300);
    }
    delay(60);
  }
}

void LEDONAlarm() {
  i2=19;
  for( int i = 0; i<NUMPIXELS-1; i++) {
    pixels.setPixelColor(i, pixels.Color(0,250,0));
    pixels.setPixelColor(i2, pixels.Color(0,250,0));
    pixels.show();
    delay(50);
    i2++;
    if (i>=23){break;}
    
  }
  clearStrip();
}

void LEDOFFAlarm() {
  i2=36;
  for( int i = 0; i<NUMPIXELS-9; i++) {
    pixels.setPixelColor(i, pixels.Color(250,0,0));
  }
  pixels.show();
  
  for( int i = 18; i>0; i--) {
  pixels.setPixelColor(i, pixels.Color(0,0,0));
  pixels.setPixelColor(i2, pixels.Color(0,0,0));
  pixels.show();
  delay(50);
  i2--;
  }
  clearStrip();
}

void Alarm() {
  if (modus == 1 && AlarmStatus == 1) {
    for( int i = 0; i<1; i++) {
      for( int i = 0; i<NUMPIXELS-1; i++) {
        pixels.setPixelColor(i, pixels.Color(0,0,255));
      
        pixels.show(); // Durchführen der Pixel-Ansteuerung
      }
        delay(10);
        clearStrip();
      }

      for( int i = 0; i<1; i++) {
      for( int i = 0; i<NUMPIXELS-1; i++) {
        pixels.setPixelColor(i, pixels.Color(0,255,0));
      
        pixels.show(); // Durchführen der Pixel-Ansteuerung
      }
        delay(10);
        clearStrip();
      }

      for( int i = 0; i<1; i++) {
      for( int i = 0; i<NUMPIXELS-1; i++) {
        pixels.setPixelColor(i, pixels.Color(0,0,255));
      
        pixels.show(); // Durchführen der Pixel-Ansteuerung
      }
        delay(10);
        clearStrip();
      }
    digitalWrite(piezopin, HIGH);
    delay(10);
    digitalWrite(piezopin, LOW);
    
  }
}
void AlarmScharfLED() {
  // millis bla bla für 20 sek.
  timerblinkaktuell = millis();
  if (timerblinkaktuell - timerblinkneu >= 20000) { 
    timerblinkneu = timerblinkaktuell;
    pixels.setPixelColor(0, pixels.Color(20,0,0));
    pixels.setPixelColor(25, pixels.Color(20,0,0));
    pixels.show();
    delay(30);
    pixels.setPixelColor(0, pixels.Color(0,0,0));
    pixels.setPixelColor(25, pixels.Color(0,0,0));
    pixels.show();
  }
}

void piezoLED()  {
  for( int i = 0; i<NUMPIXELS-1; i++){
    pixels.setPixelColor(i, pixels.Color(0,250,0)); pixels.show();
  } 
  digitalWrite(piezopin, HIGH);
  delay(100);
  digitalWrite(piezopin, LOW);
  clearStrip();
}


void loop() {
  rfAbrufen();
      switch(modus) {           // Start the new animation...
        case 0:     // Ruhe
          delay(200);
          //ESP.deepSleep(500);
          break;
        case 1:     // Alarm scharf
          AlarmScharfLED();
          GyroskopAuslesen();
          FormatMpu6050();
          Alarmauswertung();
          Alarmausloesung();
          Alarm();
          break;
        case 2:     // LED Bel.
          LEDModes();
          break;
        case 3:     // ALARM
          Alarm();
          break;
        case 4:     // Hotspot
          website();
          LEDModes();
          break;  
        case 5:     // OTA + Web
          //website();
          ArduinoOTA.handle();
          delay(300);
          Serial.println("OTA Loop");
          break;  
      }
    }


void HotspotON() {
  WiFi.disconnect();
  delay(200);
  WiFi.mode(WIFI_AP);
  WiFi.softAP("LED", "12345678");
  server.begin();
}

void HotspotOFF() {
  Serial.println("Disconnect AP!");
      WiFi.disconnect();
      delay(50);
      WiFi.mode(WIFI_STA);
      //WiFi.mode(WIFI_OFF);
}









void website()
  {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
  
  // Wait until the client sends some data
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  if(millis()>ultimeout) 
  { 
    Serial.println("client connection time-out!");
    return; 
  }
  
  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  //Serial.println(sRequest);
  client.flush();
  
  // stop client, if request is empty
  if(sRequest=="")
  {
    Serial.println("empty request! - stopping client");
    client.stop();
    return;
  }
  
  // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath="",sParam="", sCmd="";
  String sGetstart="GET ";
  int iStart,iEndSpace,iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart>=0)
  {
    iStart+=+sGetstart.length();
    iEndSpace = sRequest.indexOf(" ",iStart);
    iEndQuest = sRequest.indexOf("?",iStart);
    
    // are there parameters?
    if(iEndSpace>0)
    {
      if(iEndQuest>0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart,iEndQuest);
        sParam = sRequest.substring(iEndQuest,iEndSpace);
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart,iEndSpace);
      }
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it
  ///////////////////////////////////////////////////////////////////////////////
  if(sParam.length()>0)
  {
    int iEqu=sParam.indexOf("=");
    if(iEqu>=0)
    {
      sCmd = sParam.substring(iEqu+1,sParam.length());
      Serial.print("Kommando :");
      Serial.println(sCmd);
      Serial.println(sCmd.indexOf("handynummer"));
    }
  }
  
  
  ///////////////////////////
  // format the html response
  ///////////////////////////
  String sResponse,sHeader;
  
  ////////////////////////////
  // 404 for non-matching path
  ////////////////////////////
  if(sPath!="/")
  {
    sResponse="<html><head><title>404 Not Found</title></head><body><h1>Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
    
    sHeader  = "HTTP/1.1 404 Not found\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  ///////////////////////
  // format the html page
  ///////////////////////
  else
  {
    ulReqcount++;
    sResponse  = "<html><head><title>CrazyTuners LED Unterbobel</title></head><body>";
    sResponse += "<font color=\"#000000\"><body bgcolor=\"#d0d0f0\">";
    sResponse += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=yes\">";
    sResponse += "<h1>CrazyTuners LED Unterbodenbeleuchtung</h1>";
    sResponse += "<br>";
    sResponse += "<FONT SIZE=+2>";
    sResponse += "<p><a href=\"?pin=LEDProg01\"><button>[1] Gruen lauf</button></a>";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg02\"><button>[2] Gruen lauf breit</button></a>";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg03\"><button>[3] weis lauf breit</button></a>";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg04\"><button>[4] Gruen</button></a>";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg05\"><button>[5] Gelb</button></a>";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg06\"><button>[6]</button></a> Tuerkies";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg07\"><button>[7]</button></a> Blau";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg08\"><button>[8]</button></a> Rot";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg09\"><button>[9]</button></a> Pink";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg10\"><button>[10]</button></a> Gruen lauf";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg11\"><button>[11]</button></a> Gruen lauf";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg12\"><button>[12]</button></a> Theater Grun";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg13\"><button>[13]</button></a> Theater Pink";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg14\"><button>[14]</button></a> Sparkle langsam";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg15\"><button>[15]</button></a> Sparkle schnell";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg16\"><button>[16]</button></a> RGB";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg17\"><button>[17]</button></a> Pink schnelllauf";
    sResponse += "</p>";
    sResponse += "<p><a href=\"?pin=LEDProg18\"><button>[18]</button></a> Feuer";
    sResponse += "</p>";
    sResponse += "<br>";
    sResponse += "<br>";
    sResponse += "<p><a href=\"?pin=OTAon\"><button>OTA ON</button></a>";
    sResponse += "<a href=\"?pin=OTAoff\"><button>WiFi/OTA OFF</button></a><br>";
    sResponse += "</p><br>";
    sResponse += "<p><a href=\"?pin=ALLOFF\"><button>>>ALL OFF<<</button></a>";
    sResponse += "<a href=\"?pin=RESET\"><button>Reset</button></a>";
    sResponse += "</p>";
    //sResponse += "<p><a href=\"?pin=Wifion\"><button>WiFi ON</button></a>";
    //sResponse += "<p><a href=\"?pin=OTAoff\"><button>WiFi OFF</button></a>";
    //sResponse += "</p>";
    
/*    
    sResponse += "<p><form method='get'>";
    sResponse += "HotSpot ID: ";
    sResponse += "<input type='text' name='pin=ssid' value='";
    sResponse += data.ssid;
    sResponse += "'><br></form></p>";
    
    sResponse += "<p><form method='get'>";
    sResponse += "HotSpot PW: ";
    sResponse += "<input type='text' name='password' value='";
    sResponse += data.password;
    sResponse += "'><br></form></p>";
*/

    
    sResponse += "<br><br>";
    //////////////////////
    // react on parameters
    //////////////////////
    if (sCmd.length()>0)
    {
      // write received command to html page
      sResponse += "Kommando:" + sCmd + "<BR>";


      // switch GPIO

      
      if(sCmd.indexOf("LEDProg01")>=0)
      {
        data.LEDState = 1; 
      }
      else if(sCmd.indexOf("LEDProg02")>=0)
      {
        data.LEDState = 2; 
      }
      else if(sCmd.indexOf("LEDProg03")>=0)
      {
        data.LEDState = 3; 
      }
      else if(sCmd.indexOf("LEDProg04")>=0)
      {
        data.LEDState = 4; 
      }
      else if(sCmd.indexOf("LEDProg05")>=0)
      {
        data.LEDState = 5; 
      }
      else if(sCmd.indexOf("LEDProg06")>=0)
      {
        data.LEDState = 6; 
      }
      else if(sCmd.indexOf("LEDProg07")>=0)
      {
        data.LEDState = 7; 
      }
      else if(sCmd.indexOf("LEDProg08")>=0)
      {
        data.LEDState = 8; 
      }
      else if(sCmd.indexOf("LEDProg09")>=0)
      {
        data.LEDState = 9; 
      }
      else if(sCmd.indexOf("LEDProg10")>=0)
      {
        data.LEDState = 10; 
      }
      else if(sCmd.indexOf("LEDProg11")>=0)
      {
        data.LEDState = 11; 
      }
      else if(sCmd.indexOf("LEDProg12")>=0)
      {
        data.LEDState = 12; 
      }
      else if(sCmd.indexOf("LEDProg13")>=0)
      {
        data.LEDState = 13; 
      }
      else if(sCmd.indexOf("LEDProg14")>=0)
      {
        data.LEDState = 14; 
      }
      else if(sCmd.indexOf("LEDProg15")>=0)
      {
        data.LEDState = 15; 
      }
      else if(sCmd.indexOf("LEDProg16")>=0)
      {
        data.LEDState = 16; 
      }
      else if(sCmd.indexOf("LEDProg17")>=0)
      {
        data.LEDState = 17; 
      }
      else if(sCmd.indexOf("LEDProg18")>=0)
      {
        data.LEDState = 18; 
      }
      else if(sCmd.indexOf("OTAon")>=0)
      {
        myOTAsetup();
        modus = 5;
      }
      else if(sCmd.indexOf("OTAoff")>=0)
      {
        HotspotOFF();
        modus = 2;
      }
      else if(sCmd.indexOf("ALLOFF")>=0)
      {
        clearStrip();
        modus = 0;
      }
      else if(sCmd.indexOf("RESET")>=0)
      {
        ESP.restart();
      }
      
      
    }
    
    sResponse += "<FONT SIZE=-2>";
    sResponse += "<BR>Aufrufz&auml;hler="; 
    sResponse += ulReqcount;
    sResponse += "<BR>";
    sResponse += "CrazyTuner 03/2020<BR>";
    sResponse += "</body></html>";
    
    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  }
  
  // Send the response to the client
  client.print(sHeader);
  client.print(sResponse);
  
  // and stop the client
  client.stop();
  Serial.println("Client disonnected");
}
