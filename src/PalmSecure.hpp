#include <QObject>
#include <QTimer>
#include "QUsb.hpp"

class PalmSecure: public QObject {
	Q_OBJECT;
public:
	PalmSecure();

	bool open();
	void captureLarge();
	void captureSmall();
	QString deviceName();

public slots:
	void do_detect();
	void start();
	void stop();

private:
	QUsb usb;
	QTimer scan;
	QUsbDevice *dev;
	bool scan_first;
};

