#include <Arduino.h>
#include <Wire.h> // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
#include <PCF8574.h>
#include "ram.cpp"


Ram ram; //MUSS noch initialisiert werden!!

byte count = 0;
int addr = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); //LCD: 16 Zeichen in 2 Zeilen, HEX-Adresse 0x27 

void setup() 
{
  Serial.begin(9600);
  ram = Ram(0x20, 0x21, 0x22, 6, 7, 2); // initialisierung RAM
  lcd.init(); //initialisierung LCD 
  lcd.backlight(); //Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus). 
}

void loop() 
{ 
  
  ram.write(addr, count);
  ram.read(addr);
  count++;
  if (count == 0) {
    ram.write(addr, 0);
    addr++;
  }

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