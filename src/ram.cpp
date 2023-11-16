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
        digitalWrite(we, HIGH); //Grundstellung (LOW-Aktiv)
        digitalWrite(oe, HIGH); //Grundstellung (LOW-Aktiv)
        this->debug_lvl = debug_lvl;
        if (Serial.availableForWrite() && debug_lvl > 1) {
            Serial.println("Adress Available:\n\tHigh: " + (String)(addrH.isConnected()?"true":"false") + "\n\tLow:  " + (String)(addrL.isConnected()?"true":"false"));
            Serial.println("Data Availalable:\t" + (String)(data.isConnected()?"true":"false"));
        }
    }

    byte read(int addr) {
        if (Serial.availableForWrite() && debug_lvl > 1) {
            Serial.print("Read from: ");
            Serial.println(addr);
        }
        addrH.write8(highByte(addr));
        addrL.write8(lowByte(addr));
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


    void write(int addr, byte value) {
        if (Serial.availableForWrite() && debug_lvl > 1) {
            Serial.print("Writing to: ");
            Serial.println(addr);
            Serial.print("\tValue: ");
            Serial.println(value);
        }
        addrH.write8(highByte(addr));
        addrL.write8(lowByte(addr));
        data.setButtonMask(0); //"schreibmodus"
        digitalWrite(we, LOW);
        data.write8(value);
        digitalWrite(we, HIGH);
        data.write8(0);
    }
};