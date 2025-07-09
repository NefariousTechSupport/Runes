/*
	File:
		PortalTag.hpp

	Description:
		Data structures for the figures and a class for interfacing with figures.
*/

#ifndef RUNES_PORTAL_TAG_H
#define RUNES_PORTAL_TAG_H

#include <cstdint>

#include "ESkylandersGame.hpp"
#include "kTfbSpyroTag_DecoID.hpp"
#include "kTfbSpyroTag_ToyType.hpp"
#include "kTfbSpyroTag_HatType.hpp"
#include "kTfbSpyroTag_TrinketType.hpp"
#include "RfidTag.hpp"

// Documentation about figures:
//	Figures consist of 64 blocks of data, each block is 16 bytes
//	Every (4n + 3)th block is an access control block
//	The first two blocks make up the tag header
//	Blocks 


namespace Runes
{

// Disable alignment, these structs are meant to correspond to the figure data directly.
#pragma pack(push, 1)

	// Blocks 0x00 and 0x01 of the tag. Size must be 0x20
	struct PortalTagHeader
	{
		/* 0x00 */ uint32_t _serial;                 // Unique identifier for the figure.

		/* 0x04 */ uint8_t _bcc;
		/* 0x05 */ uint8_t _manufacturerData[0x0B];

		/* 0x10 */ kTfbSpyroTag_ToyType _toyType;    // Character ID of the figure
		/* 0x12 */ uint8_t _toyType_high;            // Character ID is actually 24 bits, this is never
		                                             //  used despite being read

		/* 0x13 */ uint8_t _importantByte;           // Not sure what this is, must be set to 0 or
		                                             //  SuperChargers (maybe others) will refuse to read it.

		/* 0x14 */ uint64_t _tradingCardId;          // Encoded webcode, 10 digit base 29 integer. The alphabet for it goes
		                                             //  as follows: 23456789BCDFGHJKLMNPQRSTVWXYZ

		/* 0x1C */ uint16_t _subType;                // Variant ID

		/* 0x1E */ uint16_t _headerChecksum;         // See SkylanderFormat.md
	};


	// Stores a date and time. Size must be 0x05
	struct PortalTagTimeOfDay
	{
		/* 0x00 */ uint8_t _minute;
		/* 0x01 */ uint8_t _hour;
		/* 0x02 */ uint8_t _day;
		/* 0x03 */ uint8_t _month;
		/* 0x04 */ uint16_t _year;
	};

	//Size should be 0xB0
	struct PortalTagData
	{
		/* 0x00 */ uint16_t _experience2011_low;     // This is just the Spyro's Adventure experience, caps at 33000.
		/* 0x02 */ uint8_t _experience2011_high;     // Experience is a 24 bit int for some reason.

		/* 0x03 */ uint16_t _coinCount;              // Money value for character, caps at 65535.

		/* 0x05 */ uint32_t _cumulativeTime;         // Total time on portal in seconds.

		/* 0x09 */ uint8_t _areaSequence0;           // See SkylanderFormat.md
		/* 0x0A */ uint16_t _crcType3;               // See SkylanderFormat.md 
		/* 0x0C */ uint16_t _crcType2;               // See SkylanderFormat.md
		/* 0x0E */ uint16_t _crcType1;               // See SkylanderFormat.md

		/* 0x10 */ uint16_t _flags1_low;             // See SkylanderFormat.md
		/* 0x12 */ uint8_t _flags1_high;             // See SkylanderFormat.md
		/* 0x13 */ uint8_t _platformUse2011;         // Platforms this figure has touched
		/* 0x14 */ uint16_t _hat2011;                // Current hat (only ssa hats)
		/* 0x16 */ uint8_t _regionCountCoded;
		/* 0x17 */ uint8_t _platformUse2013;         // Platforms this figure has touched
		/* 0x18 */ uint8_t unk18[8];                 // Maybe an owner id?

		/* 0x20 */ uint16_t _nickname[16];

