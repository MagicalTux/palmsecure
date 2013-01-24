#include "PalmSecure.hpp"
#include "QUsbDevice.hpp"

PalmSecure::PalmSecure() {
	qDebug("hi");
}

QString PalmSecure::deviceName() {
	if (dev == NULL) return QString();
	QByteArray name = dev->controlTransfer(0xc0, 0x28, 0, 21, 21);
	int idx = name.indexOf('\0');
	if (idx != -1) name.truncate(idx);
	return QString::fromLatin1(name);
}

bool PalmSecure::open() {
	dev = usb.getFirstDevice(0x04c5, 0x1084); // FUJITSU Imaging Device
	if (dev == NULL) {
		qDebug("PalmSecure: failed to open device");
		return false;
	}
	if (!dev->setConfiguration(1)) {
		delete dev;
		return false;
	}
	if (!dev->claim()) {
		delete dev;
		return false;
	}
	//QByteArray controlTransfer(quint8 bmRequestType, quint8 bRequest, quint16 wValue, quint16 wIndex, quint16 wLength, unsigned int timeout = 1000);
	dev->controlTransfer(0xc0, 0xa0, 0, 0, 3); // returns a000cb
	dev->controlTransfer(0xc0, 0x29, 0, 0, 4); // returns 29000300 (probably means "flag value is 3")
	dev->controlTransfer(0xc0, 0x66, 0, 0, 3); // returns 660000
	dev->controlTransfer(0xc0, 0xa3, 0, 0, 4); // returns a392cd01
	qDebug("PalmSecure: Connected to device %s", qPrintable(deviceName()));
	dev->controlTransfer(0xc0, 0x29, 1, 4, 4); // returns 29010400 (probably means "flag value is 4")

	// We initialize device's random generator with this method, it seems
	// file in lib/mask_2696b463.bin matches the required data for this seed
	QByteArray key = dev->controlTransfer(0xc0, 0x35, 0x2696, 0x63b4, 18); // returns 3500f15d482bb74e9c3f92b3f6cc10f9360d
	if (key != QByteArray::fromHex("3500f15d482bb74e9c3f92b3f6cc10f9360d")) {
		qDebug("PalmSecure: This device doesn't work like we believed it would");
		delete dev;
		return false;
	}
	dev->controlTransfer(0xc0, 0x36, 0, 0, 3);
	qint64 res = dev->bulkSendFile(0x01, "lib/mask_2696b463.bin");
	qDebug("PalmSecure: Wrote %lld bytes to device", res);
	if (res == -1) {
		delete dev;
		return false;
	}

	dev->controlTransfer(0xc0, 0x29, 1, 0, 4); // returns 29010400 (probably means something)
	dev->controlTransfer(0xc0, 0xf6, 9, 0, 168); // returns a400000002000000d0051000b7071000d7a3703d0ad7fd3fd7a3703d0ad7a33f6666666666565e401c041000f80410003108ac1c5a640040367689eaad81ad3f713d0ad7a3605540e80210004b031000f0a7c64b378901404faf94658863b53f52b81e85eb314e400e0210003e021000fed478e9263102406002b7eee6a9be3fae47e17a144e4540740110008c011000cff753e3a59b024024624a24d1cbc43f0ad7a3703d0a3e40
	dev->controlTransfer(0xc0, 0xf6, 8, 0, 152); // returns 94000000010000000000000000000040000000000000324000000000000084400000000000007e40fa7e6abc7493783f0038b1e02533833fccc5bd1f468d54bf00001ae91ded883f00ac11642c67ed3f4ba5ca32f867b0bfc42951a36f36793f34fe9ffa4662f43fe3d40ddaa64bd53f9b186be9afb2d9bf693754c7b3e55240aee258d2929a67bf504164df91a63dbf953ead43900be9bf
	dev->controlTransfer(0xc0, 0xf6, 7, 0, 302); // returns 3401000001000000040000000000000061e849533fe226c0190e2d82fc2827c00000000000000000094155c358418b3f60805047ad44513fd6fd521d45ffef3f7eb6d422de042740cdccd212dc4a27c00000000000000000433ed92b619f7b3f0f25c8d5ce9661bf3816387acbffef3f29a96ce37edc26c072cc748eef102740000000000000000082f2c7d86e53933f9a4c9416e67b8a3f77fc4916dbfdef3fac1796de70ed2640e428b29c3ce02640000000000000000033f14bd221639f3ffd970612085827bf5f59a39326fcef3f020004004201f0004201ef004301f5004801f20000000000d2a12dffc12342400901b700f800a6007b01b6008c01a5000a012e01fa003e0180012a0191013b01bff27cd4352453402901d7001d01cb005b01d6006701ca002a010e011f01
	dev->controlTransfer(0xc0, 0x27, 0, 0, 6); // returns 270000000000
	dev->controlTransfer(0xc0, 0x27, 0x05, 0, 8); // returns 2705000000000000
	dev->controlTransfer(0xc0, 0x27, 0x06, 0, 8); // returns 2706000000000000
	dev->controlTransfer(0xc0, 0x27, 0x07, 0, 8); // returns 2707000000000000
	dev->controlTransfer(0xc0, 0x27, 0x08, 0, 8); // returns 2708000000000000
	dev->controlTransfer(0xc0, 0x27, 0x09, 0, 8); // returns 2709000000000000
	dev->controlTransfer(0xc0, 0x27, 0x0a, 0, 8); // returns 270a000000000000
	dev->controlTransfer(0xc0, 0x27, 0x0b, 0, 8); // returns 270b000000000000
	dev->controlTransfer(0xc0, 0x27, 0x0c, 0, 8); // returns 270c000000000000
	qDebug("PalmSecure: init done");

	return false;
}

