#include "QUsbDevice.hpp"
#include <QFile>

QUsbDevice::QUsbDevice(QUsb *parent, libusb_device_handle *_dev): usb(parent), dev(_dev) {
}

QUsbDevice::~QUsbDevice() {
	libusb_close(dev);
}

bool QUsbDevice::setConfiguration(int config) {
	return handleLibUsbRes(libusb_set_configuration(dev, config));
}

bool QUsbDevice::claim(int interface) {
	return handleLibUsbRes(libusb_claim_interface(dev, interface));
}

bool QUsbDevice::release(int interface) {
	return handleLibUsbRes(libusb_release_interface(dev, interface));
}

bool QUsbDevice::reset() {
	return handleLibUsbRes(libusb_reset_device(dev));
}

bool QUsbDevice::setAltSetting(int interface, int alt) {
	return handleLibUsbRes(libusb_set_interface_alt_setting(dev, interface, alt));
}

bool QUsbDevice::handleLibUsbRes(int res) {
	if (res == 0) return true;
	qDebug("QUsbDevice: error %d (%s)", res, libusb_error_name(res));
	return false;
}

QByteArray QUsbDevice::controlTransfer(quint8 bmRequestType, quint8 bRequest, quint16 wValue, quint16 wIndex, quint16 wLength, unsigned int timeout) {
	QByteArray buf(wLength, 0);
	int res = libusb_control_transfer(dev, bmRequestType, bRequest, wValue, wIndex, (unsigned char*)buf.data(), wLength, timeout);
	if (res < 0) {
		handleLibUsbRes(res);
		return QByteArray();
	}
	if (res != wLength) buf.resize(res);
	return buf;
}

qint64 QUsbDevice::bulkSendFile(int endpoint, const QString &file) {
	QFile f(file);
	if (!f.open(QIODevice::ReadOnly)) {
		qDebug("QUsbDevice: failed to open file %s", qPrintable(file));
		return -1;
	}
	qint64 res = bulkSendFile(endpoint, f);
	f.close();
	return res;
}

qint64 QUsbDevice::bulkSendFile(int endpoint, QIODevice &file) {
	qint64 final = 0;
	while(true) {
		QByteArray data = file.read(16384); // usually good enough to read
		if (data.isNull()) break;
		int tx = 0;
		int res = libusb_bulk_transfer(dev, endpoint & 0x7f, (unsigned char*)data.data(), data.length(), &tx, 0);
		if (res < 0) {
			qDebug("QUsbDevice: failed write");
			handleLibUsbRes(res);
			return -1;
		}
		if (tx != data.length()) {
			qDebug("QUsbDevice: failed to write all the data");
			return -1;
		}
		final += tx;
	}
	return final;
}

QByteArray QUsbDevice::bulkReceive(int endpoint, qint64 len) {
	QByteArray final;
	while(len > 0) {
		QByteArray data((len > 16384)?16384:len, '\0');
		int tx = 0;
		int res = libusb_bulk_transfer(dev, endpoint | 0x80, (unsigned char*)data.data(), data.length(), &tx, 0);
		if (res < 0) {
			qDebug("QUsbDevice: failed write");
			handleLibUsbRes(res);
			return QByteArray();
		}
		data.truncate(tx);
		final.append(data);
		len -= tx;
	}
	return final;
}

