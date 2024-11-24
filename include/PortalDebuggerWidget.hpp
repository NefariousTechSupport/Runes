/*
	File:
		PortalDebuggerWidget.hpp

	Description:
		UI Widget for debugging portal support.
*/

#ifndef RUNES_PORTAL_DEBUGGER_WIDGET_HPP
#define RUNES_PORTAL_DEBUGGER_WIDGET_HPP

#include <QWidget>

namespace Runes::Portal
{
	class HardwareInterface;
}

class QPushButton;

class PortalDebuggerWidget : public QWidget
{
	Q_OBJECT
public:
	explicit PortalDebuggerWidget(QWidget* parent = nullptr);
	virtual ~PortalDebuggerWidget();

private:
	Runes::Portal::HardwareInterface*  _usbDevice;
};

#endif // RUNES_PORTAL_DEBUGGER_WIDGET_HPP