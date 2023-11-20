#include <Arduino.h>
#include <Wire.h>			   // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
#include <PCF8574.h>
#include "ram.cpp"

Ram ram; // MUSS noch initialisiert werden!!

byte count = 0;
int addr = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD: 16 Zeichen in 2 Zeilen, HEX-Adresse 0x27

void setup()
{
	Serial.begin(9600);
	ram = Ram(0x20, 0x21, 0x22, 6, 7, 0); // initialisierung RAM
	lcd.init();							  // initialisierung LCD
	lcd.backlight();					  // Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus).
}


void lcd_time(short precision = 3) {
	lcd.setCursor(0, 0); // Hier wird die Position des ersten Zeichens festgelegt. In diesem Fall bedeutet (0,0) das erste Zeichen in der ersten Zeile.
	int h = ((millis() / 1000) / 60) / 60;
	if (h < 10)
	{
		lcd.print("0");
	}
	lcd.print(h);
	if (precision >= 1) {
		int m = ((millis() / 1000) / 60) % 60;
		lcd.print(":");
		if (m < 10)
		{
			lcd.print("0");
		}
		lcd.print(m);
	}
	if (precision >= 2) {
		int s = (millis() / 1000) % 60;
		lcd.print(":");
		if (s < 10)
		{
			lcd.print("0");
		}
		lcd.print(s);
	}
	if (precision >= 3) {
		int ms = millis() % 1000;
		lcd.print(".");
		if (ms < 100)
		{
			lcd.print("0");
		}
		if (ms < 10)
		{
			lcd.print("0");
		}
		lcd.print(ms);
	}
}


void loop()
{
	ram.write_string("Hello World", true);
	ram.addrInc();

	lcd_time(2);
}