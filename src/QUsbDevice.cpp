#include "QUsbDevice.hpp"

QUsbDevice::QUsbDevice(QUsb *parent, libusb_device_handle *_dev): usb(parent), dev(_dev) {
}

