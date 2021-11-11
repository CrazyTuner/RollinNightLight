#include <Wire.h>

#define MPU6050_ADDR 0x68
#define Altern_ACCEL_GradX 20. // Alternative Empfindl. Accel X-Achse
#define Altern_ACCEL_GradY 2000. // Alternative Empfindl. Accel Y-Achse
#define Altern_GYRO_BesX 30. // Alternative Empfindl. Gyro X-Achse
#define Altern_GYRO_BesY 30. // Alternative Empfindl. Gyro Y-Achse
#define Altern_GYRO_BesZ 60. // Alternative Empfindl. Gyro Z-Achse

const int MPU_addr=0x68; // I2C Addresse MPU-6050
int16_t GYRO_OutX,  GYRO_OutY,  GYRO_OutZ; // Gyroscope (Winkeldrehung)
int16_t TEMP_Out; // Temperatur
int16_t ACCEL_OutX, ACCEL_OutY, ACCEL_OutZ; // Accelerometer (Beschleunigung)
float GYRO_MitX,  GYRO_MitY,  GYRO_MitZ; // Mittelwert Gyroscope (Winkeldrehung)
float TEMP_Mit; // Mittelwert Temperatur
float ACCEL_MitX, ACCEL_MitY, ACCEL_MitZ; // Mittelwert Accelerometer (Beschleunigung)
double Alt_ACCEL_GradX, ACCEL_GradX; // Accelerometer X-Lage in Grad
double Alt_ACCEL_GradY, ACCEL_GradY; // Accelerometer Y-Lage in Grad
double Alt_ACCEL_GradZ, ACCEL_GradZ; // Accelerometer Z-Lage in Grad
double Alt_TEMP_Grad, TEMP_Grad; // Temperatur in Grad
double Alt_GYRO_BesX, GYRO_BesX; // Gyroscope X-Lage Beschleunigung
double Alt_GYRO_BesY, GYRO_BesY; // Gyroscope Y-Lage Beschleunigung
double Alt_GYRO_BesZ, GYRO_BesZ; // Gyroscope Z-Lage Beschleunigung
byte Alarmanlage = 0; // Alarmanlage aus/ein
byte AlarmanlageScharf = 0; // Alarmanlage scharf aus/ein
byte AlarmStatus = 0; // Alarmauslösung aus/ein
long StartZeitpunkt; // Zeitmessung Startzeit
double Grenzw_ACCEL_GradX; // Grenzwert Empfindl Accel X-Achse
double Grenzw_ACCEL_GradY; // Grenzwert Empfindl. Accel Y-Achse
double Grenzw_GYRO_BesX; // Grenzwert Empfindl. Gyro X-Achse
double Grenzw_GYRO_BesY; // Grenzwert Empfindl. Gyro Y-Achse
double Grenzw_GYRO_BesZ; // Grenzwert Empfindl. Gyro Z-Achse

void GyroskopAuslesen() {
  Serial.println(">>CHECK GYROSCOPE");
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false); // the parameter indicates that the Arduino will send a restart. 
                               // As a result, the connection is kept active.
  Wire.requestFrom(MPU6050_ADDR, 14, true); // request a total of 7*2=14 registers
  
  // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same int16_t variable
    ACCEL_OutX = Wire.read() <<8 | Wire.read();   // 0x3B (ACCEL_OutX_H) & 0x3C (ACCEL_OutX_L)
    ACCEL_OutY = Wire.read() <<8 | Wire.read();   // 0x3D (ACCEL_OutY_H) & 0x3E (ACCEL_OutY_L)
    ACCEL_OutZ = Wire.read() <<8 | Wire.read();   // 0x3F (ACCEL_OutZ_H) & 0x40 (ACCEL_OutZ_L)
    TEMP_Out   = Wire.read() <<8 | Wire.read();   // 0x41 (TEMP_Out_H)   & 0x42 (TEMP_Out_L)
    GYRO_OutX  = Wire.read() <<8 | Wire.read();   // 0x43 (GYRO_OutX_H)  & 0x44 (GYRO_OutX_L)
    GYRO_OutY  = Wire.read() <<8 | Wire.read();   // 0x45 (GYRO_OutY_H)  & 0x46 (GYRO_OutY_L)
    GYRO_OutZ  = Wire.read() <<8 | Wire.read();   // 0x47 (GYRO_OutZ_H)  & 0x48 (GYRO_OutZ_L)
}