		/* 0x40 */ PortalTagTimeOfDay _recentlyUsed; // The last time they were written to
		/* 0x46 */ uint32_t _heroics2011;            // Bitfield of heroics
		/* 0x4A */ uint16_t _heroPoints;             // Hero points, from skylanders universe, rest in peace
		/* 0x4C */ uint8_t unk4C[3];
		/* 0x4F */ uint8_t _ownerCount;

		/* 0x50 */ PortalTagTimeOfDay _firstUsed;    // Last time they were used after being reset
		/* 0x56 */ uint8_t unk56[10];

		/* 0x60 */ uint8_t unk60[12];
		/* 0x6C */ uint32_t _challengeLevel;

		/* 0x70 */ uint16_t _crcType6;               // See SkylanderFormat.md
		/* 0x72 */ uint8_t _areaSequence1;           // See SkylanderFormat.md
		/* 0x73 */ uint16_t _experience2012;         // Experience from giants, caps at either 63500 or 65535 depending on the character
		/* 0x75 */ uint8_t _hat2012;                 // Current hat (only giants hats)
		/* 0x76 */ uint16_t _flags2;                 // See SkylanderFormat.md
		/* 0x78 */ uint32_t _experience2013;         // Experience for ssf onwards, caps at 101000
		/* 0x7C */ uint8_t _hat2013;                 // Current hat (only swap force or trap team hats)
		/* 0x7D */ uint8_t _trinketType;             // Trinket type
		/* 0x7E */ uint8_t _hat2015;                 // Current hat (superchargers hats)
		/* 0x7F */ uint8_t unk7F;

		/* 0x80 */ uint32_t _battlegroundsFlags;     // flags exclusive to battlegrounds
		/* 0x84 */ uint16_t _heroics2012_low;        // giants heroic challenges
		/* 0x86 */ uint8_t _heroics2012_high;        // giants heroic challenges
		/* 0x87 */ uint64_t _sgQuestsLow;            // giants quests
		/* 0x8F */ uint8_t _sgQuestsHigh;            // giants quests

		/* 0x90 */ uint8_t unk90[7];
		/* 0x97 */ uint64_t _ssfQuestsLow;	          // Swap force quests, also ssc driver quests
		/* 0x9F */ uint8_t _ssfQuestsHigh;            // Swap force quests, also ssc driver quests

		/* 0xA0 */ uint8_t unkA0[16];

		uint32_t getExperience();
		void setExperience(uint32_t exp);
		uint16_t getMoney();
		void setMoney(uint16_t exp);
		//uint32_t getCumulativeTime();
		//void setCumulativeTime(uint32_t);
		kTfbSpyroTag_HatType getHat();
		void setHat(kTfbSpyroTag_HatType hat);
		//uint16_t getPlatformBits();
		//void setPlatformBits(uint16_t platformBits);
		//uint16_t* getNickname();
		//void setNickname(uint16_t nickname);
		//PortalTagTimeOfDay getRecentTime();
		//void setRecentTime(PortalTagTimeOfDay recentTime);
		//uint64_t getHeroics();
		//void setHeroics(uint64_t heroics);
		//uint8_t getOwnerCount();
		//void setOwnerCount(uint8_t heroics);
		//PortalTagTimeOfDay getFirstTime();
		//void setFirstTime(PortalTagTimeOfDay firstTime);
	};

#pragma pack(pop)

	enum Upgrade : uint8_t
	{
		// Numeric value is bit index
		kUpgradePathChoiceMade        = 0,
		kUpgradeSelectedPath          = 1,

		kUpgradeBase1                 = 2,
		kUpgradeBase2                 = 3,
		kUpgradeBase3                 = 4,
		kUpgradeBase4                 = 5,

		kUpgradeActivePathStart       = 6,
		kUpgradeActivePathUpgrade1    = 6,
		kUpgradeActivePathUpgrade2    = 7,
		kUpgradeActivePathUpgrade3    = 8,
		kUpgradeSoulgem               = 9,
		kUpgradeWowPow                = 10,

