#ifndef DASH_HPP
#define DASH_HPP
#include <lgpio.h>

class Dash {
public:
	Dash();
	~Dash();
	
	void setLV(bool state);
	void setIMD(bool state);
	void setBMS(bool state);
	void testSequence();

private:
	int gpiodev;
	const int LV_PIN = 27;
	const int IMD_PIN = 22;
	const int BMS_PIN = 17;
};
#endif
