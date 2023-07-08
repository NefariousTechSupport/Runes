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
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>

#define createIntValueReader(field, minimum, maximum) \
	Runes::PortalTag* tag = (Runes::PortalTag*)tagPtr; \
	Fl_Value_Input* ipt = (Fl_Value_Input*)iptPtr; \
	int32_t value = (int32_t)ipt->value(); \
	if(value < minimum) value = minimum; \
	if(value > maximum) value = maximum; \
	tag->field = (decltype(tag->field))value; \
	ipt->value(value); \
	printf("new value for %s: %d\n", #field, tag->_coins);

void onEditCoins(Fl_Widget* iptPtr, void* tagPtr)
{
	createIntValueReader(_coins, 0, 0xFFFF);
}
void onEditExp(Fl_Widget* iptPtr, void* tagPtr)
{
	createIntValueReader(_exp, 0, 197500);
}
void onEditHeroPoints(Fl_Widget* iptPtr, void* tagPtr)
{
	createIntValueReader(_heroPoints, 0, 100);
}
void decryptAndDump(int argc, char* argv[]);

int main(int argc, char **argv)
{
	decryptAndDump(argc, argv); return 0;

	RedirectIOToConsole();
	Runes::PortalTag* tag = new Runes::PortalTag();
	tag->_rfidTag = new Runes::RfidTag();
	tag->_rfidTag->ReadFromFile(argv[1]);

	tag->StoreHeader();
	tag->StoreMagicMoment();
	tag->StoreRemainingData();
	tag->DebugPrintHeader();
	tag->DebugSaveTagData();


	Runes::ToyDataManager* toyMan = Runes::ToyDataManager::getInstance();

	Fl_Window *window = new Fl_Window(720,480, "Runes");

	const char* toyName = toyMan->LookupCharacter(tag->_toyType)->_toyName;
	Fl_Box* skylanderNameBox = new Fl_Box(12, 12, 100, 20, toyName);
	skylanderNameBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

	Fl_Value_Input* iptCoins = new Fl_Value_Input(skylanderNameBox->x() + 100, skylanderNameBox->y() + 32, 100, 20, "Money");
	iptCoins->callback(onEditCoins, tag);
	iptCoins->value(tag->_coins);
	iptCoins->step(1);

	Fl_Value_Input* iptExp = new Fl_Value_Input(iptCoins->x(), iptCoins->y() + 32, 100, 20, "Exp");
	//iptExp->callback(onEditCoins, tag);
	iptExp->value(tag->_exp);
	iptExp->step(1);

	char txtHat[6];
	sprintf(txtHat, "%d", tag->_hatType);
	Fl_Input* iptHat = new Fl_Input(iptExp->x(), iptExp->y() + 32, 100, 20, "Hat");
	iptHat->value(txtHat);

	window->end();
	window->show(0, NULL);
	return Fl::run();
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
}