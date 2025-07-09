#include "RunesMainWidget.hpp"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>

#include "FigureTabWidget.hpp"
#include "PortalDebuggerWidget.hpp"
#include "PortalAlgos.hpp"

RunesMainWidget::RunesMainWidget(QWidget* parent) : QWidget(parent)
{
	QVBoxLayout* root = new QVBoxLayout(this);

	_tabs = new QTabWidget(this);
	_tabs->setTabsClosable(true);
	connect(_tabs, &QTabWidget::tabCloseRequested, [this](int index)
	{
		QWidget* widget = _tabs->widget(index);
		_tabs->removeTab(index);
		if (widget)
		{
			delete widget;
		}
	});
	root->addWidget(_tabs);

	_menuBar = new QMenuBar(this);
	QMenu* menuFile = new QMenu(tr("&File"), this);
	QAction* actOpen = new QAction(tr("&Open"), this);
	actOpen->setShortcut(QKeySequence::Open);
	actOpen->setStatusTip(tr("Open a Dump"));
	connect(actOpen, &QAction::triggered, [=]()
	{
		QString sourceFile = QFileDialog::getOpenFileName(this, tr("Open Dump File"), "", tr("All Files (*.*)"));

		if (!sourceFile.isEmpty())
		{
			Runes::PortalTag* tag = new Runes::PortalTag();
			tag->_rfidTag = new Runes::RfidTag();
			tag->ReadFromFile(sourceFile.toLocal8Bit());
			int tabIndex = this->_tabs->addTab(new FigureTabWidget(tag, sourceFile.toLocal8Bit(), _tabs), tr("Figure File"));
			this->_tabs->setCurrentIndex(tabIndex);
		}
	});
	menuFile->addAction(actOpen);
	QAction* actSave = new QAction(tr("&Save"), this);
	actSave->setShortcut(QKeySequence::Save);
	actSave->setStatusTip(tr("Save a Dump"));
	connect(actSave, &QAction::triggered, [=]()
	{
		FigureTabWidget* figureWidget = (FigureTabWidget*)this->_tabs->currentWidget();
		figureWidget->_tag->SaveToFile(QFileDialog::getSaveFileName(this, tr("Save Dump File"), "", tr("All Files (*.*)")).toLocal8Bit());
	});
	menuFile->addAction(actSave);
	_menuBar->addMenu(menuFile);

	QMenu* menuDev = new QMenu(tr("&Developer"), this);
	QAction* actPortal = new QAction(tr("&Portal"), this);
	actPortal->setStatusTip(tr("Portal Debugger"));
	connect(actPortal, &QAction::triggered, [=]()
	{
		PortalDebuggerWidget* portalWindow = new PortalDebuggerWidget();
		portalWindow->show();
		portalWindow->raise();
		portalWindow->activateWindow();
	});
	menuDev->addAction(actPortal);
	_menuBar->addMenu(menuDev);

	setLayout(root);
	layout()->setMenuBar(_menuBar);
	setWindowTitle(tr("Runes"));
}