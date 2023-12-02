import json
import random
import time
from pcf8574 import PCF8574
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BOARD)

json_str = """
{
		"Alttrachau": [
				4
		],
		"Mickten": [
				4,
				9,
				13
		],
		"Sachsenallee": [
				6,
				13
		],
		"Permoser Str.": [
				6
		],
		"Hans-Grundig-Straße": [
				62
		],
		"Postplatz": [
				1,
				2,
				4,
				8,
				9,
				11,
				12
		]
}"""
json_data = json.loads(json_str)

addrH = PCF8574(1, 0x20)
addrL = PCF8574(1, 0x21)
data = PCF8574(1, 0x22)
oe = 15 # output enable
we = 13 # write enable
meBlockUno = 35 # (soft) block arduino i2c traffic
unoBlockMe = 37
GPIO.setup(oe, GPIO.IN) # dont interfere with the arduino
GPIO.setup(we, GPIO.IN) # dont interfere with the arduino
GPIO.setup(meBlockUno, GPIO.OUT)
GPIO.output(meBlockUno, GPIO.LOW)
GPIO.setup(unoBlockMe, GPIO.IN)
address = 0 # current address used for RAM-Chip. Real address is Volatile!

def bitfield(n): # len 8 (bytewise)
	'''
	Generate a bitfield representation of a number.
	
	:param n: The number to convert into a bitfield.
	:type n: int
	:return: A list of integers representing the bitfield.
	:rtype: list[int]
	'''
	return [n >> i & 1 for i in range(7, -1, -1)]

def debitfield(arr):
	'''
	Generate the integer value of a bitfield.

	:param arr: The bitfield to convert.
	:type arr: list[int]
	:return: The integer value of the bitfield.
	:rtype: int
	'''
	return int(str([].join(arr)), 2)

def blockI2C():
	'''
	Blocks the I2C bus.

	:return: None
	'''
	GPIO.output(meBlockUno, GPIO.HIGH)
	time.sleep(0.01)
	
def isI2CBlocked():
	'''
	Checks if the I2C bus is blocked.

	:return: True if the I2C bus is blocked, False otherwise.
	:rtype: bool
	'''
	return GPIO.input(unoBlockMe)

def unblockI2C():
	'''
	Unblocks the I2C bus.

	:return: None
	'''
	GPIO.output(meBlockUno, GPIO.LOW)

def setAddr(addr):
	'''
	Set the address of the RAM-Chip.

	:param addr: The address to be set.
	:return: None
	'''
	global address
	addrH.port = bitfield(addr >> 8)
	addrL.port = bitfield(addr & 0xFF)
	address = addr

def addrInc():
	global address
	setAddr(address+1)
	address += 1

def addrDec():
	global address
	setAddr(address-1)
	address -= 1

def write_byte(value, addr=None):
	'''
	Write a byte to the RAM-Chip.

	:param value: The byte to be written.
	:type value: int
	:param addr: The address to write to. If not set, the current address will be used. (MAY BE OVERWRITTEN BY ARDUINO!)
	:type addr: int
	:return: None
	'''
	if addr != None:
		setAddr(addr)
	data.port = bitfield(value)
	GPIO.setup(we, GPIO.OUT)
	GPIO.setup(oe, GPIO.OUT)
	GPIO.output(we, GPIO.LOW)
	GPIO.output(oe, GPIO.HIGH)
	GPIO.output(we, GPIO.HIGH)
	GPIO.setup(we, GPIO.IN)
	GPIO.setup(oe, GPIO.IN)

def write_int(data, addr=None):
	if addr != None:
		setAddr(addr)
	write_byte(data >> 8) # high byte
	addrDec()
	write_byte(data & 0xFF) # low byte
	addrInc()

def write_str(data, addr=None, ret=False):
	addr_tmp = 0
	if ret == True:
		addr_tmp = address
	if addr != None:
		setAddr(addr)
	for char in data:
		write_byte(ord(char))
		addrInc()
	write_byte(0)
	if ret == True:
		setAddr(addr_tmp)

def read_byte(addr=None):
	"""
	Reads a byte from a specified address.

	:param addr: The address to read from. If not provided, the function will read from the current address set by `setAddr`.
	:type addr: int, optional
	:return: The byte read from the specified address.
	:rtype: int
	"""
	if addr != None:
		setAddr(addr)
	GPIO.setup(we, GPIO.OUT)
	GPIO.setup(oe, GPIO.OUT)
	GPIO.output(we, GPIO.HIGH)
	GPIO.output(oe, GPIO.LOW)
	res = debitfield(data.port)
	GPIO.output(oe, GPIO.HIGH)
	GPIO.setup(we, GPIO.IN)
	GPIO.setup(oe, GPIO.IN)
	return res

def read_int(addr=None):
	"""
	Reads an integer from a specified address.

	:param addr: The address to read from. If not provided, the function will read from the current address set by `setAddr`.
	:type addr: int, optional
	:return: The integer read from the specified address.
	:rtype: int
	"""
	if addr != None:
		setAddr(addr)
	i = read_byte() << 8
	addrDec()
	i |= read_byte()
	return i

def read_str(addr=None, ret=False):
	"""
	Reads a string from a specified address.

	:param addr: The address to read from. If not provided, the function will read from the current address set by `setAddr`.
	:type addr: int, optional
	:return: The string read from the specified address.
	:rtype: str
	"""
	addr_tmp = 0
	if ret == True:
		addr_tmp = address
	if addr != None:
		setAddr(addr)
	s = ""
	while read_byte() != 0:
		s += chr(read_byte())
		addrInc()
	if ret == True:
		setAddr(addr_tmp)
	return s

def delay():
	random.seed()
	r = random.randrange(-10, 10, 1)
	return r if (r > -6 and r < 6) else 0

def pushToRAM():
	haltestellen = [key for key, _ in json_data.items()]
	'''
	Writes all Tram data to the RAM-Chip.
	:param: None
	:return: None
	'''
	blockI2C() # always block the I2C bus first!
	setAddr(0)
	addrInc() # prevent `write_int` from writing to address -1 (bad)
	write_int(len(haltestellen))
	addrInc()
	for haltestelle in haltestellen: # header
		write_str(json_data.get(haltestelle)[0].ljust(32), ret=False)
		addrInc()
		write_int(len(json_data.get(haltestelle)))
		addrInc()
	for haltestelle in haltestellen: # data
		for linie in haltestelle:
			write_int(linie)
			addrInc()
			write_int(delay())
			addrInc()
	unblockI2C() # never forget to unblock the I2C bus!

def main():
	# while True:
	# 	pushToRAM()
	# 	time.sleep(20)
	blockI2C()
	setAddr(1)
	write_int(2)
	print(read_int())
	unblockI2C()

if __name__ == "__main__":
	main()