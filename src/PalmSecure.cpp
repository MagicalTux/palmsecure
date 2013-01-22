#include "PalmSecure.hpp"

PalmSecure::PalmSecure() {
	qDebug("hi");
}

bool PalmSecure::open() {
	qDebug("open");
	dev = usb.getFirstDevice(0x04c5, 0x1084); // FUJITSU Imaging Device
	return false;
}

