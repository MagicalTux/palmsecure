#include <QCoreApplication>
#include "PalmSecure.hpp"

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);

	PalmSecure *p = new PalmSecure();
	p->open();

	return app.exec();
}
