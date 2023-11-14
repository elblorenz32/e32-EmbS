#include <Arduino.h>
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
#include <PCF8574.h>

PCF8574 addrH(0x20);
PCF8574 addrL(0x21);
PCF8574 data(0x22);

int we = 6;
int oe = 7;
byte count = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD: 16 Zeichen in 2 Zeilen, HEX-Adresse 0x27 

void setup() 
{
  pinMode(we, OUTPUT);
  pinMode(oe, OUTPUT);
  digitalWrite(oe, HIGH);
  digitalWrite(we, LOW);
  data.setButtonMask(0); //write
  data.write8(0);
  addrL.setButtonMask(0); //write
  addrL.write8(0);
  addrH.setButtonMask(0); //write
  addrH.write8(0);
  data.toggleMask(0xFF); //toggle all pins
  data.write8(count);
  delay(100);
  digitalWrite(we, HIGH);
  lcd.init(); //Im Setup wird der LCD gestartet 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
  Serial.begin(9600);
}

void loop() 
{ 
  digitalWrite(oe, LOW);
  delay(10);
  Serial.println(data.read8());
  digitalWrite(oe, HIGH);
  digitalWrite(we, LOW);
  count++;
  delay(10);
  data.write8(count);
  digitalWrite(we, HIGH);
  delay(10);
  data.write8(0);
  lcd.setCursor(0, 0);//Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile. 
  int h = ((millis()/1000)/60)/60;
  int m = ((millis()/1000)/60)%60;
  int s = (millis()/1000)%60;
  int mm = millis()%1000;
  if (h<10) {
    lcd.print("0");
  }
  lcd.print(h);
  lcd.print(":");
  if (m<10){
    lcd.print("0");
  }
  lcd.print(m);
  lcd.print(":");
  if (s<10){
    lcd.print("0");
  }
  lcd.print(s);
  lcd.print(":");
  lcd.print(mm);
}