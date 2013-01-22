#include <QObject>
#include "QUsb.hpp"

class PalmSecure: public QObject {
	Q_OBJECT;
public:
	PalmSecure();

	bool open();

private:
	QUsb usb;
	QUsbDevice *dev;
};

