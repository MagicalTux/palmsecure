#include <QObject>
#include "QUsb.hpp"

class PalmSecure: public QObject {
	Q_OBJECT;
public:
	PalmSecure();

	bool open();
	void capture();
	QString deviceName();

private:
	QUsb usb;
	QUsbDevice *dev;
};

