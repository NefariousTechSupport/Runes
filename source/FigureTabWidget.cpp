#include "FigureTabWidget.hpp"

#include <QFormLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QComboBox>
#include <QTextEdit>

#include "kTfbSpyroTag_HatType.hpp"
#include "Constants.hpp"
#include "toydata.hpp"

#define intToChecked(value) ((value) == 1 ? Qt::Checked : Qt::Unchecked)

FigureTabWidget::FigureTabWidget(Runes::PortalTag* tag, const char* fileName, QWidget* parent) : QWidget(parent)
{
	this->_tag = tag;
	this->_sourceFile = QString(fileName);

	QGridLayout* root = new QGridLayout(this);

	this->_lblToyName = new QLabel(this);
	this->_lblToyName->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
	root->addWidget(_lblToyName, 0, 0, 1, 3);

	uint32_t basicRow = root->rowCount();

	this->_spinMoney = new QSpinBox(this);
	this->_spinMoney->setRange(0, kMoneyCap);
	connect(this->_spinMoney, &QSpinBox::valueChanged, [=](int newMoney)
	{
		this->_tag->_coins = newMoney;
	});
	root->addWidget(new QLabel(tr("Money"), this), basicRow + 0, 0);
	root->addWidget(this->_spinMoney, basicRow + 0, 1);

	this->_spinExp = new QSpinBox(this);
	this->_spinExp->setRange(0, 197500);
	connect(this->_spinExp, &QSpinBox::valueChanged, [=](int newExp)
	{
		this->_tag->_exp = newExp;
		//TODO: update level number
	});
	root->addWidget(new QLabel(tr("Experience"), this), basicRow + 1, 0);
	root->addWidget(this->_spinExp, basicRow + 1, 1);

	this->_cmbHat = new QComboBox(this);
	for(int i = 0; i <= kTfbSpyroTag_Hat_MAX; i++)
	{
		this->_cmbHat->addItem(tr(hatNames_en[i]));
	}
	connect(this->_cmbHat, &QComboBox::currentIndexChanged, [=](int newIndex)
	{
		this->_tag->_hatType = (kTfbSpyroTag_HatType)newIndex;
	});
	root->addWidget(new QLabel(tr("Hat"), this), basicRow + 2, 0);
	root->addWidget(this->_cmbHat, basicRow + 2, 1);

	this->_spinHeroPoints = new QSpinBox(this);
	this->_spinHeroPoints->setRange(0, 100);
	connect(this->_spinHeroPoints, &QSpinBox::valueChanged, [=](int newHeroPoints)
	{
		this->_tag->_heroPoints = newHeroPoints;

		printf("%d, %d\n", this->size().width(), this->size().height());
	});
	root->addWidget(new QLabel(tr("Hero Points"), this), basicRow + 3, 0);
	root->addWidget(this->_spinHeroPoints, basicRow + 3, 1);

	this->_lblTimePlayed = new QLabel(tr("Time Played: N/A"), this);
	this->_lblLevel = new QLabel(tr("Level: N/A"), this);
	this->_lblFirstTouched = new QLabel(tr("First Touched: N/A"), this);
	this->_lblRecentlyTouched = new QLabel(tr("Last Touched: N/A"), this);
	root->addWidget(_lblTimePlayed, basicRow + 0, 2);
	root->addWidget(_lblLevel, basicRow + 1, 2);
	root->addWidget(_lblFirstTouched, basicRow + 2, 2);
	root->addWidget(_lblRecentlyTouched, basicRow + 3, 2);

	//Quests

	this->_wdGiantsElementalQuest1 = this->_sgInvalidElement1 = new QLabel(tr("Unknown Elemental 1"));
	this->_wdGiantsElementalQuest2 = this->_sgInvalidElement2 = new QLabel(tr("Unknown Elemental 2"));
	this->_wdSwapForceElementalQuest1 = this->_ssfInvalidElement1 = new QLabel(tr("Unknown Elemental 1"));
	this->_wdSwapForceElementalQuest2 = this->_ssfInvalidElement2 = new QLabel(tr("Unknown Elemental 2"));

	//TODO: Center the quest headers
	uint32_t questStart = root->rowCount();
	initGiantsQuests();
	root->addWidget(new QLabel(tr("<h3>Giants Quests</h3>")), questStart, 0, Qt::AlignLeft | Qt::AlignBottom);
	root->addLayout(_subGiantsQuests, questStart + 1, 0);

	initSwapForceQuests();
	root->addWidget(new QLabel(tr("<h3>Swap Force Quests</h3>")), questStart, 2, Qt::AlignLeft | Qt::AlignBottom);
	root->addLayout(_subSwapForceQuests, questStart + 1, 2);

	setLayout(root);
	
	setWindowTitle(tr("Runes"));

	updateFields();
}
#define defineElementalSpinQuest(form, generic, questGame, field, index, max, name) \
	QSpinBox* field = new QSpinBox(); \
	this->generic = field; \
	field->setRange(0, max); \
	field->setValue(this->_tag->questGame[index]); \
	connect(field, &QSpinBox::valueChanged, [=](int newValue) \
	{ \
		this->_tag->questGame[index] = newValue; \
	}); \
	this->form->insertRow(index, tr(name), this->generic);
