#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QString>

#include "RunesMainWidget.hpp"
#include "PortalAlgos.hpp"
#include "debug.hpp"

int main(int argc, char *argv[])
{
	assertHatIds();

	QApplication a(argc, argv);

	std::optional<std::string> result = Runes::readSalt();
	if (result.has_value())
	{
		QMessageBox::critical(
			nullptr,
			QString("salt.txt error"),
			QString(result.value().c_str()),
			QMessageBox::StandardButton::Abort,
			QMessageBox::StandardButton::NoButton);

		a.exit(1);
		return 1;
	}
	else
	{
		RunesMainWidget rw = RunesMainWidget(nullptr);
		rw.show();

		return a.exec();
	}
}
