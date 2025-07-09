/*
	File:
		PortalTagAsserts.inc

	Description:
		Assertions to make sure the PortalTag structs don't get messed up.
*/

// Helper macros
#define assert_offset(type, offset, field) static_assert(offsetof(type, field) == offset, #field " has invalid offset")
#define assert_size(type, size) static_assert(sizeof(type) == size, #type " has invalid size")

// PortalTagHeader assertions
assert_size(Runes::PortalTagHeader, 0x20);
assert_offset(Runes::PortalTagHeader, 0x00, _serial);
assert_offset(Runes::PortalTagHeader, 0x04, _bcc);
assert_offset(Runes::PortalTagHeader, 0x10, _toyType);
assert_offset(Runes::PortalTagHeader, 0x13, _importantByte);
assert_offset(Runes::PortalTagHeader, 0x14, _tradingCardId);
assert_offset(Runes::PortalTagHeader, 0x1C, _subType);
assert_offset(Runes::PortalTagHeader, 0x1E, _headerChecksum);

// PortalTagTimeOfDay assertions
assert_size(Runes::PortalTagTimeOfDay, 0x06);
assert_offset(Runes::PortalTagTimeOfDay, 0x00, _minute);
assert_offset(Runes::PortalTagTimeOfDay, 0x01, _hour);
assert_offset(Runes::PortalTagTimeOfDay, 0x02, _day);
assert_offset(Runes::PortalTagTimeOfDay, 0x03, _month);
assert_offset(Runes::PortalTagTimeOfDay, 0x04, _year);

// PortalTagData assertions
assert_size(Runes::PortalTagData, 0xB0);
assert_offset(Runes::PortalTagData, 0x00, _experience2011_low);
assert_offset(Runes::PortalTagData, 0x02, _experience2011_high);
assert_offset(Runes::PortalTagData, 0x03, _coinCount);
assert_offset(Runes::PortalTagData, 0x05, _cumulativeTime);
assert_offset(Runes::PortalTagData, 0x09, _areaSequence0);
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
assert_offset(Runes::PortalTagData, 0x72, _areaSequence1);
assert_offset(Runes::PortalTagData, 0x73, _experience2012);
assert_offset(Runes::PortalTagData, 0x75, _hat2012);
assert_offset(Runes::PortalTagData, 0x76, _flags2);
assert_offset(Runes::PortalTagData, 0x78, _experience2013);
assert_offset(Runes::PortalTagData, 0x7C, _hat2013);
assert_offset(Runes::PortalTagData, 0x7D, _trinketType);
assert_offset(Runes::PortalTagData, 0x7E, _hat2015);
assert_offset(Runes::PortalTagData, 0x80, _battlegroundsFlags);
assert_offset(Runes::PortalTagData, 0x84, _heroics2012_low);
assert_offset(Runes::PortalTagData, 0x86, _heroics2012_high);
assert_offset(Runes::PortalTagData, 0x87, _sgQuestsLow);
assert_offset(Runes::PortalTagData, 0x8F, _sgQuestsHigh);
assert_offset(Runes::PortalTagData, 0x97, _ssfQuestsLow);
assert_offset(Runes::PortalTagData, 0x9F, _ssfQuestsHigh);
