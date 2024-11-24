/*
	File:
		PortalDebuggerWidget.hpp

	Description:
		UI Widget for debugging portal support.
*/

#include "PortalDebuggerWidget.hpp"

#include "hardware/WinHidUsbInterface.hpp"

#include "RunesDebug.hpp"

#include <QPushButton>
#include <QVBoxLayout>

PortalDebuggerWidget::PortalDebuggerWidget(QWidget* parent /* = nullptr */)
: QWidget(parent)
, _usbDevice(nullptr)
{
	setLayout(new QVBoxLayout);

	QPushButton* btnConnect = new QPushButton(this);
	btnConnect->setText(tr("Connect"));
	connect(btnConnect, &QPushButton::pressed, [this]()
	{
		_usbDevice = new Runes::Portal::WinHidUsbInterface();
		auto error = _usbDevice->connect(PORTAL_TYPE_DEFAULT);
		RUNES_ASSERT(error == Runes::Portal::HardwareErrorCode::kHWErrNoError, "Errored!!!");
	});
	layout()->addWidget(btnConnect);

	QPushButton* btnRed = new QPushButton(this);
	btnRed->setText(tr("Colour"));
	connect(btnRed, &QPushButton::pressed, [this]()
	{
		uint8_t colour[4] = { 'C', 0xFF, 0x00, 0x00 };
		_usbDevice->writeOut(colour, sizeof(colour));
	});
	layout()->addWidget(btnConnect);

	setWindowTitle(tr("Portal Debugger"));
}


PortalDebuggerWidget::~PortalDebuggerWidget()
{
	if (_usbDevice)
	{
		delete _usbDevice;
	}
}