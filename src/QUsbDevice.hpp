#include <QObject>
#include <QIODevice>
#include <libusb.h>

class QUsb;

class QUsbDevice: public QObject {
	Q_OBJECT;
public:
	QUsbDevice(QUsb *parent, libusb_device_handle *dev);
	~QUsbDevice();
	bool setConfiguration(int configuration);
	bool claim(int interface = 0);
	bool release(int interface = 0);
	bool reset();
	bool setAltSetting(int interface, int alt);

	// control transfer
	QByteArray controlTransfer(quint8 bmRequestType, quint8 bRequest, quint16 wValue, quint16 wIndex, quint16 wLength, unsigned int timeout = 1000);

	// bulk transfer
	qint64 bulkSendFile(int endpoint, const QString &file);
	qint64 bulkSendFile(int endpoint, QIODevice &file);

private:
	QUsb *usb;
	libusb_device_handle *dev;
	bool handleLibUsbRes(int);
};

