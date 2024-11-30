/*
	File:
		PortalDebuggerWidget.hpp

	Description:
		UI Widget for debugging portal support.
*/

#include "PortalDebuggerWidget.hpp"

#include "hardware/HardwareInterface.hpp"
#include "hardware/PortalDriver.hpp"

#include "RunesDebug.hpp"

#include <QPushButton>
#include <QVBoxLayout>

PortalDebuggerWidget::PortalDebuggerWidget(QWidget* parent /* = nullptr */)
: QWidget(parent)
, _driver(nullptr)
{
	_driver = new Runes::Portal::PortalDriver();

	setLayout(new QVBoxLayout);

	QPushButton* btnConnect = new QPushButton(this);
	btnConnect->setText(tr("Connect"));
	connect(btnConnect, &QPushButton::pressed, [this]()
	{
		auto error = _driver->Connect();
		RUNES_ASSERT(error == Runes::Portal::HardwareErrorCode::kHWErrNoError, "Errored!!!");
	});
	layout()->addWidget(btnConnect);

	QPushButton* btnRed = new QPushButton(this);
	btnRed->setText(tr("Colour"));
	connect(btnRed, &QPushButton::pressed, [this]()
	{
		static Runes::Portal::PortalLEDColour colours[] = {
			{ 0xFF, 0x00, 0x00 },
			{ 0x00, 0xFF, 0x00 },
			{ 0x00, 0x00, 0xFF },
			{ 0x00, 0x00, 0x00 },
		};
		static int counter = 0;

		_driver->QueueColour(colours[counter]);
		counter = (counter + 1) % 4;
	});
	layout()->addWidget(btnConnect);

	setWindowTitle(tr("Portal Debugger"));
}


PortalDebuggerWidget::~PortalDebuggerWidget()
{
	delete _driver;
}