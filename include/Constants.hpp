#pragma once

enum Constants
{
	kMagicMomentOwnerIdSize = 8,
	kShapeshifterPartNameSize = 7,
	kMagicMomentNameSize = 15,
	kSecretCodeLength = 12,
	kMoneyCap = 65535,
	kQuest1Shift = 0,
	kQuest1Mask = 0x3FF,
	kQuest2Shift = 10,
	kQuest2Mask = 0xF,
	kQuest3Shift = 14,
	kQuest3Mask = 0x1,
	kQuest4Shift = 15,
	kQuest4Mask = 0xF,
	kQuest5Shift = 19,
	kQuest5Mask = 0x1,
	kQuest6Shift = 20,
	kQuest6Mask = 0x1FFF,
	kQuest7Shift = 33,
	kQuest7Mask = 0xFF,
	kQuest8Shift = 41,
	kQuest8Mask = 0x1FF,
	kQuest9Shift = 50,
	kQuest9Mask = 0xFFFF,
	kDriverQuestTrackBits = 2,
	kDriverQuestStepBits = 3,
	kDriverQuestObjectiveBits = 6
};