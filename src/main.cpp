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
String lcd_text[] = {"", ""};
long last_lcd_update = 0;
bool clear_lcd = false;
int cursor_pos[2] = {0, 0};
String lcd_pos = "main";
int lcd_scroll = 0;

button_matrix matrix;
int up[4] {
	0b0110,
	0b0000,
	0b0000,
	0b0000
};
int down[4] {
	0b0000,
	0b0000,
	0b0000,
	0b0110
};
int left[4] {
	0b0000,
	0b1000,
	0b1000,
	0b0000
};
int right[4] {
	0b0000,
	0b0001,
	0b0001,
	0b0000
};
int ok[4] {
	0b0000,
	0b0110,
	0b0110,
	0b0000
};
int back[4] {
	0b0000,
	0b0000,
	0b0000,
	0b1000
};

int num_stops;
String* stops;
String ram_command = "";

enum events {
	null,
	RAM_READ,
	BUTTON_INPUT,
	DISPLAY_OUTPUT
};

events event_queue[3] {null, null, null};

void push_event(events event) {
	for (int i = 0; i < 3; i++) {
		if (event_queue[i] == event) {
			return;
		}
		if (event_queue[i] == null) {
			event_queue[i] = event;
			return;
		}
	}
}

void button_event() {
	// TODO
}

void display_event() {
	
	if (ram.isI2CBlocked() || millis() - last_lcd_update < 100) {
		last_lcd_update = millis();
		push_event(DISPLAY_OUTPUT);
		return;
	}
	ram.blockI2C();
	if (clear_lcd) {
		lcd.clear();
		clear_lcd = false;
	}
	if (lcd_pos = "main") {
		lcd.setCursor(0, 0);
		lcd_text[0] = stops[lcd_scroll%num_stops];
		lcd.print(lcd_text[0]);
		lcd.setCursor(0, 1);
		lcd_text[1] = stops[(lcd_scroll+1)%num_stops];
		lcd.print(lcd_text[1]);
	}
	ram.unblockI2C();
}

void ram_event() {
	if (ram.isI2CBlocked()) {
		push_event(RAM_READ);
		return;
	}
	ram.blockI2C();
	if (ram_command == "#stops") {
		ram.setAddr(1);
		num_stops = ram.read_int();
		if (num_stops == 0) { // no stops --> abort (would leave stops[] uninitialized)
			push_event(RAM_READ);
			ram.unblockI2C();
			Serial.println("no stops");
			return;
		}
		ram_command = ""; // done
	}
	else if (ram_command == "stops") {
		ram.get_stops(stops);
		clear_lcd = true;
		lcd_text[0] = stops[lcd_scroll%num_stops];
		lcd_text[1] = stops[(lcd_scroll+1)%num_stops];
		push_event(DISPLAY_OUTPUT);
		ram_command = ""; // done
	}
	ram.unblockI2C();
}

void event_handler() {
	events event = event_queue[0];
	event_queue[0] = event_queue[1];
	event_queue[1] = event_queue[2];
	event_queue[2] = null;
	Serial.println(event);
	switch (event) {
	case RAM_READ:
		Serial.println("RAM_READ");
		ram_event();
		break;
	case BUTTON_INPUT:
		button_event();
		break;
	case DISPLAY_OUTPUT:
		display_event();
		break;
	case null:
		break;
	default:
		break;
	}
	return;
}

/**
 * Displays the runtime on the LCD screen with the specified precision.(For Testing Purposes)
 *
 * @param precision number of time precicion steps (hh, mm, ss, ms) accepts 1, 2 or 3, hours will always be displayed, default is 3
 *
 * @return void
 *
 * @throws None
 */
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

void setup()
{
	Serial.begin(9600);
	ram = Ram(0x20, 0x21, 0x22, 12, 13, 0); // initialisierung RAM
	ram.blockI2C();
	ram.setAddr(1);
	ram.write_int(1);
	ram.addrInc();
	ram.write_string("Alttrachau", true);
	Serial.print(ram.read_string(true));
	lcd.init();							  // initialisierung LCD
	lcd.backlight();					  // Hintergrundbeleuchtung einschalten (lcd.noBacklight(); schaltet die Beleuchtung aus).
	ram.unblockI2C();
	matrix = button_matrix();
	ram_command = "#stops";
	push_event(RAM_READ);
}


void loop()
{
	if (!ram_command.equals("#stops") || 1) {
		if(lcd_pos == "main" && 0) {
			ram_command = "stops";
			push_event(RAM_READ);
			clear_lcd = true;
			push_event(DISPLAY_OUTPUT);
		}
		if (matrix.read_any(up)) {
			lcd_scroll++;
			clear_lcd = true;
			push_event(DISPLAY_OUTPUT);
		}
		if (matrix.read_any(down)) {
			lcd_scroll--;
			clear_lcd = true;
			push_event(DISPLAY_OUTPUT);
		}
	}
	//event_handler();
	ram.write_int(lcd_scroll);
	Serial.print(lcd_scroll + " ");
	Serial.println(ram.read_int());
	lcd.setCursor(0, 1);
	lcd.print("        ");
	lcd.setCursor(0, 1);
	lcd.print(ram.read_int());
	lcd_time();
	delay(100);
}