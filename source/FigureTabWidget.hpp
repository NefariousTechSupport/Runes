#pragma once

#include <QWidget>

#include "PortalTag.hpp"

class QSpinBox;
class QComboBox;
class QCheckBox;
class QLabel;
class QListWidget;
class QFormLayout;
class QTextEdit;

class FigureTabWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FigureTabWidget(Runes::PortalTag* tag, const char* fileName, QWidget* parent = nullptr);
	virtual ~FigureTabWidget();
	Runes::PortalTag* _tag;

private:
	QString _sourceFile;

	void updateFields();
	void updateLevelNumber();
	void initGiantsQuests();
	void initSwapForceQuests();
	void initUpgrades();

	QSpinBox* _spinExp;
	QSpinBox* _spinMoney;
	QSpinBox* _spinHeroPoints;
	QComboBox* _cmbLevelNumber;
	QComboBox* _cmbHat;
	QComboBox* _cmbTrinket;
	QTextEdit* _txtNickname;

	QLabel* _lblToyName;
	QLabel* _lblTimePlayed;
	QLabel* _lblFirstTouched;
	QLabel* _lblRecentlyTouched;
	QLabel* _lblWebcode;

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

	QFormLayout* _subUpgrades;
	QCheckBox*   _chkUG_B1;
	QCheckBox*   _chkUG_B2;
	QCheckBox*   _chkUG_B3;
	QCheckBox*   _chkUG_B4;
	QComboBox*   _cmbUG_Path;
	QCheckBox*   _chkUG_P1U1;
	QCheckBox*   _chkUG_P1U2;
	QCheckBox*   _chkUG_P1U3;
	QCheckBox*   _chkUG_P2U1;
	QCheckBox*   _chkUG_P2U2;
	QCheckBox*   _chkUG_P2U3;
	QCheckBox*   _chkUG_Soulgem;
	QCheckBox*   _chkUG_WowPow;

	QListWidget* _lstHeroics;
};