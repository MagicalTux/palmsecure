#include "QUsb.hpp"
#include "QUsbDevice.hpp"

QUsb::QUsb() {
	int r;
	r = libusb_init(&ctx);
	if (r < 0) {
		qDebug("QUsb: init error %d", r);
		ctx = NULL;
		return;
	}
	libusb_set_debug(ctx, 3);
}

QUsbDevice *QUsb::getFirstDevice(quint16 vendor, quint16 device) {
	libusb_device_handle *dev = libusb_open_device_with_vid_pid(ctx, vendor, device);
	if (dev == NULL) return NULL;
	return new QUsbDevice(this, dev);
}

QUsb::~QUsb() {
	libusb_exit(ctx);
}

void QUsb::debugPrintDevices() {
	ssize_t cnt;
	libusb_device **devs;
	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0) {
		qDebug("QUsb: failed to list devices, error %ld", cnt);
		return;
	}
	for(ssize_t i = 0; i < cnt; i++) {
		libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(devs[i], &desc);
		if (r < 0) continue;
		if (desc.bDeviceClass == 9) continue; // SWITCH
		qDebug("Device #%ld", i);
		qDebug(" * Number of possible configurations: %d", desc.bNumConfigurations);
		qDebug(" * Device Class: %d", desc.bDeviceClass);
		qDebug(" * VendorID/ProductID: %04x:%04x", desc.idVendor, desc.idProduct);
		libusb_config_descriptor *config;
		libusb_get_config_descriptor(devs[i], 0, &config);
		qDebug(" * Interfaces: %d", config->bNumInterfaces);
		const libusb_interface *inter;
		const libusb_interface_descriptor *interdesc;
		const libusb_endpoint_descriptor *epdesc;
		for(int f=0; f<(int)config->bNumInterfaces; f++) {
			inter = &config->interface[f];
			qDebug("  * Number of alternate settings: %d", inter->num_altsetting);
			for(int j=0; j<inter->num_altsetting; j++) {
				interdesc = &inter->altsetting[j];
				qDebug("   * Interface Number: %d", interdesc->bInterfaceNumber);
				qDebug("   * Number of endpoints: %d", interdesc->bNumEndpoints);
				for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
					epdesc = &interdesc->endpoint[k];
					qDebug("    * Descriptor Type: %d", epdesc->bDescriptorType);
					qDebug("    * EP Address: %d", epdesc->bEndpointAddress);
				}
			}
		}
	}
	libusb_free_device_list(devs, 1);
}

