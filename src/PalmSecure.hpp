#include <QObject>
#include <QTimer>
#include <QList>
#include <QImage>
#include "QUsb.hpp"

class PalmSecure: public QObject {
	Q_OBJECT;
public:
	PalmSecure();

	bool open();
	QList<QImage> captureLarge();
	QList<QImage> captureSmall();
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
	QImage bufToImage(const QByteArray &buf, int w, int h);
};

