#include <QObject>
#include <libusb.h>

class QUsb;

class QUsbDevice: public QObject {
	Q_OBJECT;
public:
	QUsbDevice(QUsb *parent, libusb_device_handle *dev);

private:
	QUsb *usb;
	libusb_device_handle *dev;
};

