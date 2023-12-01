#include <Arduino.h>
#include <Wire.h>

namespace e32 {
	bool await() {
		while (!Wire.available()) { //???
			delay(1);
		}
		return true;
	}
}