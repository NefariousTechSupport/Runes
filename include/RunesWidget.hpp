#pragma once

#include <QWidget>

#include "PortalTag.hpp"

class QSpinBox;
class QComboBox;

class RunesWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RunesWidget(Runes::PortalTag* tag, char* fileName, QWidget* parent = nullptr);

private:
    Runes::PortalTag* _tag;
    QString _sourceFile;

    void updateFields();

    QSpinBox* _spinExp;
    QSpinBox* _spinMoney;
    QSpinBox* _spinHeroPoints;
    QComboBox* _cmbHat;
};