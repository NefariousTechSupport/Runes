#pragma once

#include <cstdint>

#include "ESkylandersGame.hpp"
#include "kTfbSpyroTag_DecoID.hpp"
#include "kTfbSpyroTag_ToyType.hpp"
#include "kTfbSpyroTag_HatType.hpp"
#include "RfidTag.hpp"

// Documentation about figures:
//	Figures consist of 64 blocks of data, each block is 16 bytes
//	Every (4n + 3)th block is an access control block
//	The first two blocks make up the tag header
//	Blocks 


namespace Runes
{
#pragma pack(push, 1)
	//Size should be 0x20
	struct PortalTagHeader
	{
		/* 0x00 */ uint32_t _serial;
		/* 0x04 */ uint8_t _bcc;
		/* 0x05 */ uint8_t _manufacturerData[0x0B];
		/* 0x10 */ kTfbSpyroTag_ToyType _toyType;
		/* 0x12 */ uint8_t _toyType_high;
		/* 0x13 */ uint8_t _importantByte;		//MUST BE SET TO 0 OR THE GAME WILL REFUSE TO READ THIS TOY
		/* 0x14 */ uint32_t _tradingCardId1;
		/* 0x18 */ uint32_t _tradingCardId2;
		/* 0x1C */ uint16_t _subType;
		/* 0x1E */ uint16_t _headerChecksum;
	};
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
		/* 0x00 */ uint16_t _experience2011_low;
		/* 0x02 */ uint8_t _experience2011_high;
		/* 0x03 */ uint16_t _coinCount;
		/* 0x05 */ uint32_t _cumulativeTime;
		/* 0x09 */ uint8_t _areaSequence;
		/* 0x0C */ uint16_t _crcType3;
		/* 0x0A */ uint16_t _crcType2;
		/* 0x0E */ uint16_t _crcType1;

		/* 0x10 */ uint16_t _flags1_low;
		/* 0x12 */ uint8_t _flags1_high;
		/* 0x13 */ uint8_t _platformUse2011;
		/* 0x14 */ uint16_t _hat2011;
		/* 0x16 */ uint8_t _regionCountCoded;
		/* 0x17 */ uint8_t _platformUse2013;
		/* 0x18 */ uint8_t unk18[8];			//Maybe an owner id?

		/* 0x20 */ uint16_t _nickname[16];

		/* 0x40 */ PortalTagTimeOfDay _recentlyUsed;
		/* 0x46 */ uint32_t _heroics2011;
		/* 0x4A */ uint16_t _heroPoints;
		/* 0x4C */ uint8_t unk4C[3];
		/* 0x4F */ uint8_t _ownerCount;

		/* 0x50 */ PortalTagTimeOfDay _firstUsed;
		/* 0x56 */ uint8_t unk56[10];

		/* 0x60 */ uint8_t unk60[12];
		/* 0x6C */ uint32_t _challengeLevel;

		/* 0x70 */ uint16_t _crcType6;
		/* 0x72 */ uint8_t unk72;
		/* 0x73 */ uint16_t _experience2012;
		/* 0x75 */ uint8_t _hat2012;
		/* 0x76 */ uint16_t _flags2;
		/* 0x78 */ uint32_t _experience2013;
		/* 0x7C */ uint8_t _hat2013;
		/* 0x7D */ uint8_t unk7D;
		/* 0x7E */ uint8_t _hat2015;
		/* 0x7F */ uint8_t unk7F;

		/* 0x80 */ uint32_t _battlegroundsFlags;
		/* 0x84 */ uint16_t _heroics2012_low;
		/* 0x86 */ uint8_t _heroics2012_high;
		/* 0x87 */ uint64_t _sgQuestsLow;
		/* 0x8F */ uint8_t _sgQuestsHigh;

		/* 0x90 */ uint8_t unk90[7];
		/* 0x97 */ uint64_t _ssfQuestsLow;	//Also ssc driver quests
		/* 0x9F */ uint8_t _ssfQuestsHigh;

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

			void StoreHeader();
			void StoreTagData();
			void StoreMagicMoment();
			void StoreRemainingData();
			void FillOutputFromStoredData();
			void ReadFromFile(const char* fileName);
			void SaveToFile(const char* fileName);
			void RecalculateTagDataChecksums();
			static void DecodeSubtype(uint16_t varId, ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DecodeSubtype(ESkylandersGame* esg, bool* fullAltDeco, bool* wowPowFlag, bool* lightcore, kTfbSpyroTag_DecoID* decoId);
			void DebugPrintHeader();
			void DebugSaveTagData();
			bool isTrap();
		private:
			bool _tagHeaderStored;
			bool _tagDataStored;
			bool _tagMagicMomentStored;
			bool _tagRemainingDataStored;
			void StoreQuestsSwapForce();
			void StoreQuestsGiants();
	};
}

