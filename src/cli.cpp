#include <QCoreApplication>
#include "PalmSecure.hpp"

int main(int argc, char *argv[]) {
	QCoreApplication app(argc, argv);

	PalmSecure *p = new PalmSecure();
	if (!p->open()) return 2; // failed

	p->detect();

	return app.exec();
}
