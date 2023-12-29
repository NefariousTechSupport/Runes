#pragma once

#include <map>
#include <vector>
#include <string>

#include "ESkylandersGame.hpp"
#include "kTfbSpyroTag_DecoID.hpp"
#include "kTfbSpyroTag_ToyType.hpp"
#include "EElementType.hpp"

namespace YAML
{
	class Node;
}

namespace Runes
{
	struct LocalizedString
	{
		std::string _en;
		std::string _fr;
		std::string _it;
		std::string _de;
		std::string _es;
		std::string _mx;
		std::string _nl;
		std::string _da;
		std::string _sv;
		std::string _fi;
		std::string _no;
		std::string _pt;
		std::string get();
	};
	class VariantIdentifier
	{
		public:
			LocalizedString _variantText;
			LocalizedString _toyName;
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
			LocalizedString _toyName;
			std::vector<VariantIdentifier*> _variants;
			kTfbSpyroTag_ToyType _toyType;
			EElementType _element;
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
			ToyDataManager(const char* yamlPath);
			void readCharacter(YAML::Node charNode);
			VariantIdentifier* readVariant(YAML::Node varNode);
			void readLocalizedString(LocalizedString* lstring, YAML::Node textNode);
	};
}