#define defineElementalCheckQuest(form, generic, questGame, field, index, name) \
	QCheckBox* field = new QCheckBox(); \
	this->generic = field; \
	field->setChecked(intToChecked(this->_tag->questGame[index])); \
	connect(field, &QCheckBox::stateChanged, [=](int newState) \
	{ \
		this->_tag->questGame[index] = newState == Qt::Checked ? 1 : 0; \
	}); \
	this->form->insertRow(index, tr(name), this->generic);
void FigureTabWidget::updateFields()
{
	Runes::FigureToyData* figure = Runes::ToyDataManager::getInstance()->LookupCharacter(this->_tag->_toyType);
	if(figure != nullptr)
	{
		Runes::VariantIdentifier* variant = figure->LookupVariant(this->_tag->_subType);
		std::string toyName = std::string();
		if(variant == nullptr)
		{
			toyName = figure->_toyName.get();
		}
		else if(variant->_toyName.get() == "null")
		{
			toyName = variant->_variantText.get() + " " + figure->_toyName.get();
		}
		else
		{
			toyName = variant->_variantText.get() + " " + variant->_toyName.get();
		}
		this->_lblToyName->setText(tr(("<h2>" + toyName + "</h2>").c_str()));   //This is probably bad

		this->_lblTimePlayed->setText(QString("Time Played: %1h %2m %3s").arg(
			QString::number(this->_tag->_cumulativeTime / 3600),
			QString::number((this->_tag->_cumulativeTime % 3600) / 60),
			QString::number(this->_tag->_cumulativeTime % 60)
		));

		this->_lblLevel->setText(QString("Level: %1").arg(
			QString::number(this->_tag->ComputeLevel()).rightJustified(2, '0')
		));

		this->_lblFirstTouched->setText(QString("First Touched: %1-%2-%3 %4:%5").arg(
			QString::number(this->_tag->_firstUsed._year).rightJustified(4, '0'),
			QString::number(this->_tag->_firstUsed._month).rightJustified(2, '0'),
			QString::number(this->_tag->_firstUsed._day).rightJustified(2, '0'),
			QString::number(this->_tag->_firstUsed._hour).rightJustified(2, '0'),
			QString::number(this->_tag->_firstUsed._minute).rightJustified(2, '0')
		));

		this->_lblRecentlyTouched->setText(QString("Last Touched: %1-%2-%3 %4:%5").arg(
			QString::number(this->_tag->_recentlyUsed._year).rightJustified(4, '0'),
			QString::number(this->_tag->_recentlyUsed._month).rightJustified(2, '0'),
			QString::number(this->_tag->_recentlyUsed._day).rightJustified(2, '0'),
			QString::number(this->_tag->_recentlyUsed._hour).rightJustified(2, '0'),
			QString::number(this->_tag->_recentlyUsed._minute).rightJustified(2, '0')
		));

		this->_subGiantsQuests->removeRow(this->_wdGiantsElementalQuest1);
		this->_subGiantsQuests->removeRow(this->_wdGiantsElementalQuest2);
		this->_subSwapForceQuests->removeRow(this->_wdSwapForceElementalQuest1);
		this->_subSwapForceQuests->removeRow(this->_wdSwapForceElementalQuest2);
		switch(figure->_element)
		{
			case eET_Earth:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqEarthStonesmith, 6, 25, "Stonesmith");
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqEarthWrecker, 7, 25, "Wrecker");
					defineElementalCheckQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqEarthSaviorOfTheLand, 6, "Savior of the Land");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqEarthUnearther, 7, 25, "Unearther");
				}
				break;
			case eET_Water:
				{
					defineElementalCheckQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqWaterExtinguisher, 6, "Extinguisher");
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqWaterWaterfall, 7, 25, "Waterfall");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqWaterExtinguisher, 6, 25, "Extinguisher");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqWaterAFishIonado, 7, 25, "A-Fish-Ionado");
				}
				break;
			case eET_Air:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqAirSkylooter, 6, 500, "Skylooter");
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqAirFromAbove, 7, 25, "From Above");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqAirGeronimo, 6, 250, "Geronimo!");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqAirSkylooter, 7, 500, "Skylooter");
				}
				break;
			case eET_Fire:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqFireBombardier, 6, 25, "Bombardier");
					defineElementalCheckQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqFireSteamer, 7, "Steamer");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqFireMegaMelter, 6, 25, "Mega Melter");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqFireBombardier, 7, 30, "Bombardier");
				}
				break;
			case eET_Life:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqLifeFullyStocked, 6, 250, "Fully Stocked");
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqLifeMelonMaestro, 7, 200, "Melon Maestro");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqLifeDefenderOfLife, 6, 25, "Defender of Life");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqLifeFullyStocked, 7, 250, "Fully Stocked");
				}
				break;
			case eET_Death:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqUndeadBossedAround, 6, 3, "Bossed Around");
					defineElementalCheckQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqUndeadByAThread, 7, "By a Thread");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqUndeadWitherer, 6, 25, "Witherer");
					defineElementalCheckQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqUndeadBackFromTheBrink, 7, "Back from the Brink");
				}
				break;
			case eET_Magic:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqMagicPuzzlePower, 6, 25, "Puzzle Power");
					defineElementalCheckQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqMagicWarpWhomper, 7, "Warp Whomper");
					defineElementalCheckQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqMagicPuzzlePower, 6, "Puzzle Power");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqMagicMageRivalry, 7, 25, "Mage Rivalry");
				}
				break;
			case eET_Tech:
				{
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest1, _giantsQuests, gqTechMagicIsntMight, 6, 50, "Magic Isn't Might");
					defineElementalSpinQuest(_subGiantsQuests, _wdGiantsElementalQuest2, _giantsQuests, gqTechCracker, 7, 25, "Cracker");
					defineElementalCheckQuest(_subSwapForceQuests, _wdSwapForceElementalQuest1, _swapforceQuests, ssfqTechOutTeched, 6, "Out-Teched");
					defineElementalSpinQuest(_subSwapForceQuests, _wdSwapForceElementalQuest2, _swapforceQuests, ssfqTechProblemSolver, 7, 25, "Problem Solver");
				}
				break;
			default:
				this->_wdGiantsElementalQuest1 = new QLabel(tr("N/A"));
				this->_wdGiantsElementalQuest2 = new QLabel(tr("N/A"));
				this->_subGiantsQuests->insertRow(6, tr("Elemental 1"), this->_sgInvalidElement1);
				this->_subGiantsQuests->insertRow(7, tr("Elemental 2"), this->_sgInvalidElement2);
				break;
		}
		this->_wdGiantsElementalQuest1->setVisible(true);
		this->_wdGiantsElementalQuest2->setVisible(true);
	}
	this->_spinExp->setValue(this->_tag->_exp);
	this->_spinMoney->setValue(this->_tag->_coins);
	this->_spinHeroPoints->setValue(this->_tag->_heroPoints);
	this->_cmbHat->setCurrentIndex(this->_tag->_hatType);

	this->_spinGiantsMonsterMasher->setValue(this->_tag->_giantsQuests[0]);
	this->_spinGiantsBattleChamp->setValue(this->_tag->_giantsQuests[1]);
	this->_spinGiantsChowHound->setValue(this->_tag->_giantsQuests[2]);
	this->_chkGiantsHeroicChallenger->setCheckState(intToChecked(this->_tag->_giantsQuests[3]));
	this->_chkGiantsArenaArtist->setCheckState(intToChecked(this->_tag->_giantsQuests[4]));
	this->_spinGiantsElementalist->setValue(this->_tag->_giantsQuests[5]);
	this->_spinGiantsIndividualQuest->setValue(this->_tag->_giantsQuests[8]);

	this->_spinSwapForceBadguyBasher->setValue(this->_tag->_swapforceQuests[0]);
	this->_spinSwapForceFruitFrontiersman->setValue(this->_tag->_swapforceQuests[1]);
	this->_chkSwapForceFlawlessChallenger->setCheckState(intToChecked(this->_tag->_giantsQuests[2]));
	this->_spinSwapForceTrueGladiator->setValue(this->_tag->_swapforceQuests[3]);
	this->_chkSwapForceTotallyMaxedOut->setCheckState(intToChecked(this->_tag->_giantsQuests[4]));
	this->_spinSwapForceElementalist->setValue(this->_tag->_swapforceQuests[5]);
	this->_spinSwapForceIndividual->setValue(this->_tag->_swapforceQuests[8]);
}

