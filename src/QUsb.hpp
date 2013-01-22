#include <QObject>
#include <libusb.h>

class QUsbDevice;

class QUsb: public QObject {
	Q_OBJECT;
public:
	QUsb();
	~QUsb();
	void debugPrintDevices();
	QUsbDevice *getFirstDevice(quint16 vendor, quint16 device);

private:
	libusb_context *ctx;
};

