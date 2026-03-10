#include "dash.hpp"
#include <iostream>
#include <unistd.h>

Dash::Dash() {
	gpiodev = lgGpiochipOpen(0);
	lgGpioClaimOutput(gpiodev, 0, TSAL_PIN, 1);
	lgGpioClaimOutput(gpiodev, 0, IMD_PIN, 1);
	lgGpioClaimOutput(gpiodev, 0, AMS_PIN, 1);
}

Dash::~Dash() {
	if(gpiodev >= 0) {
		lgGpiochipClose(gpiodev);
	}
}

void Dash::setTSAL(bool state) {
	lgGpioWrite(gpiodev, TSAL_PIN, state);
}

void Dash::setIMD(bool state) {
	lgGpioWrite(gpiodev, IMD_PIN, !state);
}

void Dash::setAMS(bool state) {
	lgGpioWrite(gpiodev, AMS_PIN, !state);
}

void Dash::testSequence() {
	setTSAL(true);
	usleep(500000);
	setTSAL(false);
	usleep(500000);
	setIMD(true);
	usleep(500000);
	setIMD(false);
	usleep(500000);
	setAMS(true);
	usleep(500000);
	setAMS(false);
}
