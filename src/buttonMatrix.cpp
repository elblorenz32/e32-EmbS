#include <Arduino.h>

namespace e32 {
	class button_matrix {
	public:
		int rows[4] = {7, 8, 9, 10};
		int cols[4] = {6, 5, 4, 3};
		int buttons[4][4];
		char facing = 'U';
		button_matrix() {}
		button_matrix(int* new_rows, int* new_cols) {
			if (sizeof(new_rows)/sizeof(int) == 4 && sizeof(new_cols)/sizeof(int) == 4) {
				for (int i = 0; i < sizeof(new_rows)/sizeof(int); i++) {
					rows[i] = new_rows[i];
					cols[i] = new_cols[i];
				}
			}
		}
		
		void read() {
			for (int i = 0; i < 4; i++) {
				pinMode(rows[i], INPUT_PULLUP);
				for (int j = 0; j < 4; j++) {
					buttons[i][j] = read_button(i, j);
					pinMode(cols[j], INPUT_PULLUP);
				}
			}
		}

		bool read_any(int* to_read) {
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (to_read[i] & (1 << j))
						if ( read_button(i, j))
					{
						return true;
					}
				}
			}
			return false;
		}

		int read_button(int r, int c) {
			/*Set Input:*/
			if(rows[r] > 7) { //Pin between 8 and 15
				PORTB |= _BV(rows[r] - 8); //Pullup/High
				DDRB &= ~_BV(rows[r] - 8); //Input
			}
			else { //Pin between 0 and 7
				PORTD |= _BV(rows[r]); //Pullup/High
				DDRD &= ~_BV(rows[r]); //Input
			}
			/*Set Output:*/
			if(cols[c] > 7) { //Pin between 8 and 15
				PORTB &= ~_BV(cols[c] - 8); //Low
				DDRB |= _BV(cols[c] - 8); //Output
			}
			else { //Pin between 0 and 7
				PORTD &= ~_BV(cols[c]); //Low
				DDRD |= _BV(cols[c]); //Output
			}
			/*
			Above code in short:
			pinMode(rows[r], INPUT_PULLUP);
			pinMode(cols[c], OUTPUT);
			digitalWrite(cols[c], LOW);
			*/
			bool result = !digitalRead(rows[r]);
			
			/*Reset Output to avoid interference:*/
			if(cols[r] > 7) { //Pin between 8 and 15
				PORTB |= _BV(cols[c] - 8); //Pullup/High
				DDRB &= ~_BV(cols[c] - 8); //Input
			}
			else { //Pin between 0 and 7
				PORTD |= _BV(cols[c]); //Pullup/High
				DDRD &= ~_BV(cols[c]); //Input
			}
			return result;
		}

		String read_button_pairs() {
			String result = "";
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (read_button(i, j)) {
						result += String(i) + ", " + String(j);
						result += "\n";
					}
				}
			}
			return result;
		}

		void flip(bool fcols) {
			int temp[4];
			if (fcols) {
				for (int i = 0; i < 4; i++) {
					temp[i] = cols[i];
				}
				for (int i = 0; i < 4; i++) {
					cols[3 - i] = temp[i];
				} 
			}
			else {
				for (int i = 0; i < 4; i++) {
					temp[i] = rows[i];
				}
				Serial.println();
				for (int i = 0; i < 4; i++) {
					rows[3 - i] = temp[i];
				}
			}
		}

		void rotate(char goal) {
			int* temp[4];
			Serial.println(facing);
			switch (facing) {
			case 'U':
				switch (goal) {
				case 'R':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					flip(false);
					break;
				case 'L':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					break;
				case 'D':
					flip(false);
					break;
				}
				break;
			case 'R':
				switch (goal) {
				case 'U':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					break;
				case 'L':
					flip(false);
					break;
				case 'D':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					break;
				}
				break;
			case 'D':
				switch (goal) {
				case 'U':
					flip(false);
					break;
				case 'L':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					break;
				case 'R':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					break;
				}
				break;
			case 'L':
				switch (goal) {
				case 'R':
					flip(false);
					break;
				case 'U':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(true);
					break;
				case 'D':
					memcpy(temp, rows, sizeof(rows));
					memcpy(rows, cols, sizeof(cols));
					memcpy(cols, temp, sizeof(temp));
					flip(false);
					break;
				}
				break;
			}
			facing = goal;
		}
	};
}