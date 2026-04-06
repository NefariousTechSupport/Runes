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
, _tagPlacedEventId(Runes::kInvalidEventListenerID)
, _tagReadFinishedEventId(Runes::kInvalidEventListenerID)
, _tagRemovedEventId(Runes::kInvalidEventListenerID)
, _tagReadUpdateEventId(Runes::kInvalidEventListenerID)
, _tagWriteCompleteEventId(Runes::kInvalidEventListenerID)
, _tagWriteCancelledEventId(Runes::kInvalidEventListenerID)
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
			FigureTabWidget* widget = new FigureTabWidget(_tabs);
			int tabIndex = this->_tabs->addTab(widget, tr("Figure File"));

			Runes::PortalTag* tag = new Runes::PortalTag();
			tag->_rfidTag = new Runes::RfidTag(false);
			tag->ReadFromFile(sourceFile.toLocal8Bit());

			widget->Initialize(tag);

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

		if (!figureWidget
		 || !figureWidget->_tag
		 || !figureWidget->_tag->_rfidTag)
		{
			// Whoops
			return;
		}

		bool writeToFigure = figureWidget->_tag->_rfidTag->fromFigure();
		QString userPath;
		if (!writeToFigure)
		{
			userPath = QFileDialog::getSaveFileName(this, tr("Save Dump File"), "", tr("All Files (*.*)"));
		}

		if (figureWidget != nullptr && (!userPath.isEmpty() || writeToFigure))
		{
			QString backupPath;
			figureWidget->StartSave(backupPath, writeToFigure);

			if (!userPath.isEmpty())
			{
				QFile backup = QFile(backupPath);
				backup.copy(userPath);
			}

			if (writeToFigure)
			{
				auto iter = std::find(_realFigures.begin(), _realFigures.end(), figureWidget);
				size_t index = std::distance(_realFigures.begin(), iter);
				_driver->QueueWrite(index);
			}
			figureWidget->_tag->SaveToFile(userPath.toLocal8Bit());
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

	_tagPlacedEventId = _driver->GetTagPlacedEvent().AddListener([=](uint8_t index)
	{
		FigureTabWidget* widget = new FigureTabWidget(_tabs);
		_realFigures[index] = widget;

		// start disabled
		widget->setDisabled(true);

		int tabIndex = this->_tabs->addTab(widget, QString("Real Figure %1").arg(index));
		this->_tabs->setCurrentIndex(tabIndex);
	});

	_tagReadFinishedEventId = _driver->GetTagReadFinishedEvent().AddListener([=](uint8_t index, Runes::PortalTag& newTag)
	{
		FigureTabWidget* widget = _realFigures[index];

		// enable now that the figure's been read
		widget->setDisabled(false);

		widget->Initialize(&newTag);
	});

	_tagRemovedEventId = _driver->GetTagRemovedEvent().AddListener([=](uint8_t index)
	{
		FigureTabWidget* widget = _realFigures[index];

		// Prevent double free
		widget->_tag = nullptr;

		this->_tabs->removeTab(this->_tabs->indexOf(widget));
	});

	_tagReadUpdateEventId = _driver->GetTagReadUpdateEvent().AddListener([=](uint8_t index, uint8_t progress)
	{
		FigureTabWidget* widget = _realFigures[index];

		widget->UpdateProgress(progress);
	});

	_tagWriteCompleteEventId = _driver->GetTagWriteCompleteEvent().AddListener([=](uint8_t index)
	{
		FigureTabWidget* widget = _realFigures[index];

		widget->setEnabled(true);
	});

	_tagWriteCancelledEventId = _driver->GetTagWriteCancelledEvent().AddListener([=](uint8_t index)
	{
		QMessageBox::warning(
			this,
			tr("Figure Removed During Write"),
			tr("A figure was removed while it was being written to, this is a bad idea normally.\nPlease do not do this again.\nIt's possible your figure is corrupted but it can be recovered using a backup by placing the figure on the portal again."),
			QMessageBox::StandardButton::Ok,
			QMessageBox::StandardButton::NoButton
		);
	});

	QTimer* driverTimer = new QTimer(this);
	connect(driverTimer, SIGNAL(timeout()), this, SLOT(PumpDriver()));
	driverTimer->start(50);
}

RunesMainWidget::~RunesMainWidget()
{
	for (int f = 0; f < _realFigures.size(); f++)
	{
		if (_realFigures[f])
		{
			// This tag doesn't belong to this figure tab widget so unset it before things go south
			_realFigures[f]->_tag = nullptr;
		}
	}

	_driver->GetTagPlacedEvent().RemoveListener(_tagPlacedEventId);
	_driver->GetTagReadFinishedEvent().RemoveListener(_tagReadFinishedEventId);
	_driver->GetTagRemovedEvent().RemoveListener(_tagRemovedEventId);
	_driver->GetTagReadUpdateEvent().RemoveListener(_tagReadUpdateEventId);
	_driver->GetTagWriteCompleteEvent().RemoveListener(_tagWriteCompleteEventId);
	_driver->GetTagWriteCancelledEvent().RemoveListener(_tagWriteCancelledEventId);
}

void RunesMainWidget::PumpDriver()
{
	_driver->Pump();
}