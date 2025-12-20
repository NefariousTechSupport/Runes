#pragma once

#include <QWidget>

#include "Hardware/PortalDriver.hpp"

#include "PortalTag.hpp"

class QTabWidget;
class QVBoxLayout;
class QMenuBar;
class FigureTabWidget;
namespace Runes::Portal
{
	class PortalDriver;
}

class RunesMainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit RunesMainWidget(QWidget* parent = nullptr);

private Q_SLOTS:
	void PumpDriver();

private:
	QTabWidget* _tabs;
	std::array<FigureTabWidget*, 16> _realFigures;
	QVBoxLayout* _root;
	QMenuBar* _menuBar;
	Runes::Portal::PortalDriver* _driver;
	Runes::EventListenerID _readTagEventId;
	Runes::EventListenerID _removeTagEventId;
};