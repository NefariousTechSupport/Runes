#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>

#include "RunesMainWidget.hpp"
#include "PortalAlgos.hpp"
#include "debug.hpp"
#include "RunesDebug.hpp"

int main(int argc, char *argv[])
{
	assertHatIds();

	QApplication a(argc, argv);
	a.setOrganizationName("NefariousTechSupport");
	a.setApplicationName("Runes");
	// Create the app data directory
	QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
	dir.mkdir(".");

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