		kUpgradeAltPathStart          = 11,
		kUpgradeAltPathUpgrade1       = 11,
		kUpgradeAltPathUpgrade2       = 12,
		kUpgradeAltPathUpgrade3       = 13,

		// alternate ways of grabing a path only upgrade
		kUpgradeSpecificPath          = 0x80,
		kUpgradeSpecificPathShift     = 6,
		kUpgradeSpecificPathPrimary   = kUpgradeSpecificPath | (0 << kUpgradeSpecificPathShift),
		kUpgradeSpecificPathSecondary = kUpgradeSpecificPath | (1 << kUpgradeSpecificPathShift),
		kUpgradePath1Upgrade1         = kUpgradeSpecificPathPrimary   | 0,
		kUpgradePath1Upgrade2         = kUpgradeSpecificPathPrimary   | 1,
		kUpgradePath1Upgrade3         = kUpgradeSpecificPathPrimary   | 2,
		kUpgradePath2Upgrade1         = kUpgradeSpecificPathSecondary | 0,
		kUpgradePath2Upgrade2         = kUpgradeSpecificPathSecondary | 1,
		kUpgradePath2Upgrade3         = kUpgradeSpecificPathSecondary | 2,
	};

	enum UpgradePath : uint8_t
	{
		// Numeric value is the path index
		kUpgradePathPrimary           = 0,
		kUpgradePathSecondary         = 1
	};


	// This class serves as an abstraction, allowing one to edit various parts of the figures
	// without needing to deal without needing to deal with the way it was coded and such.
	class PortalTag
	{
		public:
			PortalTagHeader _tagHeader;
			PortalTagData _tagData;
			RfidTag* _rfidTag;

			uint32_t _serial;
			kTfbSpyroTag_ToyType _toyType;
			uint16_t _subType;

			uint32_t _exp;
			uint16_t _coins;
			uint32_t _cumulativeTime;
			uint16_t _platformUse;
			uint64_t _heroics;
			kTfbSpyroTag_HatType _hatType;
			kTfbSpyroTag_TrinketType _trinketType;
			PortalTagTimeOfDay _firstUsed;
			PortalTagTimeOfDay _recentlyUsed;
			uint16_t _heroPoints;
			uint8_t _ownerCount;
			uint8_t _ownerId[9];
			uint16_t _giantsQuests[9];
			uint16_t _swapforceQuests[9];
			uint16_t _upgrades;
			uint8_t _elementCollectionCounts[2];
			uint8_t _elementCollection;
			uint8_t _accoladeRanks[2];
			char _webCode[12];
			uint16_t _nickname[16];

			~PortalTag();

			uint8_t ComputeLevel();
			void StoreHeader();
			void StoreTagData();
			void StoreMagicMoment();
			void StoreRemainingData();
			void FillOutputFromStoredData();
			void ReadFromFile(const char* fileName);
			void SaveToFile(const char* fileName);
			void RecalculateTagDataChecksums();
			static void DecodeSubtype(uint16_t varId, ESkylandersGame* esg, bool* fullAltDeco, bool* reposeFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DecodeSubtype(ESkylandersGame* esg, bool* fullAltDeco, bool* reposeFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DebugPrintHeader();
			void DebugSaveTagData();
			bool isTrap();
			bool isVehicle();
			uint8_t GetUpgrade(Upgrade upgrade) const;
			void SetUpgrade(Upgrade upgrade, uint8_t value);
			bool GetHeroic(uint8_t heroic) const;
			void SetHeroic(uint8_t heroic, bool value);

		private:
			bool _tagHeaderStored;
			bool _tagDataStored;
			bool _tagMagicMomentStored;
			bool _tagRemainingDataStored;
			void StoreQuestsSwapForce();
			void StoreQuestsGiants();
			void FillQuestsSwapForce();
			void FillQuestsGiants();
			void getQuestsGiantsElementalBits(uint8_t* bits);
			uint8_t DecodeUpgradeEnum(Upgrade upgrade) const;
	};
}

#include "PortalTagAsserts.inc"

#endif // RUNES_PORTAL_TAG_H