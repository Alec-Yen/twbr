#include "LiquidCrystal_I2C.h"
#include <string>
#include <ctime>
#include <cstdio>
#include <cstdlib>
using namespace std;

// get current date and time
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}


int main (int argc, char *argv []) {
	// i2c address - CHANGE IF NEEDED
	uint8_t i2c=0x27;

	// Control line PINs
	uint8_t en=2;
	uint8_t rw=1;
	uint8_t rs=0;

	// Data line PINs
	uint8_t d4=4;
	uint8_t d5=5;
	uint8_t d6=6;
	uint8_t d7=7;

	// Backlight PIN
	uint8_t bl=3;

	// LCD display size
	uint8_t rows=2;
	uint8_t cols=16;

	LiquidCrystal_I2C lcd("/dev/i2c-1", i2c, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);


	// Initialize, begin code
	lcd.begin(cols, rows);

	lcd.on();
	lcd.clear();

	// Print out time
	while (1) {
		lcd.setCursor(0,0);
		string str = currentDateTime();
		string str1 = str.substr(0,str.find('.'));
		string str2 = str.substr(str.find('.')+1);
		lcd.print(str1.c_str());
		lcd.setCursor(0,1);
		lcd.print(str2.c_str());
	}

	//lcd.noBlink();
	//lcd.noCursor();
}
