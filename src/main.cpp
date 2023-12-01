#include <Arduino.h>
#include <Wire.h>			   // Wire Bibliothek einbinden
#include <LiquidCrystal_I2C.h> // Vorher hinzugefügte LiquidCrystal_I2C Bibliothek einbinden
#include <PCF8574.h>
#include "ram.cpp"
#include "buttonMatrix.cpp"

using namespace e32;

Ram ram; // MUSS noch initialisiert werden!!

byte count = 0;
int addr = 0;
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD: 16 Zeichen in 2 Zeilen, HEX-Adresse 0x27

button_matrix matrix;

void setup()
{
	Serial.begin(9600);
	ram = Ram(0x20, 0x21, 0x22, 12, 13, 0); // initialisierung RAM
	lcd.init();							  // initialisierung LCD
	lcd.backlight();					  // Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus).
	matrix = button_matrix();
	matrix.rotate('D');
	//matrix.rotate('U');
	// U->L, L->D, D->R, R->U
	// U->D, D->U
	// R->L, L->R
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
	Serial.println();
	int button_group[4] = {
		0b1111,
		0b0000,
		0b0100,
		0b1001
	};
	Serial.println(matrix.read_any(button_group) + String(matrix.facing));
	//Serial.println(matrix.read_button(0, 0) + String(matrix.rows[0]) + String(matrix.cols[0]));
	//Serial.println(matrix.read_button(0, 1) + String(matrix.rows[0]) + String(matrix.cols[1]));
	delay(1000);

	ram.write_string("Hello World", true);
	ram.addrInc();

	lcd_time(2);
}