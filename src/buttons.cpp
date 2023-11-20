#include <Arduino.h>

class Buttons
{
public:
	byte *buttons;

	Buttons()
	{
		buttons = new byte[0];
	}
	Buttons(byte b1)
	{
		if (0 < b1 && b1 < 16)
		{
			buttons = new byte[1];
			buttons[1] = b1;
			pinMode(b1, INPUT_PULLUP);
		}
	}
	Buttons(byte b1, byte b2)
	{
		if ((0 < b1 && b1 < 16) &&
			(0 < b2 && b2 < 16))
		{
			buttons = new byte[2];
			buttons[0] = b1;
			buttons[1] = b2;
			pinMode(b1, INPUT_PULLUP);
			pinMode(b2, INPUT_PULLUP);
		}
	}
	Buttons(byte b1, byte b2, byte b3)
	{
		if ((0 < b1 && b1 < 16) &&
			(0 < b2 && b2 < 16) &&
			(0 < b3 && b3 < 16))
		{
			buttons = new byte[3];
			buttons[0] = b1;
			buttons[1] = b2;
			buttons[2] = b3;
			pinMode(b1, INPUT_PULLUP);
			pinMode(b2, INPUT_PULLUP);
			pinMode(b3, INPUT_PULLUP);
		}
	}
	Buttons(byte b1, byte b2, byte b3, byte b4)
	{
		if ((0 < b1 && b1 < 16) &&
			(0 < b2 && b2 < 16) &&
			(0 < b3 && b3 < 16) &&
			(0 < b4 && b4 < 16))
		{
			buttons = new byte[4];
			buttons[0] = b1;
			buttons[1] = b2;
			buttons[2] = b3;
			buttons[3] = b4;
			pinMode(b1, INPUT_PULLUP);
			pinMode(b2, INPUT_PULLUP);
			pinMode(b3, INPUT_PULLUP);
			pinMode(b4, INPUT_PULLUP);
		}
	}

	bool getButton(byte button)
	{
		if (button < sizeof(buttons) / 8)
		{
			return !digitalRead(buttons[button]);
		}
	}
};