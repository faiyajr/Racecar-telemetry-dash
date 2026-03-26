#include "LED.hpp"
#include <iostream>
#include <unistd.h>

Dash::Dash() {
	gpiodev = lgGpiochipOpen(0);
	lgGpioClaimOutput(gpiodev, 0, LV_PIN, 1);
	lgGpioClaimOutput(gpiodev, 0, IMD_PIN, 1);
	lgGpioClaimOutput(gpiodev, 0, BMS_PIN, 1);
}

Dash::~Dash() {
	if(gpiodev >= 0) {
		lgGpiochipClose(gpiodev);
	}
}

void Dash::setLV(bool state) {
	lgGpioWrite(gpiodev, LV_PIN, state);
}

void Dash::setIMD(bool state) {
	lgGpioWrite(gpiodev, IMD_PIN, !state);
}

void Dash::setBMS(bool state) {
	lgGpioWrite(gpiodev, BMS_PIN, !state);
}

void Dash::testSequence() {
	setLV(true);
	usleep(500000);
	setLV(false);
	usleep(500000);
	setIMD(true);
	usleep(500000);
	setIMD(false);
	usleep(500000);
	setBMS(true);
	usleep(500000);
	setBMS(false);
}
