#include <Arduino.h>
#include <PCF8574.h>

class Ram{
public:
	int debug_lvl; //0->no Debug; 1->simple Debug; 2->more Debug; 3->verbose Debug; negativ->einzeln reserviert
	PCF8574 addrH, addrL, data;
	byte we, oe;
	Ram(){};
	Ram(byte high, byte low, byte dat, byte wPin, byte rPin, int debug_lvl=0) {
		addrH = PCF8574(high);
		addrH.begin(); //begin() muss unbedingt aufgerufen werden!! Der Konstruktor macht das nicht!
		addrL = PCF8574(low);
		addrL.begin();
		data = PCF8574(dat);
		data.begin();
		we = wPin;
		oe = rPin;
		pinMode(we, OUTPUT);
		pinMode(oe, OUTPUT);
		setAddr(0);
		digitalWrite(we, HIGH); //Grundstellung (LOW-Aktiv)
		digitalWrite(oe, HIGH); //Grundstellung (LOW-Aktiv)
		this->debug_lvl = debug_lvl;
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.println("Adress Available:\n\tHigh: " + (String)(addrH.isConnected()?"true":"false") + "\n\tLow:  " + (String)(addrL.isConnected()?"true":"false"));
			Serial.println("Data Availalable:\t" + (String)(data.isConnected()?"true":"false"));
		}
	}

	/**
	 * Reads a byte of data from the specified RAM-chip address.
	 *
	 * @param addr the address to read from
	 *
	 * @return the byte of data read
	 */
	byte read_byte(int addr=0) {
		if (addr != 0)
			setAddr(addr);
	if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Read from: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
		}
		data.setButtonMask(0xFF); //wichtig für lesen!! ("lesemodus")
		digitalWrite(oe, LOW);
		data.readButton8(); //Gelesenes wird automatisch zwischengespeichert!
		digitalWrite(oe, HIGH);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("\tResult: ");
			Serial.println(data.value());
		}
		return data.value(); //zwischenspeicher auslesen
	}

	String read_string(bool ret = false, int addr = 0) {
		short addr_tmp;
		if (ret)
			addr_tmp = getAddr();
		if (addr != 0)
			setAddr(addr);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Read from: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
		}
		data.setButtonMask(0xFF); //wichtig für lesen!! ("lesemodus")
		char read_char = 1;
		String str = "";
		do
		{
			read_char = read_byte();
			str += read_char;
			addrInc();
		} while (read_char != '\0');
		if (ret)
			setAddr(addr_tmp);
		return str;
	}

	int read_int(int addr = 0) {
		if (addr != 0)
			setAddr(addr);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Read from: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
		}
		short i = read_byte() << 8;
		addrDec();
		i |= read_byte();
		addrInc();
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("\tResult: ");
			Serial.println(i);
		}
		return i; //zwischenspeicher auslesen
	}

	/**
	 * Get the address stored for the RAM-chip.
	 *
	 * @return The currently set RAM-chip address.
	 */
	short getAddr() {
		return (addrH.valueOut() << 8) | addrL.valueOut();
	}

	/**
	 * Sets the address for the RAM-chip.
	 *
	 * @param addr the address to set
	 */
	void setAddr(int addr) {
		addrH.write8(highByte(addr));
		addrL.write8(lowByte(addr));
	}

	void addrInc() {
		setAddr(getAddr() + 1);
	}

	void addrDec() {
		setAddr(getAddr() - 1);
	}

	/**
	 * Writes a byte value to a specified memory address.
	 *
	 * @param addr the memory address to write to
	 * @param value the byte value to write
	 */
	void write_byte(byte value, int addr=0) {
		if (addr != 0)
			setAddr(addr);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Writing to: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
			Serial.print("\tValue: ");
			Serial.println(value);
		}
		data.setButtonMask(0); //"schreibmodus"
		digitalWrite(we, LOW);
		data.write8(value);
		digitalWrite(we, HIGH);
		data.write8(0);
	}

	/**
	 * Writes a String value to a specific RAM-chip address.
	 * The adress will point to '\0' at the end of the String.
	 *
	 * @param addr the address to write the value to
	 * @param value the String value to write
	 */
	void write_string(String value, bool ret=false, int addr=0) {
		short addr_tmp;
		if (ret)
			addr_tmp = getAddr();
		if (addr != 0)
			setAddr(addr);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Writing to: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
			Serial.print("\tValue: ");
			Serial.println(value);
		}
		for(char c : value) {
			write_byte(c);
			addrInc();
		}
		write_byte('\0');
		if (ret)
			setAddr(addr_tmp);
	}

	void write_int(int value, int addr=0) {
		if (addr != 0)
			setAddr(addr);
		if (Serial.availableForWrite() && debug_lvl > 1) {
			Serial.print("Writing to: ");
			Serial.print(addrH.valueOut());
			Serial.println(addrL.valueOut());
			Serial.print("\tValue: ");
			Serial.println(value);
		}
		write_byte(highByte(value));
		addrDec();
		write_byte(lowByte(value));
		addrInc();
	}
};