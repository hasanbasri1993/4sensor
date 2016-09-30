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


double temp_amp1 = 0.0; //gunakan tipe data double pada penampung penjumlahan arus sensor
float temps1, adcVolt1, cal_value1;
unsigned long calTime1 = 0, time_cal1 = 600;
boolean on_calibrasi1 = false;

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
  timer.setInterval(1000L, sensortegangan);
  timer.setInterval(1000L, arusbaca1);
}

void loop() {
  // put your main code here, to run repeatedly:
  timer.run();
}

void sensortegangan () {

  lcd.clear;
  tegangan = map(analogRead(VOLTAGEPIN), 0, 1023, 0, 325);

  if (tegangan <= 190)
  {
    
    lcd.setCursor(0, 0); lcd.print("Tegangan: ");
    lcd.print(tegangan); lcd.print("V");
    serialSIM800.begin(9600);
    delay(1000);
    Serial.println("Setup Complete!");
    Serial.println("Sending SMS...");
    serialSIM800.write("AT+CMGF=1\r\n");
    delay(1000);
    serialSIM800.write("AT+CMGS=\"087770933435\"\r\n");
    delay(1000);
    serialSIM800.write("Tegangan dibawah Standar Keamanan, Perlu Ditindak Lanjuti");
    delay(1000);
    serialSIM800.write((char)26);
    delay(1000);
    Serial.println("SMS Terkirim");

  } else
  {
    lcd.setCursor(0, 0); lcd.print("Tegangan : ");
    lcd.print(tegangan); lcd.print("V");
    digitalWrite(RelayVariak, LOW);
  }
}




void arusbaca1() {
  temps1     = analogRead(CURRENTPIN1) * (5.0 / 1023.0); //convert ke tegangan dari ADC
  adcVolt1   = abs(temps1 - 2.50); //mengambil selisih tegangan pada zero point
  adcVolt1  /= 0.185; //Arus dalam A
  adcVolt1  *= 1000; //merubah Arus A ke mA

  //proses kalibrasi
  /*
    bagian if(calTime < time_cal) merupakan seleksi waktu. Digunakan sebagai
    pembatas satu kali kalibrasi, hal ini sangat di anjurkan untuk mengurangi
    kelebihan muatan pada var calTime dan temp_amp11 dimana jika tidak di batasi
    akan melakukan penambahan berulang-ulang.
  */
  if (calTime1 < time_cal1) {
    calTime1++;
    Serial.print("Kalibrasi Time:");
    Serial.println(calTime1);
    temp_amp1 += adcVolt1; //penjumlahan arus output sensor
    on_calibrasi1 = true;
  } else if (on_calibrasi1 == true) {
    cal_value1 = temp_amp1 / time_cal1; //pembagian nilai keseluruhan dengan waktu
    on_calibrasi1 = false;
  }

  if (on_calibrasi1 == false) {
    adcVolt1 -= cal_value1;
    adcVolt1 = abs(adcVolt1);
    Serial.println("Dalam");
    Serial.print(" mA :");
    Serial.println(adcVolt1);
    adcVolt1 /= 1000;
    Serial.print(" A :");
    Serial.println(adcVolt1);
    Serial.println(" ");

    if (adcVolt1 >= 1)
    {

      lcd.setCursor(0, 1);
      lcd.print("ARUS R: ");
      lcd.print(adcVolt1);
      lcd.print("A");
      serialSIM800.begin(9600);
      delay(1000);
      Serial.println("Setup Complete!");
      Serial.println("Sending SMS...");
      serialSIM800.write("AT+CMGF=1\r\n");
      delay(1000);
      serialSIM800.write("AT+CMGS=\"085695709450\"\r\n");
      delay(1000);
      serialSIM800.write("Arus phase R Terjadi Arus Berlebih, Perlu Ditindak Lanjuti");
      delay(1000);
      serialSIM800.write((char)26);
      delay(1000);
      Serial.println("SMS Terkirim");

    } else
    {
      lcd.setCursor(0, 1);
      lcd.print("ARUS R: ");
      lcd.print(adcVolt1);
      lcd.print("A");
      digitalWrite(RelayCurrent1, LOW);
    }
  }
}
