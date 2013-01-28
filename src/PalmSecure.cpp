#include "PalmSecure.hpp"
#include "QUsbDevice.hpp"
#include <QFile>

PalmSecure::PalmSecure() {
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

	// switch off all lights?
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

	return true;
}

void PalmSecure::detect() {
	qDebug("PalmSecure: capture");

	// switch on light?
	dev->controlTransfer(0xc0, 0x27, 0x07, 1, 8); // returns 2707000000000000
	dev->controlTransfer(0xc0, 0x27, 0x08, 1, 8); // returns 2708000000000000
	dev->controlTransfer(0xc0, 0x27, 0x00, 1, 6); // returns 270000280000

	while(true) {
		QByteArray val1 = dev->controlTransfer(0xc0, 0x4d, 0x78, 240, 5); // returns 4d01005802
		QByteArray val2 = dev->controlTransfer(0xc0, 0x58, 0xffce, 0xf0f, 56); // returns 5801000000000808080808090809090808080809080908090808090807080808080808080808080909080908080908080808080808080800
		qDebug("CHECK [%s] [%s]", val1.toHex().constData(), val2.toHex().constData());
		int d[4];
		for(int i = 0; i < 4; i++) d[i] = (unsigned char)val2.at(2+i);
		qDebug("DIST VALUE=%d,%d,%d,%d", d[0], d[1], d[2], d[3]);
		bool ok = true;
		for(int i = 0; i < 4; i++) if ((d[i] < 40) || (d[i] > 50)) { ok = false; break; }
		if (ok) {
			captureLarge();
			return;
		}
		usleep(20000);
	}
}

void PalmSecure::captureLarge() {
	qDebug("Capture!");
	dev->controlTransfer(0xc0, 0x4e, 0, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 1, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 2, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 3, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x46, 0x5d0, 0, 3); // returns 460100
	dev->controlTransfer(0xc0, 0x47, 0x10, 0, 3); // returns 470100
	dev->controlTransfer(0xc0, 0x49, 0x100, 0, 3); // returns 490100
	dev->controlTransfer(0xc0, 0x4a, 0x78, 240, 5); // returns 4a01005802 - image size related?
	dev->controlTransfer(0xc0, 0x46, 0xc8, 3, 3); // returns 460100
	dev->controlTransfer(0xc0, 0x47, 0x10, 3, 3); // returns 470100
	dev->controlTransfer(0xc0, 0x49, 0x100, 3, 3); // returns 490100
	dev->controlTransfer(0xc0, 0x42, 0x100, 2, 3); // returns 420100
	dev->controlTransfer(0xc0, 0x43, 0, 0, 3); // returns 430100
	dev->controlTransfer(0xc0, 0x4a, 0, 480, 5); // returns 4a0100b004 - image height?
	dev->controlTransfer(0xc0, 0x44, 0, 0, 6); // returns 440100b00400
	qDebug("Capture 1");
	QByteArray dat1 = dev->bulkReceive(2, 307200); // vein data
	QFile f("data1.dat"); f.open(QIODevice::WriteOnly | QIODevice::Truncate); f.write(dat1); f.close();
	dev->controlTransfer(0xc0, 0x44, 0, 1, 6); // returns 440100b00400
	qDebug("Capture 2");
	QByteArray dat2 = dev->bulkReceive(2, 307200); // normal picture
	QFile f2("data2.dat"); f2.open(QIODevice::WriteOnly | QIODevice::Truncate); f2.write(dat2); f2.close();
	dev->controlTransfer(0xc0, 0x4d, 0x78, 240, 5); // returns 4d01005802
	dev->controlTransfer(0xc0, 0x44, 3, 2, 6); // returns 440100580200
	qDebug("Capture 3");
	QByteArray dat3 = dev->bulkReceive(2, 153600); // "4 dots"

	// OK, write to file
	QFile f3("data3.dat"); f3.open(QIODevice::WriteOnly | QIODevice::Truncate); f3.write(dat3); f3.close();

	// switch off light?
	dev->controlTransfer(0xc0, 0x27, 7, 1, 8); // returns 2707000000000000
	dev->controlTransfer(0xc0, 0x27, 8, 1, 8); // returns 2708000000000000
	dev->controlTransfer(0xc0, 0x27, 0, 1, 6); // returns 270000280000
}

void PalmSecure::captureSmall() {
	dev->controlTransfer(0xc0, 0x4e, 0, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 1, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 2, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x4e, 3, 0, 3); // returns 4e0100
	dev->controlTransfer(0xc0, 0x46, 0x7b7, 2, 3); // returns 460100
	dev->controlTransfer(0xc0, 0x47, 0x10, 2, 3); // returns 470100
	dev->controlTransfer(0xc0, 0x49, 0x100, 2, 3); // returns 490100
	dev->controlTransfer(0xc0, 0x4c, 0xc0, 96, 5); // returns 4c0100f000 - image height?
	dev->controlTransfer(0xc0, 0x46, 0xc8, 3, 3); // returns 460100
	dev->controlTransfer(0xc0, 0x47, 0x10, 3, 3); // returns 470100
	dev->controlTransfer(0xc0, 0x49, 0x100, 3, 3); // returns 490100
	dev->controlTransfer(0xc0, 0x42, 0, 258, 3); // returns 420100
	dev->controlTransfer(0xc0, 0x43, 0, 0, 3); // returns 430100
	dev->controlTransfer(0xc0, 0x4c, 0xc0, 96, 5); // returns 4c0100f000 - image height?
	dev->controlTransfer(0xc0, 0x44, 2, 0, 6); // returns 440100f00000
	qDebug("Capture 4");
	QByteArray dat4 = dev->bulkReceive(2, 61440);
	QFile f4("data4.dat"); f4.open(QIODevice::WriteOnly | QIODevice::Truncate); f4.write(dat4); f4.close();
	dev->controlTransfer(0xc0, 0x44, 2, 1, 6); // returns 440100f00000
	qDebug("Capture 5");
	QByteArray dat5 = dev->bulkReceive(2, 61440);
	QFile f5("data5.dat"); f5.open(QIODevice::WriteOnly | QIODevice::Truncate); f5.write(dat5); f5.close();
	dev->controlTransfer(0xc0, 0x4d, 0x78, 240, 5); // returns 4d01005802
	dev->controlTransfer(0xc0, 0x44, 3, 2, 6); // returns 440100580200
	qDebug("Capture 6");
	QByteArray dat6 = dev->bulkReceive(2, 153600);
	QFile f6("data6.dat"); f6.open(QIODevice::WriteOnly | QIODevice::Truncate); f6.write(dat6); f6.close();

	// switch off light?
	dev->controlTransfer(0xc0, 0x27, 7, 1, 8); // returns 2707000000000000
	dev->controlTransfer(0xc0, 0x27, 8, 1, 8); // returns 2708000000000000
	dev->controlTransfer(0xc0, 0x27, 0, 1, 6); // returns 270000280000
}

