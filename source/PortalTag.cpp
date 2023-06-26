#include "PortalTag.hpp"
#include "toydata.hpp"

#include <iostream>

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
void Runes::PortalTag::StoreMagicMoment()
{
	this->_rfidTag->CopyBlocks(&this->_tagMagicMoment, 8, 1);
}