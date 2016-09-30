#include <SimpleTimer.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal.h> // Panggil Library LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

//Macam macam variabel
#define VOLTAGEPIN A0 // Sensor Tegangan
#define CURRENTPIN1 A1 // Sensor Arus 1
#define CURRENTPIN2 A2 // Sensor Arus 2
#define CURRENTPIN3 A3 // Sensor Arus 3
#define SIM800_TX_PIN 8 //SIM800 TX is connected to Arduino D8
#define SIM800_RX_PIN 9 //SIM800 RX is connected to Arduino D9
#define RelayCurrent1 41 // Pin Relay 1
#define RelayCurrent2 42 // Pin Relay 2
#define RelayCurrent3 43 // Pin Relay 3
#define RelayVariak 44 // Pin Variak
SimpleTimer timer;
int tegangan = 0;
int arus1 = 0;
int arus2 = 0;
int arus3 = 0;


double temp_amp1, temp_amp2, temp_amp3 = 0.0; //gunakan tipe data double pada penampung penjumlahan arus sensor
float temps1, adcVolt1, cal_value1, temps2, adcVolt2, cal_value2, temps3, adcVolt3, cal_value3;
unsigned long calTime1, calTime2, calTime3 = 0, time_cal1, time_cal2, time_cal3 = 600;
boolean on_calibrasi1, on_calibrasi2, on_calibrasi3 = false;

SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

void setup() {
  // Setting mode pin Relay menjadi output
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
  //Setting memakai LCD 20 x 4
  lcd.begin(20, 4);
  while (!Serial);

  //Pengaturan SmS
  serialSIM800.begin(9600);
  delay(1000);
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");

  //Rumus SmS
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);

  //SmS nomor yang dituju
  serialSIM800.write("AT+CMGS=\"085695709450\"\r\n");
  delay(1000);

  //SmS alat sudah mulai bekerja
  serialSIM800.write("Alat Bekerja");
  delay(5000);

  //SmS telah selesai
  serialSIM800.write((char)26);
  delay(1000);

  Serial.println("SMS Terkirim");

  // put your setup code here, to run once:
  timer.setInterval(1000L, bacasensor);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.run();
}

