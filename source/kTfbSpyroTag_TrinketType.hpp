#pragma once

#include <cstdint>
#include <string_view>

enum kTfbSpyroTag_TrinketType : uint16_t
{
	kTfbSpyroTag_Trinket_NONE = 0,
	kTfbSpyroTag_Trinket_LuckyTie = 1,
	kTfbSpyroTag_Trinket_Bubble = 2,
	kTfbSpyroTag_Trinket_DarkWaterDaisy = 3,
	kTfbSpyroTag_Trinket_VoteForCyclops = 4,
	kTfbSpyroTag_Trinket_DragonHorn = 5,
	kTfbSpyroTag_Trinket_Iris = 6,
	kTfbSpyroTag_Trinket_KuckooKazoo = 7,
	kTfbSpyroTag_Trinket_Rune = 8,
	kTfbSpyroTag_Trinket_UllyssesUniclops = 9,
	kTfbSpyroTag_Trinket_BillyBison = 10,
	kTfbSpyroTag_Trinket_StealthElfGift = 11,
	kTfbSpyroTag_Trinket_LizardLilly = 12,
	kTfbSpyroTag_Trinket_PiratePinwheel = 13,
	kTfbSpyroTag_Trinket_BubbleBlower = 14,
	kTfbSpyroTag_Trinket_MedalOfHeroism = 15,
	kTfbSpyroTag_Trinket_MedalOfCourage = 16,
	kTfbSpyroTag_Trinket_MedalOfValiance = 17,
	kTfbSpyroTag_Trinket_MedalOfGallantry = 18,
	kTfbSpyroTag_Trinket_MedalOfMettle = 19,
	kTfbSpyroTag_Trinket_WingedMedalOfBravery = 20,
	kTfbSpyroTag_Trinket_SeadogSeashell = 21,
	kTfbSpyroTag_Trinket_Sunflower = 22,
	kTfbSpyroTag_Trinket_TeddyCyclops = 23,
	kTfbSpyroTag_Trinket_GooFactoryGear = 24,
	kTfbSpyroTag_Trinket_ElementalOpal = 25,
	kTfbSpyroTag_Trinket_ElementalRadiant = 26,
	kTfbSpyroTag_Trinket_ElementalDiamond = 27,
	kTfbSpyroTag_Trinket_CyclopsSpinner = 28,
	kTfbSpyroTag_Trinket_WilikinWindmill = 29,
	kTfbSpyroTag_Trinket_TimeTownTicker = 30,
	kTfbSpyroTag_Trinket_BigBowOfBoom = 31,
	kTfbSpyroTag_Trinket_MabusMedallion = 32,
	kTfbSpyroTag_Trinket_SpyrosShield = 33,
	kTfbSpyroTag_Trinket_MAX = 33,
};

extern const char* trinketNames_en[kTfbSpyroTag_Trinket_MAX + 1];