#define assert_offset(type, offset, field) static_assert(offsetof(type, field) == offset, #field " has invalid offset")
#define assert_size(type, size) static_assert(sizeof(type) == size, #type " has invalid size")

assert_size(Runes::PortalTagHeader, 0x20);
assert_offset(Runes::PortalTagHeader, 0x00, _serial);
assert_offset(Runes::PortalTagHeader, 0x04, _bcc);
assert_offset(Runes::PortalTagHeader, 0x10, _toyType);
assert_offset(Runes::PortalTagHeader, 0x13, _importantByte);
assert_offset(Runes::PortalTagHeader, 0x14, _tradingCardId1);
assert_offset(Runes::PortalTagHeader, 0x18, _tradingCardId2);
assert_offset(Runes::PortalTagHeader, 0x1C, _subType);
assert_offset(Runes::PortalTagHeader, 0x1E, _headerChecksum);

assert_size(Runes::PortalTagTimeOfDay, 0x06);
assert_offset(Runes::PortalTagTimeOfDay, 0x00, _minute);
assert_offset(Runes::PortalTagTimeOfDay, 0x01, _hour);
assert_offset(Runes::PortalTagTimeOfDay, 0x02, _day);
assert_offset(Runes::PortalTagTimeOfDay, 0x03, _month);
assert_offset(Runes::PortalTagTimeOfDay, 0x04, _year);

assert_size(Runes::PortalTagData, 0xB0);
assert_offset(Runes::PortalTagData, 0x00, _experience2011_low);
assert_offset(Runes::PortalTagData, 0x02, _experience2011_high);
assert_offset(Runes::PortalTagData, 0x03, _coinCount);
assert_offset(Runes::PortalTagData, 0x05, _cumulativeTime);
assert_offset(Runes::PortalTagData, 0x09, _areaSequence);
assert_offset(Runes::PortalTagData, 0x0A, _crcType3);
assert_offset(Runes::PortalTagData, 0x0C, _crcType2);
assert_offset(Runes::PortalTagData, 0x0E, _crcType1);
assert_offset(Runes::PortalTagData, 0x10, _flags1_low);
assert_offset(Runes::PortalTagData, 0x12, _flags1_high);
assert_offset(Runes::PortalTagData, 0x13, _platformUse2011);
assert_offset(Runes::PortalTagData, 0x14, _hat2011);
assert_offset(Runes::PortalTagData, 0x17, _platformUse2013);
assert_offset(Runes::PortalTagData, 0x20, _nickname);
assert_offset(Runes::PortalTagData, 0x40, _recentlyUsed);
assert_offset(Runes::PortalTagData, 0x46, _heroics2011);
assert_offset(Runes::PortalTagData, 0x4A, _heroPoints);
assert_offset(Runes::PortalTagData, 0x4F, _ownerCount);
assert_offset(Runes::PortalTagData, 0x50, _firstUsed);
assert_offset(Runes::PortalTagData, 0x70, _crcType6);
assert_offset(Runes::PortalTagData, 0x73, _experience2012);
assert_offset(Runes::PortalTagData, 0x75, _hat2012);
assert_offset(Runes::PortalTagData, 0x76, _flags2);
assert_offset(Runes::PortalTagData, 0x78, _experience2013);
assert_offset(Runes::PortalTagData, 0x7C, _hat2013);
assert_offset(Runes::PortalTagData, 0x7E, _hat2015);
assert_offset(Runes::PortalTagData, 0x80, _battlegroundsFlags);
assert_offset(Runes::PortalTagData, 0x84, _heroics2012_low);
assert_offset(Runes::PortalTagData, 0x86, _heroics2012_high);
assert_offset(Runes::PortalTagData, 0x87, _sgQuestsLow);
assert_offset(Runes::PortalTagData, 0x8F, _sgQuestsHigh);
assert_offset(Runes::PortalTagData, 0x97, _ssfQuestsLow);
assert_offset(Runes::PortalTagData, 0x9F, _ssfQuestsHigh);
