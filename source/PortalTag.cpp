#include "PortalTag.hpp"
#include "toydata.hpp"
#include "Constants.hpp"
#include "EElementType.hpp"

#include <iostream>
#include <cstring>
#include "3rd_party/crc.h"

#define to24(low, high) ((uint32_t)(low) + ((uint32_t)(high) << 16))
#define bitsToMask(bits) (0xFFFFFFFFFFFFFFFF >> (64 - (bits)))

void Runes::PortalTag::DecodeSubtype(uint16_t varId, ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId)
{
	*esg = (ESkylandersGame)((varId >> 12) & 0xF);
	*wowPowFlag = (varId >> 11) & 1;
	*fullAltDeco = (varId >> 10) & 1;
	*lightcore = (varId >> 9) & 1;
	*decoId = (kTfbSpyroTag_DecoID)(varId & 0xFF);
}
void Runes::PortalTag::DecodeSubtype(ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId)
{
	DecodeSubtype(this->_tagHeader._subType, esg, fullAltDeco, wowPowFlag, lightcore, decoId);
}
void Runes::PortalTag::DebugPrintHeader()
{
	ESkylandersGame esg;
	bool fullAltDeco, wowPowFlag, lightcore;
	kTfbSpyroTag_DecoID decoId;
	DecodeSubtype(&esg, &fullAltDeco, &wowPowFlag, &lightcore, &decoId);
	Runes::ToyDataManager* toyMan = Runes::ToyDataManager::getInstance();
	Runes::FigureToyData* toyData = toyMan->LookupCharacter(this->_tagHeader._toyType);
	Runes::VariantIdentifier* varData = toyData->LookupVariant(this->_tagHeader._subType);

	printf("_serial: %08X\n", this->_tagHeader._serial);
	printf("_toyType: %04X\n", this->_tagHeader._toyType);
	printf("_toyName: %s\n", toyData->_toyName.get().c_str());
	printf("_subType:\n\t_yearCode: %d\n\t_fullAltDeco: %hhu\n\t_wowPowFlag: %hhu\n\t_lightcore: %hhu\n\t_decoId: %02X\n\t_variantText: %s\n\t_toyName: %s\n", esg, fullAltDeco, wowPowFlag, lightcore, decoId, (varData ? varData->_variantText.get().c_str() : "N/A"), (varData ? varData->_toyName.get().c_str() : "N/A"));
	printf("_tradingCardId: %08X%08X\n", this->_tagHeader._tradingCardId2, this->_tagHeader._tradingCardId1);
	printf("_webCode: %s\n", this->_webCode);
}
void Runes::PortalTag::DebugSaveTagData()
{
	std::string fileName(Runes::ToyDataManager::getInstance()->LookupCharacter(this->_toyType)->_toyName.get());
	fileName += ".dat";
	FILE* f = fopen(fileName.c_str(), "wb");
	fwrite(&this->_tagData, 1, sizeof(Runes::PortalTagData), f);
	fflush(f);
	fclose(f);
}
void Runes::PortalTag::StoreHeader()
{
	this->_rfidTag->CopyBlocks(&this->_tagHeader, 0, 2);

	this->_serial = this->_tagHeader._serial;
	this->_toyType = this->_tagHeader._toyType;
	this->_subType = this->_tagHeader._subType;

	//Web code

	//basically 0->9, A->Z except not 0, 1, or any vowels
	const char* webCodeTable = "23456789BCDFGHJKLMNPQRSTVWXYZ";

	//the original trading card id is split up into 2 uints because of alignment
	uint64_t tradingCardId = ((uint64_t)this->_tagHeader._tradingCardId2 << 0x20) | this->_tagHeader._tradingCardId1;

	//420707233300200 is 29^10-1
	if(tradingCardId < 420707233300200 && tradingCardId != 0)
	{
		for(int i = 0; i < 10; i++)
		{
			uint8_t index = tradingCardId % 29;
			tradingCardId = tradingCardId / 29;
			this->_webCode[9 - i] = webCodeTable[index];
		}
		//Add the hyphen and a null byte
		for(int i = 10; i > 5; i--)
		{
			this->_webCode[i] = this->_webCode[i-1];
		}
		this->_webCode[5] = '-';
		this->_webCode[11] = '\0';
	}
}
void Runes::PortalTag::StoreTagData()
{
	if(!this->_tagDataStored)
	{
		this->_rfidTag->CopyBlocks(((uint8_t*)&this->_tagData) + 0x00, this->_rfidTag->DetermineActiveDataRegion0() ? 0x24 : 0x08, 0x7);
		this->_rfidTag->CopyBlocks(((uint8_t*)&this->_tagData) + 0x70, this->_rfidTag->DetermineActiveDataRegion1() ? 0x2D : 0x11, 0x4);
		this->_tagDataStored = true;
	}
}
void Runes::PortalTag::StoreMagicMoment()
{
	this->StoreTagData();

	this->_tagMagicMomentStored = true;
	this->_exp = to24(this->_tagData._experience2011_low, this->_tagData._experience2011_high) + (uint32_t)this->_tagData._experience2012 + this->_tagData._experience2013;
	this->_coins = this->_tagData._coinCount;
	this->_cumulativeTime = this->_tagData._cumulativeTime;
	this->_platformUse = (this->_tagData._platformUse2013 << 8) | this->_tagData._platformUse2011;

	     if(this->_tagData._hat2011 > 0) this->_hatType = (kTfbSpyroTag_HatType)this->_tagData._hat2011;
	else if(this->_tagData._hat2012 > 0) this->_hatType = (kTfbSpyroTag_HatType)this->_tagData._hat2012;
	else if(this->_tagData._hat2013 > 0) this->_hatType = (kTfbSpyroTag_HatType)this->_tagData._hat2013;
	else if(this->_tagData._hat2015 > 0) this->_hatType = (kTfbSpyroTag_HatType)(this->_tagData._hat2015 + kTfbSpyroTag_Hat_OFFSET_2015);

	uint32_t flags1 = to24(this->_tagData._flags1_low, this->_tagData._flags1_high);
	this->_upgrades = ((this->_tagData._flags2 & 0xF) << 10) | (flags1 & 0x3FF);
	this->_elementCollectionCounts[0] = (flags1 >> 10) & 3;
	this->_elementCollectionCounts[1] = (flags1 >> 14) & 7;
	this->_elementCollection = this->_elementCollectionCounts[0] + this->_elementCollectionCounts[1] + ((this->_tagData._flags2 >> 11) & 7);
	this->_accoladeRanks[0] = (this->_tagData._flags2 >> 9) & 3;
	this->_accoladeRanks[1] = (this->_tagData._flags2 >> 4) & 3;

	memcpy(&this->_nickname, &this->_tagData._nickname, 0x20);
}
void Runes::PortalTag::StoreRemainingData()
{
	this->_firstUsed = this->_tagData._firstUsed;
	this->_recentlyUsed = this->_tagData._recentlyUsed;
	this->_heroics = ((uint64_t)to24(this->_tagData._heroics2012_low, this->_tagData._heroics2012_high) << 32) | this->_tagData._heroics2011;
	this->_ownerCount = this->_tagData._ownerCount;
	this->_heroPoints = this->_tagData._heroPoints;
	StoreQuestsGiants();
	StoreQuestsSwapForce();
}
void Runes::PortalTag::StoreQuestsGiants()
{
	uint64_t questsLow = this->_tagData._sgQuestsLow;
	uint8_t questsHigh = this->_tagData._sgQuestsHigh;
	this->_giantsQuests[0] = questsLow & bitsToMask(kQuestGiantsMonsterMasherBits); 	questsLow >>= kQuestGiantsMonsterMasherBits;
	questsLow |= (uint64_t)questsHigh << 54;
	this->_giantsQuests[1] = questsLow & bitsToMask(kQuestGiantsBattleChampBits); 		questsLow >>= kQuestGiantsBattleChampBits;
	this->_giantsQuests[2] = questsLow & bitsToMask(kQuestGiantsChowHoundBits); 		questsLow >>= kQuestGiantsChowHoundBits;
	this->_giantsQuests[3] = questsLow & bitsToMask(kQuestGiantsHeroicChallengerBits); 	questsLow >>= kQuestGiantsHeroicChallengerBits;
	this->_giantsQuests[4] = questsLow & bitsToMask(kQuestGiantsArenaArtistBits); 		questsLow >>= kQuestGiantsArenaArtistBits;
	this->_giantsQuests[5] = questsLow & bitsToMask(kQuestGiantsElementalistBits); 		questsLow >>= kQuestGiantsElementalistBits;

	uint8_t elementQuests[2];
	getQuestsGiantsElementalBits(elementQuests);

	this->_giantsQuests[6] = questsLow & bitsToMask(elementQuests[0]); 	questsLow >>= elementQuests[0];
	this->_giantsQuests[7] = questsLow & bitsToMask(elementQuests[1]); 	questsLow >>= elementQuests[1];
	this->_giantsQuests[8] = (questsLow & bitsToMask(kQuestSwapForceIndividualBits)) | ((uint16_t)(questsHigh & 2) << 14);
}
void Runes::PortalTag::FillQuestsGiants()
{
	uint8_t accum = 0;
	this->_tagData._sgQuestsLow = 0;
	this->_tagData._sgQuestsHigh = 0;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[0] & bitsToMask(kQuestGiantsMonsterMasherBits)) << accum;    accum += kQuestGiantsMonsterMasherBits;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[1] & bitsToMask(kQuestGiantsBattleChampBits)) << accum;      accum += kQuestGiantsBattleChampBits;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[2] & bitsToMask(kQuestGiantsChowHoundBits)) << accum;        accum += kQuestGiantsChowHoundBits;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[3] & bitsToMask(kQuestGiantsHeroicChallengerBits)) << accum; accum += kQuestGiantsHeroicChallengerBits;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[4] & bitsToMask(kQuestGiantsArenaArtistBits)) << accum;      accum += kQuestGiantsArenaArtistBits;
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[5] & bitsToMask(kQuestGiantsElementalistBits)) << accum;     accum += kQuestGiantsElementalistBits;

	uint8_t elementQuests[2];
	getQuestsGiantsElementalBits(elementQuests);

	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[6] & bitsToMask(elementQuests[0])) << accum; accum += elementQuests[0];
	this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[7] & bitsToMask(elementQuests[1])) << accum; accum += elementQuests[1];

	if(accum > 48)	//if the individual quests will overflow into _sgQuestsHigh
	{
		this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[8] & bitsToMask(64 - accum)) << accum;
		this->_tagData._sgQuestsHigh |= this->_giantsQuests[8] >> (64 - accum);
	}
	else
	{
		this->_tagData._sgQuestsLow |= ((uint64_t)this->_giantsQuests[8] & bitsToMask(16)) << accum;
	}
}
void Runes::PortalTag::getQuestsGiantsElementalBits(uint8_t* bits)
{
	FigureToyData* figure = ToyDataManager::getInstance()->LookupCharacter(this->_toyType);
	switch(figure->_element)
	{
		case eET_Earth:
			bits[0] = kQuestGiantsEarthWreckerBits;
			bits[1] = kQuestGiantsEarthStonesmithBits;
			break;
		case eET_Water:
			bits[0] = kQuestGiantsWaterExtinguisherBits;
			bits[1] = kQuestGiantsWaterWaterfallBits;
			break;
		case eET_Air:
			bits[0] = kQuestGiantsAirFromAboveBits;
			bits[1] = kQuestGiantsAirSkyLooterBits;
			break;
		case eET_Fire:
			bits[0] = kQuestGiantsFireBombardierBits;
			bits[1] = kQuestGiantsFireSteamerBits;
			break;
		case eET_Life:
			bits[0] = kQuestGiantsLifeFullyStockedBits;
			bits[1] = kQuestGiantsLifeMelonMaestroBits;
			break;
		case eET_Death:
			bits[0] = kQuestGiantsUndeadBossedAroundBits;
			bits[1] = kQuestGiantsUndeadByAThreadBits;
			break;
		case eET_Magic:
			bits[0] = kQuestGiantsMagicPuzzlePowerBits;
			bits[1] = kQuestGiantsMagicWarpWomperBits;
			break;
		case eET_Tech:
			bits[0] = kQuestGiantsTechMagicIsntMightBits;
			bits[1] = kQuestGiantsTechCrackerBits;
			break;
		default:
			return;
	}
}
void Runes::PortalTag::StoreQuestsSwapForce()
{
	uint64_t questsLow = this->_tagData._ssfQuestsLow;
	uint8_t questsHigh = this->_tagData._ssfQuestsHigh;
	this->_swapforceQuests[0] = questsLow & bitsToMask(kQuestSwapForceBadguyBasherBits); 		questsLow >>= kQuestSwapForceBadguyBasherBits;
	this->_swapforceQuests[1] = questsLow & bitsToMask(kQuestSwapForceFruitFrontiersmanBits); 	questsLow >>= kQuestSwapForceFruitFrontiersmanBits;
	this->_swapforceQuests[2] = questsLow & bitsToMask(kQuestSwapForceFlawlessChallengerBits); 	questsLow >>= kQuestSwapForceFlawlessChallengerBits;
	this->_swapforceQuests[3] = questsLow & bitsToMask(kQuestSwapForceTrueGladiatorBits); 		questsLow >>= kQuestSwapForceTrueGladiatorBits;
	this->_swapforceQuests[4] = questsLow & bitsToMask(kQuestSwapForceTotallyMaxedOutBits); 	questsLow >>= kQuestSwapForceTotallyMaxedOutBits;
	this->_swapforceQuests[5] = questsLow & bitsToMask(kQuestSwapForceElementalistBits); 		questsLow >>= kQuestSwapForceElementalistBits;
	this->_swapforceQuests[6] = questsLow & bitsToMask(kQuestSwapForceElemental1Bits); 			questsLow >>= kQuestSwapForceElemental1Bits;
	this->_swapforceQuests[7] = questsLow & bitsToMask(kQuestSwapForceElemental2Bits); 			questsLow >>= kQuestSwapForceElemental2Bits;
	this->_swapforceQuests[8] = (questsLow & bitsToMask(kQuestSwapForceIndividualBits)) | ((uint16_t)(questsHigh & 3) << 14);
}
void Runes::PortalTag::FillQuestsSwapForce()
{
	this->_tagData._ssfQuestsHigh = (this->_swapforceQuests[8] >> 14) & bitsToMask(2);
	this->_tagData._ssfQuestsLow = this->_swapforceQuests[8] & bitsToMask(14);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceElemental2Bits;         this->_tagData._ssfQuestsLow |= this->_swapforceQuests[7] & bitsToMask(kQuestSwapForceElemental2Bits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceElemental1Bits;         this->_tagData._ssfQuestsLow |= this->_swapforceQuests[6] & bitsToMask(kQuestSwapForceElemental1Bits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceElementalistBits;       this->_tagData._ssfQuestsLow |= this->_swapforceQuests[5] & bitsToMask(kQuestSwapForceElementalistBits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceTotallyMaxedOutBits;    this->_tagData._ssfQuestsLow |= this->_swapforceQuests[4] & bitsToMask(kQuestSwapForceTotallyMaxedOutBits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceTrueGladiatorBits;      this->_tagData._ssfQuestsLow |= this->_swapforceQuests[3] & bitsToMask(kQuestSwapForceTrueGladiatorBits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceFlawlessChallengerBits; this->_tagData._ssfQuestsLow |= this->_swapforceQuests[2] & bitsToMask(kQuestSwapForceFlawlessChallengerBits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceFruitFrontiersmanBits;  this->_tagData._ssfQuestsLow |= this->_swapforceQuests[1] & bitsToMask(kQuestSwapForceFruitFrontiersmanBits);
	this->_tagData._ssfQuestsLow <<= kQuestSwapForceBadguyBasherBits;       this->_tagData._ssfQuestsLow |= this->_swapforceQuests[0] & bitsToMask(kQuestSwapForceBadguyBasherBits);
}
void Runes::PortalTag::FillOutputFromStoredData()
{
	Runes::PortalTagData* tagData = &this->_tagData;

	//Set exp
	uint32_t currentExp = this->_exp;
	tagData->_experience2011_low = (currentExp > 33000 ? 33000 : currentExp);
	tagData->_experience2011_high = 0;
	if(currentExp > 33000) currentExp -= 33000;
	else currentExp = 0;

	if((this->_subType >> 0xC) > 1)
	{
		tagData->_experience2012 = (currentExp > 63500 ? 63500 : currentExp);
		if(currentExp > 63500) currentExp -= 63500;
		else currentExp = 0;
	}
	else
	{
		tagData->_experience2012 = (currentExp > 0xFFFF ? 0xFFFF : currentExp);
		if(currentExp > 0xFFFF) currentExp -= 0xFFFF;
		else currentExp = 0;
	}

	tagData->_experience2013 = (currentExp > 101000 ? 101000 : currentExp);
	if(currentExp > 101000) currentExp -= 101000;
	else currentExp = 0;

	//Set money
	tagData->_coinCount = this->_coins;

	//Set cumulative time
	tagData->_cumulativeTime = this->_cumulativeTime;
	//Set hero points
	tagData->_heroPoints = this->_heroPoints;
	//Set owner count
	tagData->_ownerCount = this->_ownerCount;

	this->FillQuestsGiants();
	this->FillQuestsSwapForce();
}
void Runes::PortalTag::ReadFromFile(const char* fileName)
{
	this->_tagHeaderStored = false;
	this->_tagMagicMomentStored = false;
	this->_tagRemainingDataStored = false;
	this->_tagDataStored = false;

	this->_rfidTag->ReadFromFile(fileName);

	this->StoreHeader();
	this->StoreMagicMoment();
	this->StoreRemainingData();
}
void Runes::PortalTag::SaveToFile(const char* fileName)
{
	//this->_tagData._areaSequence0++;
	//this->_tagData._areaSequence1++;
	this->FillOutputFromStoredData();
	this->RecalculateTagDataChecksums();
	_rfidTag->SaveBlocks(((uint8_t*)&this->_tagData) + 0x00, this->_rfidTag->DetermineActiveDataRegion0() ? 0x24 : 0x08, 0x7);
	_rfidTag->SaveBlocks(((uint8_t*)&this->_tagData) + 0x70, this->_rfidTag->DetermineActiveDataRegion1() ? 0x2D : 0x11, 0x4);
	//this->DebugSaveTagData();
	_rfidTag->SaveToFile(fileName);
}
void Runes::PortalTag::RecalculateTagDataChecksums()
{
	char checksumBuffer[0x110];
	memset(checksumBuffer, 0x00, 0x110);

	//Type 6
	memcpy(checksumBuffer, ((uint8_t*)&this->_tagData) + 7 * BLOCK_SIZE, 4 * BLOCK_SIZE);
	checksumBuffer[0] = 6;
	checksumBuffer[1] = 1;
	this->_tagData._crcType6 = crc16(checksumBuffer, 0x04 * BLOCK_SIZE);

	//Type 3
	memcpy(checksumBuffer, ((uint8_t*)&this->_tagData) + 4 * BLOCK_SIZE, 3 * BLOCK_SIZE);
	this->_tagData._crcType3 = crc16(checksumBuffer, 0x11 * BLOCK_SIZE);

	//Type 2
	memcpy(checksumBuffer, ((uint8_t*)&this->_tagData) + 1 * BLOCK_SIZE, 3 * BLOCK_SIZE);
	this->_tagData._crcType2 = crc16(checksumBuffer, 0x03 * BLOCK_SIZE);

	//Type 1
	this->_tagData._crcType1 = 5;
	memcpy(checksumBuffer, ((uint8_t*)&this->_tagData), BLOCK_SIZE);
	this->_tagData._crcType1 = crc16(checksumBuffer, 0x01 * BLOCK_SIZE);
}
uint32_t Runes::PortalTagData::getExperience()
{
	printf("%d + %d + %d = %d\n", to24(this->_experience2011_low, this->_experience2011_high), this->_experience2012, this->_experience2013, to24(this->_experience2011_low, this->_experience2011_high) + (uint32_t)this->_experience2012 + this->_experience2013);
	return to24(this->_experience2011_low, this->_experience2011_high) + (uint32_t)this->_experience2012 + this->_experience2013;
}
void Runes::PortalTagData::setExperience(uint32_t experience)
{
	uint32_t currentExp = experience;
	this->_experience2011_low = 0;
	this->_experience2011_high = 0;
	this->_experience2012 = 0;
	this->_experience2013 = 0;

	//2011 (level 1->10)
	if(currentExp >= 33000)
	{
		this->_experience2011_low = 33000;
		currentExp -= 33000;
	}
	else
	{
		this->_experience2011_low = (uint16_t)currentExp;
		return;
	}
	//2012 (level 10->15)
	if(currentExp >= 63500)
	{
		this->_experience2012 = 63500;
		currentExp -= 63500;
	}
	else
	{
		this->_experience2012 = currentExp;
		return;
	}
	//2013 (level 15->20)
	if(currentExp >= 101000)
	{
		this->_experience2013 = 101000;
	}
	else
	{
		this->_experience2013 = currentExp;
		return;
	}
}

uint16_t Runes::PortalTagData::getMoney()
{
	return this->_coinCount;
}
void Runes::PortalTagData::setMoney(uint16_t money)
{
	this->_coinCount = money;
}

kTfbSpyroTag_HatType Runes::PortalTagData::getHat()
{
	if(this->_hat2011 > 0) return (kTfbSpyroTag_HatType)this->_hat2011;
	if(this->_hat2012 > 0) return (kTfbSpyroTag_HatType)this->_hat2012;
	if(this->_hat2013 > 0) return (kTfbSpyroTag_HatType)this->_hat2013;
	if(this->_hat2015 > 0) return (kTfbSpyroTag_HatType)(this->_hat2015 + kTfbSpyroTag_Hat_OFFSET_2015);
	return kTfbSpyroTag_Hat_NONE;
}
void Runes::PortalTagData::setHat(kTfbSpyroTag_HatType hat)
{
	this->_hat2011 = 0;
	this->_hat2012 = 0;
	this->_hat2013 = 0;
	this->_hat2015 = 0;
	if(hat <= kTfbSpyroTag_Hat_MAX_2011)
	{
		this->_hat2011 = hat;
		return;
	}
	else if(hat <= kTfbSpyroTag_Hat_MAX_2012)
	{
		this->_hat2012 = hat;
		return;
	}
	else if(hat <= kTfbSpyroTag_Hat_MAX_2014)
	{
		this->_hat2013 = hat;
		return;
	}
	else if(hat >= kTfbSpyroTag_Hat_MIN_2015 && hat <= kTfbSpyroTag_Hat_MAX_2015)
	{
		this->_hat2015 = hat - kTfbSpyroTag_Hat_OFFSET_2015;
		return;
	}
	printf("Invalid Hat ID");
}
bool Runes::PortalTag::isTrap()
{
	return this->_toyType >= kTfbSpyroTag_ToyType_TRAP_2014 && this->_toyType <= kTfbSpyroTag_ToyType_TRAP_2014_MAX;
}