#define defineSpinQuest(questGame, field, index, max) \
	this->field = new QSpinBox(); \
	this->field->setRange(0, max); \
	connect(this->field, &QSpinBox::valueChanged, [=](int newValue) \
	{ \
		this->_tag->questGame[index] = newValue; \
	});
#define defineCheckQuest(questGame, field, index) \
	this->field = new QCheckBox(); \
	connect(this->field, &QCheckBox::stateChanged, [=](int newState) \
	{ \
		this->_tag->questGame[index] = newState == Qt::Checked ? 1 : 0; \
	});
void FigureTabWidget::initGiantsQuests()
{
	_subGiantsQuests = new QFormLayout();
	defineSpinQuest(_giantsQuests, _spinGiantsMonsterMasher, 0, 1000);
	defineSpinQuest(_giantsQuests, _spinGiantsBattleChamp, 1, 10);
	defineSpinQuest(_giantsQuests, _spinGiantsChowHound, 2, 50);
	defineCheckQuest(_giantsQuests, _chkGiantsHeroicChallenger, 3);
	defineCheckQuest(_giantsQuests, _chkGiantsArenaArtist, 4);
	defineSpinQuest(_giantsQuests, _spinGiantsElementalist, 5, 7500);
	defineSpinQuest(_giantsQuests, _spinGiantsIndividualQuest, 8, 0xFFFF);

	_subGiantsQuests->addRow(tr("Monster Masher"), this->_spinGiantsMonsterMasher);
	_subGiantsQuests->addRow(tr("Battle Champ"), this->_spinGiantsBattleChamp);
	_subGiantsQuests->addRow(tr("Chow Hound"), this->_spinGiantsChowHound);
	_subGiantsQuests->addRow(tr("Heroic Challenger"), this->_chkGiantsHeroicChallenger);
	_subGiantsQuests->addRow(tr("Arena Artist"), this->_chkGiantsArenaArtist);
	_subGiantsQuests->addRow(tr("Elementalist"), this->_spinGiantsElementalist);
	_subGiantsQuests->addRow(tr("Elemental Quest 1"), this->_sgInvalidElement1);
	_subGiantsQuests->addRow(tr("Elemental Quest 2"), this->_sgInvalidElement2);
	_subGiantsQuests->addRow(tr("Individual Quest"), this->_spinGiantsIndividualQuest);
}
void FigureTabWidget::initSwapForceQuests()
{
	_subSwapForceQuests = new QFormLayout();
	defineSpinQuest(_swapforceQuests, _spinSwapForceBadguyBasher, 0, 1000);
	defineSpinQuest(_swapforceQuests, _spinSwapForceFruitFrontiersman, 1, 15);
	defineCheckQuest(_swapforceQuests, _chkSwapForceFlawlessChallenger, 2);
	defineSpinQuest(_swapforceQuests, _spinSwapForceTrueGladiator, 3, 10);
	defineCheckQuest(_swapforceQuests, _chkSwapForceTotallyMaxedOut, 4);
	defineSpinQuest(_swapforceQuests, _spinSwapForceElementalist, 5, 7500);
	defineSpinQuest(_swapforceQuests, _spinSwapForceIndividual, 8, 0xFFFF);

	_subSwapForceQuests->addRow(tr("Badguy Basher"), this->_spinSwapForceBadguyBasher);
	_subSwapForceQuests->addRow(tr("Fruit Frontiersman"), this->_spinSwapForceFruitFrontiersman);
	_subSwapForceQuests->addRow(tr("Flawless Challenger"), this->_chkSwapForceFlawlessChallenger);
	_subSwapForceQuests->addRow(tr("True Gladiator"), this->_spinSwapForceTrueGladiator);
	_subSwapForceQuests->addRow(tr("Totally Maxed Out"), this->_chkSwapForceTotallyMaxedOut);
	_subSwapForceQuests->addRow(tr("Elementalist"), this->_spinSwapForceElementalist);
	_subSwapForceQuests->addRow(tr("Elemental Quest 1"), this->_ssfInvalidElement1);
	_subSwapForceQuests->addRow(tr("Elemental Quest 2"), this->_ssfInvalidElement2);
	_subSwapForceQuests->addRow(tr("Individual Quest"), this->_spinSwapForceIndividual);
}