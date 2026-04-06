#include <QtCore/QCoreApplication>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QString>

#include "RunesMainWidget.hpp"
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

	RunesMainWidget rw = RunesMainWidget(nullptr);
	rw.show();

	return a.exec();
}
