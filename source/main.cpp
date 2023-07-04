#include <iostream>
#include <fstream>
#include <assert.h>

#include "PortalTag.hpp"
#include "3rd_party/crc.h"
#include "PortalAlgos.hpp"
#include "toydata.hpp"
#include "debug.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
 
int main(int argc, char **argv)
{
	RedirectIOToConsole();
	Runes::PortalTag* tag = new Runes::PortalTag();
	tag->_rfidTag = new Runes::RfidTag();
	tag->_rfidTag->ReadFromFile(argv[1]);

	tag->StoreHeader();
	tag->StoreMagicMoment();
	tag->StoreRemainingData();
	tag->DebugPrintHeader();

	Runes::ToyDataManager* toyMan = Runes::ToyDataManager::getInstance();

	Fl_Window *window = new Fl_Window(720,480, "Runes");

	const char* toyName = toyMan->LookupCharacter(tag->_toyType)->_toyName;
	Fl_Box* skylanderNameBox = new Fl_Box(12, 12, 100, 20, toyName);
	skylanderNameBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

	char txtCoin[6];
	sprintf(txtCoin, "%d", tag->_coins);
	Fl_Input* iptCoins = new Fl_Input(skylanderNameBox->x() + 100, skylanderNameBox->y() + 32, 100, 20, "Money");
	iptCoins->value(txtCoin);

	char txtExp[11];
	sprintf(txtExp, "%d", tag->_exp);
	Fl_Input* iptExp = new Fl_Input(iptCoins->x(), iptCoins->y() + 32, 100, 20, "Exp");
	iptExp->value(txtExp);

	char txtHat[6];
	sprintf(txtHat, "%d", tag->_hatType);
	Fl_Input* iptHat = new Fl_Input(iptExp->x(), iptExp->y() + 32, 100, 20, "Hat");
	iptHat->value(txtHat);

	window->end();
	window->show(0, NULL);
	return Fl::run();
}
/*void loadToydata(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	assert(sizeof(Runes::PortalTagHeader) == 0x20);

	Runes::PortalTag* tag = new Runes::PortalTag();
	tag->_rfidTag = new Runes::RfidTag();
	tag->_rfidTag->ReadFromFile(argv[1]);

	tag->StoreHeader();
	tag->StoreMagicMoment();
	tag->DebugPrintHeader();

	printf("coins: %d\n", tag->_tagData._coinCount);
}

void loadToydata(int argc, char* argv[])
{
	Runes::ToyDataManager* toyMan = Runes::ToyDataManager::getInstance();
	printf("looking for vvind up\n");
	Runes::FigureToyData* toyData = toyMan->LookupCharacter(kTfbSpyroTag_ToyType_Character_GillGrunt);
	Runes::VariantIdentifier* varData = toyData->LookupVariant(0x2405);
	printf("display text for variant: %s, %s\n", (varData ? varData->_variantText : "N/A"), (varData ? varData->_toyName : "N/A"));
}

void decryptAndDump(int argc, char* argv[])
{
	Runes::PortalTag* tag = new Runes::PortalTag();
	FILE* f = fopen(argv[1], "rb");
	FILE* of = fopen("test.dat", "wb");
	fseek(f, 0, SEEK_SET);
	fread(&tag->_tagHeader, 0x01, sizeof(Runes::PortalTagHeader), f);

	uint8_t cryptBlock[0x10];
	uint8_t decryBlock[0x10];
	for(int i = 0; i < 0x40; i++)
	{
		fseek(f, i  * 0x10, SEEK_SET);
		fread(cryptBlock, 0x1, 0x10, f);
		if(i >= 8 && (i % 4) != 3)
		{
			Runes::decryptBlock(&tag->_tagHeader, cryptBlock, decryBlock, i);
		}
		else
		{
			memcpy(decryBlock, cryptBlock, 0x10);
		}
		fwrite(decryBlock, 1, 0x10, of);
	}
	fclose(f);
}*/