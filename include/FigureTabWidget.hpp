#pragma once

#include <QWidget>

#include "PortalTag.hpp"

class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;
class QFormLayout;
class QTextEdit;

class FigureTabWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FigureTabWidget(Runes::PortalTag* tag, const char* fileName, QWidget* parent = nullptr);
	Runes::PortalTag* _tag;

private:
	QString _sourceFile;

	void updateFields();
	void initGiantsQuests();
	void initSwapForceQuests();

	QSpinBox* _spinExp;
	QSpinBox* _spinMoney;
	QSpinBox* _spinHeroPoints;
	QComboBox* _cmbHat;
	QTextEdit* _txtNickname;

	QLabel* _lblToyName;
	QLabel* _lblTimePlayed;
	QLabel* _lblLevel;
	QLabel* _lblFirstTouched;
	QLabel* _lblRecentlyTouched;

	QLabel* _sgInvalidElement1;
	QLabel* _sgInvalidElement2;
	QLabel* _ssfInvalidElement1;
	QLabel* _ssfInvalidElement2;

	QFormLayout* _subGiantsQuests;
	QSpinBox* _spinGiantsMonsterMasher;
	QSpinBox* _spinGiantsBattleChamp;
	QSpinBox* _spinGiantsChowHound;
	QCheckBox* _chkGiantsHeroicChallenger;
	QCheckBox* _chkGiantsArenaArtist;
	QSpinBox* _spinGiantsElementalist;
	QWidget* _wdGiantsElementalQuest1;
	QWidget* _wdGiantsElementalQuest2;
	QSpinBox* _spinGiantsIndividualQuest;

	QFormLayout* _subSwapForceQuests;
	QSpinBox* _spinSwapForceBadguyBasher;
	QSpinBox* _spinSwapForceFruitFrontiersman;
	QCheckBox* _chkSwapForceFlawlessChallenger;
	QSpinBox* _spinSwapForceTrueGladiator;
	QCheckBox* _chkSwapForceTotallyMaxedOut;
	QSpinBox* _spinSwapForceElementalist;
	QWidget* _wdSwapForceElementalQuest1;
	QWidget* _wdSwapForceElementalQuest2;
	QSpinBox* _spinSwapForceIndividual;
};