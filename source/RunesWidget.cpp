#include "RunesWidget.hpp"

#include <QFormLayout>
#include <QSpinBox>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>

#include "kTfbSpyroTag_HatType.hpp"
#include "Constants.hpp"

RunesWidget::RunesWidget(Runes::PortalTag* tag, char* fileName, QWidget* parent) : QWidget(parent)
{
    this->_tag = tag;
    this->_sourceFile = QString(fileName);

    this->_spinExp = new QSpinBox(this);
    this->_spinExp->setRange(0, 197500);
    connect(this->_spinExp, &QSpinBox::valueChanged, [=](int newExp)
    {
        this->_tag->_exp = newExp;
    });

    this->_spinMoney = new QSpinBox(this);
    this->_spinMoney->setRange(0, kMoneyCap);
    connect(this->_spinMoney, &QSpinBox::valueChanged, [=](int newMoney)
    {
        this->_tag->_coins = newMoney;
    });

    this->_spinHeroPoints = new QSpinBox(this);
    this->_spinHeroPoints->setRange(0, 100);
    connect(this->_spinHeroPoints, &QSpinBox::valueChanged, [=](int newHeroPoints)
    {
        this->_tag->_heroPoints = newHeroPoints;
    });

    QFormLayout* layout = new QFormLayout(this);
    layout->addRow(tr("Experience"), this->_spinExp);
    layout->addRow(tr("Money"), this->_spinMoney);
    layout->addRow(tr("Hero Points"), this->_spinHeroPoints);

    QMenuBar* menubar = new QMenuBar(this);
    QMenu* menuFile = new QMenu(tr("&File"), this);
    QAction* actOpen = new QAction(tr("&Open"), this);
    actOpen->setShortcut(QKeySequence::Open);
    actOpen->setStatusTip(tr("Open a Dump"));
    connect(actOpen, &QAction::triggered, [=]()
    {
        this->_sourceFile = QFileDialog::getOpenFileName(this, tr("Open Dump File"), "", tr("All Files (*.*)"));

        this->_tag->ReadFromFile(_sourceFile.toLocal8Bit());

        this->updateFields();
    });
    menuFile->addAction(actOpen);
    QAction* actSave = new QAction(tr("&Save"), this);
    actSave->setShortcut(QKeySequence::Save);
    actSave->setStatusTip(tr("Save a Dump"));
    connect(actSave, &QAction::triggered, [=]()
    {
        this->_tag->SaveToFile(QFileDialog::getSaveFileName(this, tr("Save Dump File"), "", tr("All Files (*.*)")).toLocal8Bit());

        this->updateFields();
    });
    menuFile->addAction(actSave);
    menubar->addMenu(menuFile);

    this->layout()->setMenuBar(menubar);

    setLayout(layout);
    
    setWindowTitle(tr("Runes"));

    updateFields();
}
void RunesWidget::updateFields()
{
    this->_spinExp->setValue(this->_tag->_exp);
    this->_spinMoney->setValue(this->_tag->_coins);
    this->_spinHeroPoints->setValue(this->_tag->_heroPoints);
}