void FormatMpu6050()                              // Werte formatieren Accel, Temp, Gyro
  {
    Serial.println(">>Format");
    // Werte Accelerometer (Beschleunigung) konvertieren zu 0 bis 2π RADIAN, Radian zu Degree
    // Funktionswert (arctan2) in einem Wertebereich von 360° (vier Quadranten) darstellen
    ACCEL_GradX = atan2(ACCEL_OutY, ACCEL_OutZ) * 180 / PI;
    ACCEL_GradY = atan2(ACCEL_OutX, ACCEL_OutZ) * 180 / PI;
    // Temperatur in °C umrechnen (Datenblatt)
    TEMP_Grad = TEMP_Out / 340.00 + 36.53;
    // Wert Gyroscope (Winkeldrehung) umrechnen
    GYRO_BesX = GYRO_OutX / 131.0 * 2;
    GYRO_BesY = GYRO_OutY / 131.0 * 2;
    GYRO_BesZ = GYRO_OutZ / 131.0 * 2;
    Serial.println(ACCEL_GradX);
    Serial.println(ACCEL_GradY);
    Serial.println(ACCEL_GradZ);
    Serial.println(TEMP_Grad);
    Serial.println(GYRO_BesX);
    Serial.println(GYRO_BesY);
    Serial.println(GYRO_BesZ);
  }


  void LageSichern()                                // aktuelle Werte Accel, Temp, Gyro sichern
  {
    Serial.println("Gyroscope speichern");
    Alt_ACCEL_GradX = ACCEL_GradX;                // Accelerometer X-Lage in Grad
    Alt_ACCEL_GradY = ACCEL_GradY;                // Accelerometer Y-Lage in Grad
    Alt_ACCEL_GradZ = ACCEL_GradZ;                // Accelerometer Z-Lage in Grad
    Alt_TEMP_Grad = TEMP_Grad;                    // Temperatur in °C
    Alt_GYRO_BesX = GYRO_BesX;                    // Gyroscope Beschleunigung X-Achse
    Alt_GYRO_BesY = GYRO_BesY;                    // Gyroscope Beschleunigung Y-Achse
    Alt_GYRO_BesZ = GYRO_BesZ;                    // Gyroscope Beschleunigung Z-Achse
  } 


  void Alarmauswertung()                            // Funktion Alarmauslösung
  {
     if (AlarmStatus != 1)    // kein Alarm
      {
        Serial.println("Alarmauswertung");
        // Alarmauslösung bei Winkeländerung X oder Y um x-Grad oder Beschleunigung
         if (abs(Alt_ACCEL_GradX - ACCEL_GradX) > Grenzw_ACCEL_GradX  ||
             abs(Alt_ACCEL_GradY - ACCEL_GradY) > Grenzw_ACCEL_GradY  ||
             abs(Alt_GYRO_BesX - GYRO_BesX)     > Grenzw_GYRO_BesX    ||
             abs(Alt_GYRO_BesY - GYRO_BesY)     > Grenzw_GYRO_BesY    ||
             abs(Alt_GYRO_BesZ - GYRO_BesZ)     > Grenzw_GYRO_BesZ)
          {
            StartZeitpunkt = millis();            // Zeitmessung Startzeit in ms
            AlarmStatus = 1;                            // Alarmauslösung Hupe ein
            Serial.println(">>ALARM");
          }
      }
  }


  void Alarmausloesung()  {
     if (AlarmStatus == 1)  {
         if (millis()-StartZeitpunkt > 4000) {
            AlarmStatus = 0;                            // Alarmauslösung Zeit abgelaufen
            LageSichern();                        // aktuelle Werte Accel, Temp, Gyro sichern
            Serial.println(">>ALARM vorbei");
          }
      }
  }
