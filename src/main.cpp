#include "MainDialog.h"
#include <QApplication>

int main(int argc, char **argv) {
	QApplication app(argc, argv);
	MainDialog d;
	d.show();
	QObject::connect(&d, &MainDialog::rejected, qApp, &QGuiApplication::quit);
	QObject::connect(&d, &MainDialog::accepted, qApp, &QGuiApplication::quit);
	app.exec();
}
