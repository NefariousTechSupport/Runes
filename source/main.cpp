#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include "RunesMainWidget.hpp"
#include "PortalTag.hpp"
#include "debug.hpp"

int main(int argc, char *argv[])
{
	assertHatIds();

	QApplication a(argc, argv);

	Runes::PortalTag tag;
	tag._rfidTag = new Runes::RfidTag();
	tag.ReadFromFile(argv[1]);

	RunesMainWidget rw = RunesMainWidget(&tag, argv[1], nullptr);
	rw.show();

	return a.exec();
}