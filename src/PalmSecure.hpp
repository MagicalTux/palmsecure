#include <QObject>
#include "QUsb.hpp"

class PalmSecure: public QObject {
	Q_OBJECT;
public:
	PalmSecure();

	bool open();
	void detect();
	void captureLarge();
	void captureSmall();
	QString deviceName();

private:
	QUsb usb;
	QUsbDevice *dev;
};

