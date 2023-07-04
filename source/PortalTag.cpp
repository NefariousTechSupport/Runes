#include "PortalTag.hpp"
#include "toydata.hpp"

#include <iostream>

#define to24(low, high) ((uint32_t)(low) + ((uint32_t)(high) << 16))

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
	printf("_toyName: %s\n", toyData->_toyName);
	printf("_subType:\n\t_yearCode: %d\n\t_fullAltDeco: %hhu\n\t_wowPowFlag: %hhu\n\t_lightcore: %hhu\n\t_decoId: %02X\n\t_variantText: %s\n\t_toyName: %s\n", esg, fullAltDeco, wowPowFlag, lightcore, decoId, (varData ? varData->_variantText : "N/A"), (varData ? varData->_toyName : "N/A"));
	printf("_tradingCardId: %08X%08X\n", this->_tagHeader._tradingCardId2, this->_tagHeader._tradingCardId1);
	printf("_webCode: %s\n", this->_webCode);
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
		this->_rfidTag->CopyBlocks(&this->_tagData, 8, 0xB);
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

	this->_upgrades = ((this->_tagData._flags2 & 0xF) << 10) | (this->_tagData._flags1 & 0x3FF);
	this->_elementCollectionCounts[0] = (this->_tagData._flags1 >> 10) & 3;
	this->_elementCollectionCounts[1] = (this->_tagData._flags1 >> 14) & 7;
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
	this->_quests = this->_tagData._quests;
	this->_ownerCount = this->_tagData._ownerCount;
	this->_heroPoints = this->_tagData._heroPoints;
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