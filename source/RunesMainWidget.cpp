#include "RunesMainWidget.hpp"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>

#include "Hardware/PortalDriver.hpp"

#include "FigureTabWidget.hpp"
#include "PortalDebuggerWidget.hpp"
#include "PortalAlgos.hpp"
#include "RunesDebug.hpp"

RunesMainWidget::RunesMainWidget(QWidget* parent)
: QWidget(parent)
, _tabs(nullptr)
, _realFigures()
, _root(nullptr)
, _menuBar(nullptr)
, _driver(nullptr)
, _readTagEventId(Runes::kInvalidEventListenerID)
, _removeTagEventId(Runes::kInvalidEventListenerID)
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
			int tabIndex = this->_tabs->addTab(new FigureTabWidget(tag, _tabs), tr("Figure File"));
			this->_tabs->setCurrentIndex(tabIndex);
		}
	});
	menuFile->addAction(actOpen);
	QAction* actSave = new QAction(tr("&Save"), this);
	actSave->setShortcut(QKeySequence::Save);
	actSave->setStatusTip(tr("Save a Dump"));
	connect(actSave, &QAction::triggered, [=]()
	{
		FigureTabWidget* figureWidget = static_cast<FigureTabWidget*>(this->_tabs->currentWidget());
		if (figureWidget != nullptr)
		{
			figureWidget->_tag->SaveToFile(QFileDialog::getSaveFileName(this, tr("Save Dump File"), "", tr("All Files (*.*)")).toLocal8Bit());
		}
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

	_driver = new Runes::Portal::PortalDriver();
	Runes::Portal::HardwareErrorCode errorCode = _driver->Connect();
	if (errorCode == Runes::Portal::kHWErrNoError)
	{
		_readTagEventId = _driver->GetTagReadFinishedEvent().AddListener([=](uint8_t index, Runes::PortalTag& newTag)
		{
			FigureTabWidget* widget = new FigureTabWidget(&newTag, _tabs);
			_realFigures[index] = widget;
			int tabIndex = this->_tabs->addTab(widget, QString("Real Figure %1").arg(index));
			this->_tabs->setCurrentIndex(tabIndex);
		});

		_removeTagEventId = _driver->GetTagRemovedEvent().AddListener([=](uint8_t index)
		{
			FigureTabWidget* widget = _realFigures[index];

			this->_tabs->removeTab(this->_tabs->indexOf(widget));
		});
	}

	QTimer* driverTimer = new QTimer(this);
	connect(driverTimer, SIGNAL(timeout()), this, SLOT(PumpDriver()));
	driverTimer->start(100);
}

void RunesMainWidget::PumpDriver()
{
	_driver->Pump();
}