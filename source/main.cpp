#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include "RunesWidget.hpp"
#include "PortalTag.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Runes::PortalTag tag;
	tag._rfidTag = new Runes::RfidTag();
    tag.ReadFromFile(argv[1]);

    RunesWidget rw = RunesWidget(&tag, argv[1]);
    rw.show();

    return a.exec();
}