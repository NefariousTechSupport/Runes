#pragma once

#include <map>
#include <vector>

#include "ESkylandersGame.hpp"
#include "kTfbSpyroTag_DecoID.hpp"
#include "kTfbSpyroTag_ToyType.hpp"

namespace Runes
{
	class VariantIdentifier
	{
		public:
			const char* _variantText;
			const char* _toyName;
			ESkylandersGame _yearCode : 4;
			bool _lightCore : 1;
			bool _fullAltDeco : 1;
			bool _wowPow : 1;
			bool : 0;
			kTfbSpyroTag_DecoID _decoId : 8;
	};
	class FigureToyData
	{
		public:
			const char* _toyName;
			std::vector<VariantIdentifier*> _variants;
			kTfbSpyroTag_ToyType _toyType;
			VariantIdentifier* LookupVariant(uint16_t varId);
	};
	class ToyDataManager
	{
		public:
			std::map<kTfbSpyroTag_ToyType, FigureToyData*> _toyTypeLookup;
			static ToyDataManager* getInstance();
			FigureToyData* LookupCharacter(kTfbSpyroTag_ToyType toyType);
		private:
			static ToyDataManager* _Instance;
			ToyDataManager(const char* csvPath);
	};
}