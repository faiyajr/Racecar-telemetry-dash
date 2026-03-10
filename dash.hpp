#ifndef DASH_HPP
#define DASH_HPP
#include <lgpio.h>

class Dash {
public:
	Dash();
	~Dash();
	
	void setTSAL(bool state);
	void setIMD(bool state);
	void setAMS(bool state);
	void testSequence();

private:
	int gpiodev;
	const int TSAL_PIN = 27;
	const int IMD_PIN = 22;
	const int AMS_PIN = 17;
};
#endif
