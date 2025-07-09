#pragma once

#include <QWidget>

#include "PortalTag.hpp"

class QTabWidget;
class QVBoxLayout;
class QMenuBar;

class RunesMainWidget : public QWidget
{
	Q_OBJECT
public:
	explicit RunesMainWidget(QWidget* parent = nullptr);
	QTabWidget* _tabs;
	QVBoxLayout* _root;
	QMenuBar* _menuBar;
};