void bacasensor () {

  lcd.clear();

  //BACA TEGANGAN
  tegangan = map(analogRead(VOLTAGEPIN), 0, 1023, 0, 220);
  lcd.setCursor(0, 0); lcd.print("Tegangan: ");
  lcd.print(tegangan); lcd.print("V");

  if (tegangan <= 190 ) {
    alertlebihtegangan();
  }

  //BACA ARUS 1 ================================================================================================
  temps1     = analogRead(CURRENTPIN1) * (5.0 / 1023.0); //convert ke tegangan dari ADC
  adcVolt1   = abs(temps1 - 2.50); //mengambil selisih tegangan pada zero point
  adcVolt1  /= 0.185; //Arus dalam A
  adcVolt1  *= 1000; //merubah Arus A ke mA

  if (calTime1 < time_cal1) {
    calTime1++;
    temp_amp1 += adcVolt1; //penjumlahan arus output sensor
    on_calibrasi1 = true;
  } else if (on_calibrasi1 == true) {
    cal_value1 = temp_amp1 / time_cal1; //pembagian nilai keseluruhan dengan waktu
    on_calibrasi1 = false;
  }

  if (on_calibrasi1 == false) {
    adcVolt1 -= cal_value1;
    adcVolt1 = abs(adcVolt1);
    adcVolt1 /= 1000;
    lcd.setCursor(0, 1); lcd.print("ARUS 1: ");
    lcd.print(adcVolt1); lcd.print(" mA");

    if (adcVolt1 >= 1200 ) {
      alertlebiharus1();
    }
  }


  //BACA ARUS 2 ================================================================================================

  temps2     = analogRead(CURRENTPIN2) * (5.0 / 1023.0); //convert ke tegangan dari ADC
  adcVolt2   = abs(temps2 - 2.50); //mengambil selisih tegangan pada zero point
  adcVolt2  /= 0.185; //Arus dalam A
  adcVolt2  *= 1000; //merubah Arus A ke mA

  if (calTime2 < time_cal2) {
    calTime2++;
    temp_amp2 += adcVolt2; //penjumlahan arus output sensor
    on_calibrasi2 = true;
  } else if (on_calibrasi2 == true) {
    cal_value2 = temp_amp2 / time_cal2; //pembagian nilai keseluruhan dengan waktu
    on_calibrasi2 = false;
  }

  if (on_calibrasi2 == false) {
    adcVolt2 -= cal_value2;
    adcVolt2 = abs(adcVolt2);
    adcVolt2 /= 1000;
    lcd.setCursor(0, 2); lcd.print("ARUS 2: ");
    lcd.print(adcVolt2); lcd.print(" mA");

    if (adcVolt2 >= 1200 ) {
      alertlebiharus2();
    }
  }


  //BACA ARUS 3 ================================================================================================

  temps3     = analogRead(CURRENTPIN3) * (5.0 / 1023.0); //convert ke tegangan dari ADC
  adcVolt3   = abs(temps3 - 2.50); //mengambil selisih tegangan pada zero point
  adcVolt3  /= 0.185; //Arus dalam A
  adcVolt3  *= 1000; //merubah Arus A ke mA

  if (calTime3 < time_cal3) {
    calTime3++;
    temp_amp3 += adcVolt3; //penjumlahan arus output sensor
    on_calibrasi3 = true;
  } else if (on_calibrasi3 == true) {
    cal_value3 = temp_amp3 / time_cal3; //pembagian nilai keseluruhan dengan waktu
    on_calibrasi3 = false;
  }

  if (on_calibrasi3 == false) {
    adcVolt1 -= cal_value1;
    adcVolt1 = abs(adcVolt1);
    lcd.setCursor(0, 3); lcd.print("ARUS 3: ");
    lcd.print(adcVolt3); lcd.print(" mA");

    if (adcVolt3 >= 1200 ) {
      alertlebiharus3();
    }
  }
}

void alertlebihtegangan () {

  serialSIM800.begin(9600);
  delay(1000);
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
  serialSIM800.write("AT+CMGS=\"087770933435\"\r\n");
  delay(1000);
  serialSIM800.write("tegnagn nya lebih euyy paur Keamanan, Perlu Ditindak Lanjuti");
  delay(1000);
  serialSIM800.write((char)26);
  delay(1000);
  Serial.println("SMS Terkirim");

}

void alertlebiharus1 () {

  serialSIM800.begin(9600);
  delay(1000);
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
  serialSIM800.write("AT+CMGS=\"087770933435\"\r\n");
  delay(1000);
  serialSIM800.write("Arus 1 nya lebih euyy paur Keamanan, Perlu Ditindak Lanjuti");
  delay(1000);
  serialSIM800.write((char)26);
  delay(1000);
  Serial.println("SMS Terkirim");

}

void alertlebiharus2 () {

  serialSIM800.begin(9600);
  delay(1000);
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
  serialSIM800.write("AT+CMGS=\"087770933435\"\r\n");
  delay(1000);
  serialSIM800.write("Arus 2 nya lebih euyy paur Keamanan, Perlu Ditindak Lanjuti");
  delay(1000);
  serialSIM800.write((char)26);
  delay(1000);
  Serial.println("SMS Terkirim");

}

void alertlebiharus3 () {

  serialSIM800.begin(9600);
  delay(1000);
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
  serialSIM800.write("AT+CMGS=\"087770933435\"\r\n");
  delay(1000);
  serialSIM800.write("Arus 3 nya lebih euyy paur Keamanan, Perlu Ditindak Lanjuti");
  delay(1000);
  serialSIM800.write((char)26);
  delay(1000);
  Serial.println("SMS Terkirim");

}
