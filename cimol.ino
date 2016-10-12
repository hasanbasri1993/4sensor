
#include <GPRS_Shield_Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>

#define PIN_TX    6
#define PIN_RX    7
//#define PIN_RESET 10
#define BAUDRATE  9600
#define PHONE_NUMBER "+6285695709450"
#define MESSAGE  "Alat telah bekerja"
#define MESSAGE_LENGTH 160
char ussdCommand[160];
char resultcode[16];
char response[24];
char message[MESSAGE_LENGTH];
int messageIndex = 0;

char phone[16];
char datetime[24];



//PENDETEKSI GANGGUAN SISTEM KELISTRIKAN BY M.CIMOL.MAULANA
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h> // Panggil Library LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#include <SimpleTimer.h>
SimpleTimer timer;

#define VOLTAGEPIN A0 // Sensor Tegangan 
#define CURRENTPIN1 A1 // Sensor Arus1 R
#define CURRENTPIN2 A2 // Sensor Arus2 S
#define CURRENTPIN2 A3 // Sensor Arus3 T
#define RelayCurrent1 8 // Pin Relay 1
#define RelayCurrent2 9 // Pin Relay 2
#define RelayCurrent3 10 // Pin Relay 3
#define RelayVariak 13 // Pin Variak
int tegangan = 0;

GPRS gprsTest(PIN_TX, PIN_RX, BAUDRATE); //RX,TX,BaudRate

void setup() {

  Serial.begin(9600);

  pinMode(RelayVariak, OUTPUT);
  pinMode(RelayCurrent1, OUTPUT);
  pinMode(RelayCurrent2, OUTPUT);
  pinMode(RelayCurrent3, OUTPUT);
  Serial.begin(9600);
  //Setting pin Relay
  digitalWrite(RelayVariak, LOW);
  digitalWrite(RelayCurrent1, LOW);
  digitalWrite(RelayCurrent2, LOW);
  digitalWrite(RelayCurrent3, LOW);

  while (!gprsTest.init()) {
    delay(1000);
    Serial.print("init error\r\n");
  }
  Serial.println("gprs init success");
  Serial.println("start to send message ...");
  gprsTest.sendSMS(PHONE_NUMBER, MESSAGE); //define phone number and text

  /*for (int i = 0; i <= 255; i++) {
    Serial.println(i);
    delay(50);
    }
    gprsTest.sendSMS(PHONE_NUMBER, "SUKSE BRI"); //define phone number and text
  */
  Serial.print("WAIT FOR SMS: ");
  //set interval pembacaan dengan timer
  timer.setInterval(1000L, arus1);
  timer.setInterval(1000L, arus2);
  timer.setInterval(1000L, arus3);
  timer.setInterval(1000L, sensortegangan);
  // uncomment yaa buat pake chip yang SIM800L
  //#define pinreset    10
  //gprsTest.powerReset(pinreset);

  //Setting memakai LCD 20 x 4
  lcd.begin(20, 4);

}

void loop() {

  timer.run();

  //nothing to do
  messageIndex = gprsTest.isSMSunread();
  if (messageIndex > 0)
  {
    //At least, there is one UNREAD SMS
    gprsTest.readSMS(messageIndex, message, MESSAGE_LENGTH, phone, datetime);
    //In order not to full SIM Memory, is better to delete it
    gprsTest.deleteSMS(messageIndex);
    Serial.print("From number: ");
    Serial.println(phone);
    Serial.print("Datetime: ");
    Serial.println(datetime);
    Serial.print("Recieved Message: ");
    Serial.println(message);

    if (strcmp(message, "RelayR On") == 0)    {

      digitalWrite(RelayCurrent1, HIGH);
      gprsTest.sendSMS(phone, "Arus R Telah Mati");
      Serial.println("Arus R Telah Mati");
    }


    if (strcmp(message, "RelayS On") == 0)    {

      digitalWrite(RelayCurrent2, HIGH);
      gprsTest.sendSMS(phone, "Arus S Telah Mati");
      Serial.println("Arus S Telah Mati");
    }

    if (strcmp(message, "RelayT On") == 0)    {

      digitalWrite(RelayCurrent3, HIGH);
      gprsTest.sendSMS(phone, "Arus T Telah Mati");
      Serial.println("Arus T Telah Mati");
    }

    if (strcmp(message, "RelayTegangan On") == 0)    {

      digitalWrite(RelayVariak, HIGH);
      gprsTest.sendSMS(phone, "Back Up Tegangan Telah Dilakukan");
      Serial.println("Back Up Tegangan Telah Dilakukan");
    }


    if (strcmp(message, "CEKPULSA") == 0)
    {
      gprsTest.sendUSSDSynchronous("*888#", resultcode, response);
      Serial.print("Recieved Message: ");
      Serial.println(response);
      delay(1000);
      gprsTest.sendSMS(phone, response);
      gprsTest.cancelUSSDSession();
    }
  }
}

//buat fungsi
void arus1 () {

  if (getARUS(A1) > 1.0)
  {
    gprsTest.sendSMS(PHONE_NUMBER, "ArusR Diatas Standar, Perlu Tindakan Lebih Lanjut");
    Serial.print("LEBH EYGHJGDJSHD");
  }
  lcd.setCursor(0, 0);
  lcd.print("ARUS R: ");
  lcd.print(getARUS(A1));
  lcd.print("A");
  Serial.print(getARUS(A1));
  Serial.println(" Amps RMS1 ");
}
void arus2 () {

    if (getARUS(A2) > 1.0)
  {
    gprsTest.sendSMS(PHONE_NUMBER, "ArusS Diatas Standar, Perlu Tindakan Lebih Lanjut");

  }
  lcd.setCursor(0, 1);
  lcd.print("ARUS S: ");
  lcd.print(getARUS(A2));
  lcd.print("A");
  Serial.print(getARUS(A2));
  Serial.println(" Amps RMS2 ");
}
void arus3 () {

    if (getARUS(A3) > 1.0)
  {
    gprsTest.sendSMS(PHONE_NUMBER, "ArusT Diatas Standar, Perlu Tindakan Lebih Lanjut");

  }
  lcd.setCursor(0, 2);
  lcd.print("ARUS T: ");
  lcd.print(getARUS(A3));
  lcd.print("A");
  Serial.print(getARUS(A3));
  Serial.println(" Amps RMS3 ");
}
void sensortegangan () {

  tegangan = map(analogRead(VOLTAGEPIN), 0, 1023, 0, 325);
  if (tegangan < 190)
  {
    gprsTest.sendSMS(PHONE_NUMBER, "Tegangan Dibawah Standar, Perlu Tindakan Lebih Lanjut"); //define phone number and text
  }
  lcd.setCursor(0, 3);
  lcd.print("Tegangan: ");
  lcd.print(tegangan);
  Serial.println(tegangan);
  lcd.print("V");
}
float getARUS(int pin)
{
  float result;

  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  int mVperAmp = 185; // use 100 for 20A Module and 66 for 30A Module


  double Voltage = 0;
  double VRMS = 0;
  double AmpsRMS = 0;

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) //sample for 1 Sec
  {
    readValue = analogRead(pin);
    // see if you have a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the maximum sensor value*/
      minValue = readValue;
    }
  }

  // Subtract min from max
  result = ((maxValue - minValue) * 5.0) / 1024.0;
  VRMS = (result / 2.0) * 0.707;
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  return AmpsRMS;
}
