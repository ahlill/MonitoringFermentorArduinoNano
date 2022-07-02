//include library SD
#include <SPI.h>
#include <SD.h>

//include library DS18b20
#include <OneWire.h>
#include <DallasTemperature.h>

//include library rtc ds3231
#include <DS3231.h>

//include library I2C
#include <Wire.h>

//include library LCD 20x4
#include <LiquidCrystal_I2C.h>

// sensor diletakkan di pin 3 dan 4
#define ONE_WIRE_BUS1 3
#define ONE_WIRE_BUS2 4

//init sensor DS18b20
OneWire oneWire1(ONE_WIRE_BUS1);
OneWire oneWire2(ONE_WIRE_BUS2);

DallasTemperature sensorSuhu1(&oneWire1);
DallasTemperature sensorSuhu2(&oneWire2);

//init SD module
File myFile;

//init sensor DS3231
DS3231 rtc;
RTCDateTime dt;

//init LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

//boolean condition = false;
int counter = 1;
int lastTime = -1;
byte derajat = B11011111;

//pin SD
int sdPin = 8;

void setup() {

  //start serial using port 9600
  Serial.begin(9600);

  //start sensor rtc DS3231
  rtc.begin();

  //set start time rtc DS3231
  rtc.setDateTime(2022, 2, 22, 4, 59, 55);

  //start LCD 20x4
  lcd.init();
  lcd.backlight();

  //start sensor DS18b20 1 dan 2
  sensorSuhu1.begin();
  sensorSuhu2.begin();

  //connecting to SD card
  lcd.setCursor(1, 1);
  lcd.print("Init SD card...");
  Serial.println("Init SD card...");
  if (!SD.begin(sdPin)) {
    lcd.clear();
    lcd.setCursor(1, 1);
    lcd.print("init failed!");
    Serial.println("init failed!");
//    while (1);
  }
  delay(500);
  lcd.println("init done.");
  Serial.println("init done.");
  lcd.clear();

  dt = rtc.getDateTime();

  lcd.setCursor(1, 0);
  lcd.print(dt.hour); lcd.print(":");
  lcd.print(dt.minute); lcd.print(":");
  lcd.print(dt.second); lcd.print(" ");
  lcd.setCursor(1, 1);
  lcd.print("Suhu1: ");
  lcd.print(ambilSuhu1());
  lcd.print(" ");
  lcd.write(derajat);
  lcd.print("C");
  lcd.setCursor(1, 2);
  lcd.print("Suhu2: ");
  lcd.print(ambilSuhu2());
  lcd.print(" ");
  lcd.write(derajat);
  lcd.print("C");
}
void loop() {

  myFile = SD.open("myfile.txt", FILE_WRITE);
  if (myFile) {
    lcd.clear();
    lcd.setCursor(1, 3);
    lcd.print("SD ok");
    Serial.println("SD Ok");
    myFile.close();
  } else {
    lcd.clear();
    lcd.setCursor(1, 3);
    lcd.print("SD Bermasalah");
    Serial.println("SD bermasalah");
    SD.begin(sdPin);
  }

  dt = rtc.getDateTime();

  float suhuTabung1 = ambilSuhu1();
  float suhuTabung2 = ambilSuhu2();
  delay(200);
  
  lcd.setCursor(1, 0);
  lcd.print(dt.hour); lcd.print(":");
  lcd.print(dt.minute); lcd.print(":");
  lcd.print(dt.second); lcd.print(" ");
  lcd.setCursor(1, 1);
  lcd.print("Tabung 1: ");
  lcd.print(suhuTabung1);
  lcd.print(" ");
  lcd.write(derajat);
  lcd.print("C");
  lcd.setCursor(1, 2);
  lcd.print("Tabung 2: ");
  lcd.print(suhuTabung2);
  lcd.print(" ");
  lcd.write(derajat);
  lcd.print("C");

  Serial.print(dt.year); Serial.print("-");
  Serial.print(dt.month); Serial.print("-");
  Serial.print(dt.day); Serial.print(" ");
  Serial.print(dt.hour); Serial.print(":");
  Serial.print(dt.minute); Serial.print(":");
  Serial.print(dt.second); Serial.print(" ");
  Serial.print("---> ");
  Serial.print("Tabung 1: ");
  Serial.print(suhuTabung1);
  Serial.print(" | ");
  Serial.print("Tabung 2: ");
  Serial.println(suhuTabung2);

  if (dt.hour - lastTime != 0 && ambilSuhu1() > 0 && ambilSuhu2() > 0) {
    Serial.println(dt.hour);
    Serial.println(lastTime);
    lastTime = dt.hour;
    Serial.println(dt.hour);
    Serial.println(lastTime);

    //opening SD card
    myFile = SD.open("myfile.txt", FILE_WRITE);
    if (myFile) {
      lcd.setCursor(1, 0);
      lcd.print(dt.hour); lcd.print(":");
      lcd.print(dt.minute); lcd.print(":");
      lcd.print(dt.second); lcd.print(" ");
      lcd.setCursor(1, 1);
      lcd.print("Tabung 1: ");
      lcd.print(suhuTabung1);
      lcd.print(" ");
      lcd.write(derajat);
      lcd.print("C");
      lcd.setCursor(1, 2);
      lcd.print("Tabung 2: ");
      lcd.print(suhuTabung2);
      lcd.print(" ");
      lcd.write(derajat);
      lcd.print("C");

      myFile.print(dt.year); myFile.print("-");
      myFile.print(dt.month); myFile.print("-");
      myFile.print(dt.day); myFile.print(" ");
      myFile.print(dt.hour); myFile.print(":");
      myFile.print(dt.minute); myFile.print(":");
      myFile.print(dt.second); myFile.print(" ");
      myFile.print(suhuTabung1);
      myFile.print(" ");
      myFile.println(suhuTabung2);

      // close the file:
      myFile.close();
    } else {
      Serial.println("error opening myfile.txt");
      lcd.clear();
      lcd.print("SD card bermasalah");
      SD.begin(sdPin);
      delay(100);
    }
  }
  delay(2000);
}

float ambilSuhu1()
{
  sensorSuhu1.requestTemperatures();
  float suhu = sensorSuhu1.getTempCByIndex(0);
  return suhu;
}

float ambilSuhu2()
{
  sensorSuhu2.requestTemperatures();
  float suhu = sensorSuhu2.getTempCByIndex(0);
  return suhu;
}

//String ambilSuhu3()
//{
//  sensorSuhu1.requestTemperatures();
//  float suhu = sensorSuhu1.getTempCByIndex(0);
//  String suhuString = "Tabung 1: " + String(suhu) + " " + byte(derajat) + "C";
//  return suhuString;
//}
