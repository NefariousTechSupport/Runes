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
#include <FL/Fl_Menu_Item.H>
#include <FL/Fl_Menu_Bar.H>

#define createIntValueReader(method, field, minimum, maximum) \
void method(Fl_Widget* iptPtr, void* tagPtr) \
{ \
	Runes::PortalTag* tag = (Runes::PortalTag*)tagPtr; \
	Fl_Value_Input* ipt = (Fl_Value_Input*)iptPtr; \
	int32_t value = (int32_t)ipt->value(); \
	if(value < minimum) value = minimum; \
	if(value > maximum) value = maximum; \
	tag->field = (decltype(tag->field))value; \
	ipt->value(value); \
	printf("new value for %s: %d\n", #field, tag->field); \
}

#define createIntValueField(variableName, name, previous, field, cb) \
	Fl_Value_Input* variableName = new Fl_Value_Input(previous->x(), previous->y() + 32, 100, 20, name); \
	variableName->callback(cb, tag); \
	variableName->value(tag->field); \
	variableName->step(1);

Runes::PortalTag* tag = NULL;

createIntValueReader(onEditCoins, _coins, 0, 0xFFFF);
createIntValueReader(onEditExp, _exp, 0, 197500);
createIntValueReader(onEditHeroPoints, _heroPoints, 0, 100);
createIntValueReader(onEditHat, _hatType, kTfbSpyroTag_Hat_MIN, kTfbSpyroTag_Hat_MAX);
createIntValueReader(onEditOwnerCount, _ownerCount, 0, 255);

void onSave(Fl_Widget* savePtr, void* tagPtr)
{
	tag->FillOutputFromStoredData();
	tag->SaveToFile("test.dat");
}

void decryptAndDump(int argc, char* argv[]);

int main(int argc, char **argv)
{
	//decryptAndDump(argc, argv); return 0;

	RedirectIOToConsole();
	tag = new Runes::PortalTag();
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
	Fl_Box* skylanderNameBox = new Fl_Box(12, 42, 100, 20, toyName);
	skylanderNameBox->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);

	Fl_Value_Input* iptCoins = new Fl_Value_Input(skylanderNameBox->x() + 100, skylanderNameBox->y() + 32, 100, 20, "Money");
	iptCoins->callback(onEditCoins, tag);
	iptCoins->value(tag->_coins);
	iptCoins->step(1);

	createIntValueField(iptExp, "Exp", iptCoins, _exp, onEditExp);
	createIntValueField(iptHat, "Hat", iptExp, _hatType, onEditHat);
	createIntValueField(iptHeroPoints, "Hero Points", iptHat, _heroPoints, onEditHeroPoints);
	createIntValueField(iptOwnerCount, "Owner Count", iptHeroPoints, _ownerCount, onEditOwnerCount);

	Fl_Menu_Item menuItems[] = {
		{ "&File",      0, 0, 0, FL_SUBMENU },
		//{ "&Open File", FL_COMMAND + 'o', NULL },
		{ "&Save File", FL_COMMAND + 's', onSave },
		{0},
		{0}
	};

	Fl_Menu_Bar *m = new Fl_Menu_Bar(0, 0, 640, 30);
	m->copy(menuItems);

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